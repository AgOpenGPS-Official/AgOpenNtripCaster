using System.Net;
using System.Net.Sockets;
using System.Text;
using Microsoft.AspNetCore.SignalR;
using Microsoft.EntityFrameworkCore;
using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Hubs;
using AgOpenNtripCaster.Server.Models.DTOs;
using AgOpenNtripCaster.Server.Models.Entities;
using AgOpenNtripCaster.Server.Services.Auth;
using AgOpenNtripCaster.Server.Services.Email;

namespace AgOpenNtripCaster.Server.Services.NTRIP;

/// <summary>
/// NTRIP Server core service
/// Listens on port 2101 for:
/// - GNSS source connections (push RTCM data)
/// - NTRIP client connections (pull RTCM data)
/// - Sourcetable requests (GET /)
/// </summary>
public class NtripServerService : IHostedService
{
    private readonly ILogger<NtripServerService> _logger;
    private readonly IServiceProvider _serviceProvider;
    private readonly ConnectionPool _connectionPool;
    private readonly IHubContext<NtripHub> _hubContext;
    private readonly IEmailService _emailService;
    private readonly IEmailTriggerSettingsService _emailTriggerSettingsService;
    private readonly Dictionary<string, RingBuffer> _mountPointBuffers;
    private readonly Dictionary<string, string> _clientSessionIds; // clientId -> sessionId mapping

    private TcpListener? _tcpListener;
    private CancellationTokenSource? _cancellationTokenSource;
    private Task? _acceptTask;

    private const int Port = 2101;
    private const int ListenBacklog = 128;

    public NtripServerService(
        ILogger<NtripServerService> logger,
        IServiceProvider serviceProvider,
        ConnectionPool connectionPool,
        IHubContext<NtripHub> hubContext,
        IEmailService emailService,
        IEmailTriggerSettingsService emailTriggerSettingsService)
    {
        _logger = logger;
        _serviceProvider = serviceProvider;
        _connectionPool = connectionPool;
        _hubContext = hubContext;
        _emailService = emailService;
        _emailTriggerSettingsService = emailTriggerSettingsService;
        _mountPointBuffers = new Dictionary<string, RingBuffer>();
        _clientSessionIds = new Dictionary<string, string>();
    }

    public async Task StartAsync(CancellationToken cancellationToken)
    {
        try
        {
            _logger.LogInformation("Starting NTRIP Server on port {Port}...", Port);

            // Clean up orphaned ClientSessions on startup
            // All previous sessions are invalid since server just restarted
            await CleanupOrphanedSessionsAsync(cancellationToken);

            _tcpListener = new TcpListener(IPAddress.Any, Port);
            _tcpListener.Start(ListenBacklog);

            _cancellationTokenSource = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken);
            _acceptTask = AcceptConnectionsAsync(_cancellationTokenSource.Token);

            _logger.LogInformation("NTRIP Server started on port {Port}", Port);
            await Task.CompletedTask;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to start NTRIP Server");
            throw;
        }
    }

    public async Task StopAsync(CancellationToken cancellationToken)
    {
        try
        {
            _logger.LogInformation("Stopping NTRIP Server...");

            _tcpListener?.Stop();
            _cancellationTokenSource?.Cancel();

            if (_acceptTask != null)
            {
                await _acceptTask;
            }

            _logger.LogInformation("NTRIP Server stopped");
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error stopping NTRIP Server");
        }
    }

    /// <summary>
    /// Clean up orphaned ClientSessions and SourceConnections on server startup
    /// Any session/connection without DisconnectedAt is invalid after server restart
    /// </summary>
    private async Task CleanupOrphanedSessionsAsync(CancellationToken cancellationToken)
    {
        try
        {
            using var scope = _serviceProvider.CreateScope();
            var dbContext = scope.ServiceProvider.GetRequiredService<ApplicationDbContext>();

            var now = DateTime.UtcNow;

            // Clean up orphaned ClientSessions (DisconnectedAt == null)
            var orphanedSessions = await dbContext.ClientSessions
                .Where(cs => cs.DisconnectedAt == null)
                .ToListAsync(cancellationToken);

            if (orphanedSessions.Count > 0)
            {
                _logger.LogWarning("🧹 Cleaning up {Count} orphaned ClientSessions from previous server run", orphanedSessions.Count);

                foreach (var session in orphanedSessions)
                {
                    session.DisconnectedAt = now;
                    session.Status = ClientStreamStatus.Disconnected;
                }

                await dbContext.SaveChangesAsync(cancellationToken);
                _logger.LogWarning("🧹 ClientSessions cleanup complete! Marked {Count} sessions as disconnected", orphanedSessions.Count);
            }
            else
            {
                _logger.LogInformation("✅ No orphaned ClientSessions to clean up");
            }

            // Clean up orphaned SourceConnections (DisconnectedAt == null)
            var orphanedConnections = await dbContext.SourceConnections
                .Where(sc => sc.DisconnectedAt == null)
                .ToListAsync(cancellationToken);

            if (orphanedConnections.Count > 0)
            {
                _logger.LogWarning("🧹 Cleaning up {Count} orphaned SourceConnections from previous server run", orphanedConnections.Count);

                foreach (var connection in orphanedConnections)
                {
                    connection.DisconnectedAt = now;
                }

                await dbContext.SaveChangesAsync(cancellationToken);
                _logger.LogWarning("🧹 SourceConnections cleanup complete! Marked {Count} connections as disconnected", orphanedConnections.Count);
            }
            else
            {
                _logger.LogInformation("✅ No orphaned SourceConnections to clean up");
            }
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error cleaning up orphaned sessions/connections on startup");
            // Don't throw - let server start even if cleanup fails
        }
    }

    /// <summary>
    /// Accept incoming TCP connections
    /// </summary>
    private async Task AcceptConnectionsAsync(CancellationToken cancellationToken)
    {
        while (!cancellationToken.IsCancellationRequested)
        {
            try
            {
                var tcpClient = await _tcpListener!.AcceptTcpClientAsync(cancellationToken);
                var clientId = Guid.NewGuid().ToString();

                // Handle connection in background
                _ = HandleConnectionAsync(clientId, tcpClient, cancellationToken);
            }
            catch (OperationCanceledException)
            {
                // Server is shutting down
                break;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error accepting connection");
            }
        }
    }

    /// <summary>
    /// Handle incoming connection (source or client)
    /// </summary>
    private async Task HandleConnectionAsync(string clientId, TcpClient tcpClient, CancellationToken cancellationToken)
    {
        try
        {
            using (tcpClient)
            using (var stream = tcpClient.GetStream())
            using (var reader = new StreamReader(stream, Encoding.ASCII, leaveOpen: true))
            {
                // Read first line to determine connection type
                string? requestLine = await reader.ReadLineAsync(cancellationToken);
                if (string.IsNullOrEmpty(requestLine))
                {
                    _logger.LogWarning("Empty request from {ClientId}", clientId);
                    return;
                }

                _logger.LogInformation("Received request from {ClientId}: {Request}", clientId, requestLine);

                // Determine connection type
                if (requestLine.StartsWith("SOURCE"))
                {
                    // GNSS station connection
                    await HandleSourceConnectionAsync(clientId, tcpClient, reader, requestLine, cancellationToken);
                }
                else if (requestLine.StartsWith("GET"))
                {
                    // Client connection
                    await HandleClientConnectionAsync(clientId, tcpClient, reader, requestLine, cancellationToken);
                }
                else
                {
                    _logger.LogWarning("Unknown request type from {ClientId}: {Request}", clientId, requestLine);
                    await SendResponseAsync(stream, "400 Bad Request\r\n\r\n", cancellationToken);
                }
            }
        }
        catch (OperationCanceledException)
        {
            // Cancellation is expected
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error handling connection {ClientId}", clientId);
        }
    }

    /// <summary>
    /// Handle GNSS source connection
    /// SOURCE password mountpoint
    /// (e.g., "SOURCE R8QGsWgrPE test")
    /// </summary>
    private async Task HandleSourceConnectionAsync(
        string sourceId,
        TcpClient tcpClient,
        StreamReader reader,
        string requestLine,
        CancellationToken cancellationToken)
    {
        string? mountPointName = null;
        try
        {
            // Parse: "SOURCE password mountpoint"
            var parts = requestLine.Split(' ');
            if (parts.Length < 3)
            {
                _logger.LogWarning("Invalid SOURCE format from {ClientId}: {Request}", sourceId, requestLine);
                await SendResponseAsync(tcpClient.GetStream(), "400 Bad Request\r\n\r\n", cancellationToken);
                return;
            }

            var password = parts[1];
            mountPointName = parts[2];

            // Authenticate source
            using var scope = _serviceProvider.CreateScope();
            var authService = scope.ServiceProvider.GetRequiredService<NtripAuthenticationService>();
            var mountPoint = await authService.AuthenticateSourceAsync(mountPointName, password);

            if (mountPoint == null)
            {
                await SendResponseAsync(tcpClient.GetStream(), "401 Unauthorized\r\n\r\n", cancellationToken);
                return;
            }

            // Register source
            if (!_connectionPool.RegisterSource(sourceId, mountPointName, tcpClient))
            {
                await SendResponseAsync(tcpClient.GetStream(), "503 Service Unavailable\r\n\r\n", cancellationToken);
                return;
            }

            // Send success response
            await SendResponseAsync(tcpClient.GetStream(), "200 OK\r\n\r\n", cancellationToken);

            // Create SourceConnection in database
            var sourceConnectionId = await CreateSourceConnectionAsync(mountPointName, cancellationToken);

            // Create NEW ring buffer for this mount point (clear old data from previous source)
            // Each source connection gets a fresh buffer
            var ringBuffer = new RingBuffer();
            _mountPointBuffers[mountPointName] = ringBuffer;
            _logger.LogInformation("Source {SourceId} created new ring buffer for mount point {MountPointName}", sourceId, mountPointName);

            // Broadcast source data directly to all connected clients (real-time piping)
            await HandleSourceStreamAsync(sourceId, ringBuffer, reader, tcpClient.GetStream(), mountPointName, cancellationToken);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error handling source connection {SourceId}", sourceId);
        }
        finally
        {
            _connectionPool.UnregisterSource(sourceId);
            // Mark connection as disconnected
            // Note: SourceConnectionId is not stored, so we mark the latest one for this mountpoint
            await MarkSourceConnectionDisconnectedAsync(mountPointName, cancellationToken);
        }
    }

    /// <summary>
    /// Handle client connection
    /// GET /STATION_A HTTP/1.1
    /// Authorization: Basic base64(username:password)
    /// </summary>
    private async Task HandleClientConnectionAsync(
        string clientId,
        TcpClient tcpClient,
        StreamReader reader,
        string requestLine,
        CancellationToken cancellationToken)
    {
        try
        {
            // Parse: "GET /STATION_A HTTP/1.1"
            var parts = requestLine.Split(' ');
            if (parts.Length < 2)
            {
                await SendResponseAsync(tcpClient.GetStream(), "400 Bad Request\r\n\r\n", cancellationToken);
                return;
            }

            var mountPointPath = parts[1];
            var mountPointName = mountPointPath.TrimStart('/');

            // Empty path = sourcetable request
            if (string.IsNullOrEmpty(mountPointName))
            {
                await HandleSourcetableRequestAsync(tcpClient.GetStream(), cancellationToken);
                return;
            }

            // Read Authorization header
            string? authHeader = null;
            string? line;
            while ((line = await reader.ReadLineAsync(cancellationToken)) != null)
            {
                if (string.IsNullOrEmpty(line))
                    break; // End of headers

                if (line.StartsWith("Authorization:", StringComparison.OrdinalIgnoreCase))
                {
                    authHeader = line;
                }
            }

            // Extract credentials from Basic auth
            (string? username, string? password) = ExtractBasicAuth(authHeader);
            if (username == null || password == null)
            {
                await SendResponseAsync(tcpClient.GetStream(), "401 Unauthorized\r\n\r\n", cancellationToken);
                return;
            }

            // Authenticate client
            using var scope = _serviceProvider.CreateScope();
            var authService = scope.ServiceProvider.GetRequiredService<NtripAuthenticationService>();
            var authResult = await authService.AuthenticateClientAsync(username, password, mountPointName);

            if (!authResult.Success)
            {
                await SendResponseAsync(tcpClient.GetStream(), "401 Unauthorized\r\n\r\n", cancellationToken);
                return;
            }

            // Register client
            if (!_connectionPool.RegisterClient(clientId, mountPointName, username, tcpClient))
            {
                await SendResponseAsync(tcpClient.GetStream(), "503 Service Unavailable\r\n\r\n", cancellationToken);
                return;
            }

            // Send success response
            await SendResponseAsync(tcpClient.GetStream(), "200 OK\r\n\r\n", cancellationToken);

            // Create ClientSession in database
            var clientIpAddress = (tcpClient.Client.RemoteEndPoint as IPEndPoint)?.Address.ToString();
            var sessionId = await CreateClientSessionAsync(username, clientId, mountPointName, clientIpAddress, cancellationToken);

            if (!string.IsNullOrEmpty(sessionId))
            {
                _clientSessionIds[clientId] = sessionId;
            }

            // Check if ring buffer exists for this mount point
            if (!_mountPointBuffers.ContainsKey(mountPointName))
            {
                _mountPointBuffers[mountPointName] = new RingBuffer();
            }

            var ringBuffer = _mountPointBuffers[mountPointName];

            // Stream RTCM data to client (with position tracking)
            await HandleClientStreamAsync(clientId, ringBuffer, reader, tcpClient.GetStream(), cancellationToken);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error handling client connection {ClientId}", clientId);
        }
        finally
        {
            // Mark session as disconnected
            if (_clientSessionIds.TryGetValue(clientId, out var sessionId))
            {
                await MarkClientSessionDisconnectedAsync(sessionId, cancellationToken, clientId);
                _clientSessionIds.Remove(clientId);
            }

            _connectionPool.UnregisterClient(clientId);
        }
    }

    /// <summary>
    /// Handle source streaming - read RTCM from source, write to ring buffer, parse messages for station position
    /// </summary>
    private async Task HandleSourceStreamAsync(
        string sourceId,
        RingBuffer ringBuffer,
        StreamReader reader,
        NetworkStream stream,
        string mountPointName,
        CancellationToken cancellationToken)
    {
        var buffer = new byte[4096];
        var rtcmBuffer = new List<byte>();  // Buffer for collecting RTCM message chunks
        _logger.LogInformation("Source {SourceId} starting stream", sourceId);

        try
        {
            while (!cancellationToken.IsCancellationRequested)
            {
                // Read from source
                int bytesRead = await stream.ReadAsync(buffer, 0, buffer.Length, cancellationToken);
                if (bytesRead == 0)
                {
                    _logger.LogInformation("Source {SourceId} disconnected (EOF)", sourceId);
                    break;
                }

                // Write to ring buffer
                var data = buffer.AsSpan(0, bytesRead).ToArray();
                ringBuffer.WriteData(data);

                // Add to RTCM buffer and try to parse complete messages
                rtcmBuffer.AddRange(data);
                // TODO: Temporarily disabled RTCM1005 parsing until bit offsets are fixed
                // await ParseRtcmMessagesAsync(rtcmBuffer, mountPointName, cancellationToken);

                // Update statistics
                var sourceConnection = _connectionPool.GetSourceForMountPoint(
                    _connectionPool._sourceConnections.Values.FirstOrDefault(s => s.Id == sourceId)?.MountPointName ?? "");
                if (sourceConnection != null)
                {
                    sourceConnection.BytesReceived += bytesRead;
                }
            }
        }
        catch (OperationCanceledException)
        {
            _logger.LogInformation("Source {SourceId} stream cancelled", sourceId);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error in source stream {SourceId}", sourceId);
        }
    }

    /// <summary>
    /// Parse RTCM messages from buffer, extract complete messages, and update MountPoint position if RTCM1005 detected
    /// Handles message fragmentation across multiple TCP packets
    /// </summary>
    private async Task ParseRtcmMessagesAsync(List<byte> rtcmBuffer, string mountPointName, CancellationToken cancellationToken)
    {
        try
        {
            // Process complete RTCM messages from the buffer
            while (rtcmBuffer.Count >= 6)  // RTCM3 header is 3 bytes minimum
            {
                // Look for RTCM3 preamble (0xD3)
                int preambleIndex = -1;
                for (int i = 0; i < rtcmBuffer.Count; i++)
                {
                    if (rtcmBuffer[i] == 0xD3)
                    {
                        preambleIndex = i;
                        break;
                    }
                }

                // No preamble found, clear buffer (corrupted data)
                if (preambleIndex < 0)
                {
                    _logger.LogDebug("No RTCM3 preamble found in buffer of {Count} bytes for {MountPointName}", rtcmBuffer.Count, mountPointName);
                    rtcmBuffer.Clear();
                    return;
                }

                // Remove data before preamble
                if (preambleIndex > 0)
                {
                    rtcmBuffer.RemoveRange(0, preambleIndex);
                }

                // Extract message length from RTCM3 header (bits 14-23 of first 3 bytes)
                if (rtcmBuffer.Count < 3)
                    return;  // Not enough data yet

                int length = ((rtcmBuffer[1] & 0x03) << 8) | rtcmBuffer[2];
                int messageSize = 3 + length + 3;  // preamble(3) + payload(length) + checksum(3)

                // Not enough data for complete message yet
                if (rtcmBuffer.Count < messageSize)
                    return;

                // Extract complete message
                var messageData = rtcmBuffer.GetRange(0, messageSize).ToArray();
                rtcmBuffer.RemoveRange(0, messageSize);

                // Parse message
                await ParseAndUpdateRtcmDataAsync(messageData, mountPointName, cancellationToken);
            }
        }
        catch (Exception ex)
        {
            _logger.LogDebug(ex, "Error processing RTCM buffer for {MountPointName}", mountPointName);
            rtcmBuffer.Clear();  // Clear on error to prevent stuck state
        }
    }

    /// <summary>
    /// Parse single RTCM message and update MountPoint position if RTCM1005 detected
    /// </summary>
    private async Task ParseAndUpdateRtcmDataAsync(byte[] messageData, string mountPointName, CancellationToken cancellationToken)
    {
        try
        {
            // Parse the RTCM message
            var parseResult = RtcmMessageParser.ParseRtcmMessage(messageData);

            // If RTCM1005 message detected and contains position data
            if (parseResult?.Position1005 != null)
            {
                var position = parseResult.Position1005;
                _logger.LogInformation(
                    "RTCM1005 message detected for {MountPointName}: RefStation={RefStationId}, Lat={Lat}, Lon={Lon}",
                    mountPointName, position.ReferenceStationId, position.Latitude, position.Longitude);

                // Update mount point with RTCM position data
                await UpdateMountPointPositionAsync(
                    mountPointName,
                    position.Latitude,
                    position.Longitude,
                    position.ReferenceStationId,
                    cancellationToken);
            }

            // Log detected format if available
            if (!string.IsNullOrEmpty(parseResult?.DetectedFormat))
            {
                _logger.LogDebug("Detected RTCM format: {Format} for {MountPointName}",
                    parseResult.DetectedFormat, mountPointName);
            }
        }
        catch (Exception ex)
        {
            _logger.LogDebug(ex, "Error parsing RTCM message for {MountPointName}", mountPointName);
        }
    }

    /// <summary>
    /// Handle client streaming - read RTCM from ring buffer, send to client
    /// Also handles position frames from client
    /// </summary>
    private async Task HandleClientStreamAsync(
        string clientId,
        RingBuffer ringBuffer,
        StreamReader reader,
        NetworkStream stream,
        CancellationToken cancellationToken)
    {
        // Smaller buffer to send RTCM data in reasonable chunks (not 4KB at once)
        var rtcmBuffer = new byte[512];

        // Start at CURRENT position to avoid dumping whole buffer at once
        // This gives us NEW data only, preventing client overwhelm
        var readPos = ringBuffer.GetCurrentPosition();

        var lastPositionTime = DateTime.MinValue;  // Position is optional
        var hasReceivedPosition = false;  // Track if we've received any position
        const int MaxPositionAgeSec = 15;

        _logger.LogInformation("Client {ClientId} starting stream at current position", clientId);

        try
        {
            // Start reading positions and streaming data concurrently
            var positionTask = ReadPositionFramesAsync(clientId, reader, cancellationToken);
            var streamTask = StreamRtcmDataAsync();

            await Task.WhenAll(positionTask, streamTask);
        }
        catch (OperationCanceledException)
        {
            _logger.LogInformation("Client {ClientId} stream cancelled", clientId);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error in client stream {ClientId}", clientId);
        }

        async Task ReadPositionFramesAsync(string cId, StreamReader sr, CancellationToken ct)
        {
            try
            {
                string? line;
                int lineCount = 0;
                while ((line = await sr.ReadLineAsync(ct)) != null)
                {
                    lineCount++;

                    // Parse NMEA GPGGA sentences: $GPGGA,time,lat,N/S,lon,E/W,...
                    if (line.StartsWith("$GPGGA"))
                    {
                        _logger.LogInformation("📨 GPGGA line: {Line}", line);

                        var parts = line.Split(',');
                        _logger.LogInformation("🔍 GPGGA parts count: {Count}", parts.Length);
                        if (parts.Length >= 6)
                        {
                            _logger.LogInformation("🔍 parts[2]={Lat}, parts[3]={LatDir}, parts[4]={Lon}, parts[5]={LonDir}",
                                parts[2], parts[3], parts[4], parts[5]);

                            // Parse latitude (DDMM.MMMM format)
                            // IMPORTANT: Use InvariantCulture so "5242.000" is parsed as 5242.0, not 5242000
                            if (double.TryParse(parts[2], System.Globalization.NumberStyles.Any, System.Globalization.CultureInfo.InvariantCulture, out var latValue) &&
                                (parts[3] == "N" || parts[3] == "S"))
                            {
                                _logger.LogInformation("✅ Latitude parsed: latValue={LatValue}", latValue);

                                // Parse longitude (DDDMM.MMMM format)
                                if (double.TryParse(parts[4], System.Globalization.NumberStyles.Any, System.Globalization.CultureInfo.InvariantCulture, out var lonValue) &&
                                    (parts[5] == "E" || parts[5] == "W"))
                                {
                                    _logger.LogInformation("✅ Longitude parsed: lonValue={LonValue}", lonValue);

                                    // INLINE conversion DDMM.MMMM to decimal degrees
                                    _logger.LogError("🔥🔥🔥 INLINE CONVERSION START: latValue={LatValue}, lonValue={LonValue}", latValue, lonValue);

                                    // Latitude conversion
                                    int latDegrees = (int)(latValue / 100.0);
                                    double latMinutes = latValue - (latDegrees * 100.0);
                                    double decimalLat = latDegrees + (latMinutes / 60.0);
                                    if (parts[3] == "S") decimalLat = -decimalLat;

                                    // Longitude conversion
                                    int lonDegrees = (int)(lonValue / 100.0);
                                    double lonMinutes = lonValue - (lonDegrees * 100.0);
                                    double decimalLon = lonDegrees + (lonMinutes / 60.0);
                                    if (parts[5] == "W") decimalLon = -decimalLon;

                                    _logger.LogError("🔥 CONVERTED: latDegrees={LD}, latMinutes={LM}, decimalLat={DL} | lonDegrees={LOD}, lonMinutes={LOM}, decimalLon={DOL}",
                                        latDegrees, latMinutes, decimalLat, lonDegrees, lonMinutes, decimalLon);

                                    var acc = 5.0; // Default accuracy for GPGGA

                                    // Always process (no null checks since we just calculated)
                                    {
                                        _logger.LogError("🔥 SENDING TO SIGNALR: decimalLat={Lat}, decimalLon={Lon}", decimalLat, decimalLon);

                                        lastPositionTime = DateTime.UtcNow;
                                        if (!hasReceivedPosition)
                                        {
                                            hasReceivedPosition = true;
                                            _logger.LogInformation("Client {ClientId} first position received: {Lat},{Lon}", cId, decimalLat, decimalLon);
                                        }

                                        var clientInfo = _connectionPool.GetClient(cId);
                                        if (clientInfo != null)
                                        {
                                            clientInfo.LastLatitude = decimalLat;
                                            clientInfo.LastLongitude = decimalLon;
                                            clientInfo.LastAccuracy = acc;
                                            clientInfo.LastPositionAt = lastPositionTime;

                                            // Update ClientSession and broadcast via SignalR
                                            if (_clientSessionIds.TryGetValue(cId, out var sessionId))
                                            {
                                                await UpdateClientSessionPositionAsync(sessionId, decimalLat, decimalLon, acc, ct);
                                            }

                                            // Broadcast position update via SignalR
                                            var positionUpdate = new ClientPositionUpdate
                                            {
                                                ClientId = cId,
                                                Username = clientInfo.Username,
                                                MountPoint = clientInfo.MountPointName,
                                                Latitude = decimalLat,
                                                Longitude = decimalLon,
                                                Accuracy = acc,
                                                Timestamp = lastPositionTime
                                            };

                                            _logger.LogError("🔥 FINAL PAYLOAD: Latitude={Lat}, Longitude={Lon}", positionUpdate.Latitude, positionUpdate.Longitude);

                                            await _hubContext.Clients.All.SendAsync("ClientPositionUpdated", positionUpdate, ct);
                                        }
                                        else
                                        {
                                            _logger.LogWarning("⚠️ ClientInfo not found in pool for {ClientId}", cId);
                                        }
                                    }
                                }
                                else
                                {
                                    _logger.LogError("❌ Failed to parse GPGGA lon/direction: parts[4]={Lon}, parts[5]={Dir}", parts[4], parts[5]);
                                }
                            }
                            else
                            {
                                _logger.LogError("❌ Failed to parse GPGGA lat/direction: parts[2]={Lat}, parts[3]={Dir}", parts[2], parts[3]);
                            }
                        }
                        else
                        {
                            _logger.LogError("❌ GPGGA line has {PartCount} parts, expected 6+: {Line}", parts.Length, line);
                        }
                    }
                    else if (lineCount <= 5)
                    {
                        // Log first 5 non-GPGGA lines to see data format
                        _logger.LogInformation("📥 Line #{Count}: {Line}", lineCount, line.Substring(0, Math.Min(80, line.Length)));
                    }
                }
            }
            catch (OperationCanceledException) { }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error reading positions from {ClientId}", cId);
            }
        }

        async Task StreamRtcmDataAsync()
        {
            try
            {
                // Stream data immediately - position is optional
                _logger.LogInformation("Client {ClientId} stream task started", clientId);

                while (!cancellationToken.IsCancellationRequested)
                {
                    // Check position freshness only if we've received a position
                    if (hasReceivedPosition)
                    {
                        var timeSinceLastPos = DateTime.UtcNow - lastPositionTime;
                        if (timeSinceLastPos.TotalSeconds > MaxPositionAgeSec)
                        {
                            // Pause stream if position is too old
                            var clientInfo = _connectionPool.GetClient(clientId);
                            if (clientInfo != null && clientInfo.IsStreaming)
                            {
                                clientInfo.IsStreaming = false;
                                _logger.LogWarning("Client {ClientId} stream paused (stale position)", clientId);
                            }

                            await Task.Delay(1000, cancellationToken);
                            continue;
                        }
                    }

                    // Resume/stream data
                    var client = _connectionPool.GetClient(clientId);
                    if (client != null && !client.IsStreaming)
                    {
                        client.IsStreaming = true;
                        _logger.LogInformation("Client {ClientId} stream active", clientId);
                    }

                    // Read from ring buffer
                    int bytesRead = ringBuffer.ReadData(readPos, rtcmBuffer);
                    if (bytesRead < 0)
                    {
                        // Client too far behind
                        _logger.LogWarning("Client {ClientId} disconnected (too slow)", clientId);
                        break;
                    }

                    if (bytesRead > 0)
                    {
                        await stream.WriteAsync(rtcmBuffer, 0, bytesRead, cancellationToken);
                        readPos.Advance(bytesRead);

                        if (client != null)
                        {
                            client.BytesSent += bytesRead;
                        }

                        // Small delay to prevent overwhelming slow clients
                        // RTCM data is realtime, so small delays (5ms) don't hurt
                        await Task.Delay(5, cancellationToken);
                    }
                    else
                    {
                        await Task.Delay(20, cancellationToken);
                    }
                }
            }
            catch (OperationCanceledException) { }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error streaming to {ClientId}", clientId);
            }
        }
    }

    /// <summary>
    /// Convert NMEA DDMM.MMMM format to decimal degrees
    /// </summary>
    private double? ConvertNmeaToDecimal(double nmeaValue, bool isNegative)
    {
        if (nmeaValue < 0)
        {
            _logger.LogWarning("⚠️ ConvertNmeaToDecimal: negative value {Value}", nmeaValue);
            return null;
        }

        // Extract degrees (integer part / 100)
        int degrees = (int)(nmeaValue / 100);

        // Extract minutes (remainder / 100)
        double minutes = nmeaValue - (degrees * 100);

        // Convert to decimal degrees
        double decimalDegrees = degrees + (minutes / 60.0);

        // Apply sign if negative (South or West)
        if (isNegative)
            decimalDegrees = -decimalDegrees;

        _logger.LogInformation("🔄 ConvertNmeaToDecimal: input={Input}, degrees={Deg}, minutes={Min}, result={Result}, isNegative={IsNeg}",
            nmeaValue, degrees, minutes, decimalDegrees, isNegative);

        return decimalDegrees;
    }

    /// <summary>
    /// Handle sourcetable request: GET /
    /// Returns list of CONNECTED mount points in NTRIP 2.0 format
    /// Only includes sources that have active GNSS station connections
    /// </summary>
    private async Task HandleSourcetableRequestAsync(NetworkStream stream, CancellationToken cancellationToken)
    {
        try
        {
            // Create a scoped DbContext for this request
            using var scope = _serviceProvider.CreateScope();
            var dbContext = scope.ServiceProvider.GetRequiredService<ApplicationDbContext>();

            // Get ALL active mount points from database
            var allMountPoints = await dbContext.MountPoints
                .Where(m => m.IsActive)
                .ToListAsync();

            // Filter: Only include mount points that have CONNECTED sources
            var connectedMountPoints = new List<Models.Entities.MountPoint>();
            foreach (var mp in allMountPoints)
            {
                var source = _connectionPool.GetSourceForMountPoint(mp.Name);
                // Only include if source is connected (not disconnected)
                if (source != null && !source.IsDisconnected)
                {
                    connectedMountPoints.Add(mp);
                }
            }

            var sb = new StringBuilder();

            // NTRIP 2.0 sourcetable format
            sb.AppendLine("SOURCETABLE 2.0");

            // Get CAS and NET info from database
            var casterInfo = await dbContext.CasterInfos.FirstOrDefaultAsync(cancellationToken);
            var networkInfo = await dbContext.NetworkInfos.FirstOrDefaultAsync(cancellationToken);

            // CAS entry (Caster Info)
            // CAS;identifier;operator;nmea;country;lat;lon;fallback_host;port;misc
            if (casterInfo != null)
            {
                sb.AppendLine(
                    $"CAS;{casterInfo.Identifier};{casterInfo.Operator};{casterInfo.NmeaSupport};{casterInfo.Country};{casterInfo.Latitude:F1};{casterInfo.Longitude:F1};{casterInfo.FallbackHost ?? ""};{casterInfo.Port};{casterInfo.Description}");
            }
            else
            {
                // Fallback if no config found
                _logger.LogWarning("No CasterInfo configured, using defaults for sourcetable");
                sb.AppendLine("CAS;agopencast;AgOpenNtripCaster;0;NL;52.0;5.0;;2101;AgOpen GNSS RTK Server");
            }

            // NET entry (Network Info) - optional but recommended
            // NET;identifier;operator;auth;fee;website;email;startdate;enddate
            if (networkInfo != null)
            {
                sb.AppendLine(
                    $"NET;{networkInfo.Identifier};{networkInfo.Operator};{networkInfo.AuthenticationRequired};{networkInfo.FeeRequired};{networkInfo.Website};{networkInfo.Email};{networkInfo.StartDate:yyyy-MM-dd};{networkInfo.EndDate:yyyy-MM-dd}");
            }
            else
            {
                // Fallback if no config found
                _logger.LogWarning("No NetworkInfo configured, using defaults for sourcetable");
                sb.AppendLine("NET;NTRIP;AgOpenNtripCaster;Y;N;https://github.com/AgOpenGPS;info@agopenrtk.local;2025-01-01;2026-12-31");
            }

            // STR entries - ONLY for connected sources
            foreach (var mp in connectedMountPoints)
            {
                // Use RTCM-extracted coordinates if available, otherwise fallback to static coordinates
                var latitude = mp.RtcmLatitude ?? mp.Latitude;
                var longitude = mp.RtcmLongitude ?? mp.Longitude;
                var format = mp.DetectedFormat ?? mp.Format;
                var navSystems = mp.DetectedNavSystems ?? "GPS";
                var carrier = "1"; // Always 1 for RTK base stations
                var auth = mp.RequireClientAuthentication ? "Y" : "N";

                // STR;ID;Format;Carrier;NavSystem;Network;Country;Latitude;Longitude;NMEA;Solution;Generator;Compression;Auth;Fee;Bitrate;Misc
                sb.AppendLine(
                    $"STR;{mp.Name};{format};{carrier};{navSystems};NTRIP;NL;{latitude:F6};{longitude:F6};0;2;NtripCaster/2.0;none;{auth};N;{mp.BytesPerSecond ?? 2400};RTK");
            }

            sb.AppendLine("ENDSOURCETABLE");

            var sourcetableData = sb.ToString();

            // Send HTTP 200 response with sourcetable
            var responseBuilder = new StringBuilder();
            responseBuilder.AppendLine("HTTP/1.1 200 OK");
            responseBuilder.AppendLine("Content-Type: text/plain");
            responseBuilder.AppendLine($"Content-Length: {Encoding.ASCII.GetByteCount(sourcetableData)}");
            responseBuilder.AppendLine("Connection: close");
            responseBuilder.AppendLine();
            responseBuilder.Append(sourcetableData);

            var response = Encoding.ASCII.GetBytes(responseBuilder.ToString());
            await stream.WriteAsync(response, 0, response.Length, cancellationToken);

            _logger.LogInformation("Sourcetable sent with {Connected}/{Total} connected mount points",
                connectedMountPoints.Count, allMountPoints.Count);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error handling sourcetable request");
        }
    }

    /// <summary>
    /// Send HTTP response
    /// </summary>
    private static async Task SendResponseAsync(NetworkStream stream, string response, CancellationToken cancellationToken)
    {
        var data = Encoding.ASCII.GetBytes(response);
        await stream.WriteAsync(data, 0, data.Length, cancellationToken);
    }

    /// <summary>
    /// Extract username and password from Basic auth header
    /// Format: "Authorization: Basic base64(username:password)"
    /// </summary>
    private static (string?, string?) ExtractBasicAuth(string? authHeader)
    {
        if (string.IsNullOrEmpty(authHeader))
            return (null, null);

        try
        {
            const string prefix = "Basic ";
            var prefixIndex = authHeader.IndexOf(prefix, StringComparison.OrdinalIgnoreCase);
            if (prefixIndex < 0)
                return (null, null);

            var base64 = authHeader.Substring(prefixIndex + prefix.Length).Trim();
            var decoded = Encoding.ASCII.GetString(Convert.FromBase64String(base64));
            var parts = decoded.Split(':');

            if (parts.Length != 2)
                return (null, null);

            return (parts[0], parts[1]);
        }
        catch
        {
            return (null, null);
        }
    }

    /// <summary>
    /// Create a ClientSession in the database with sequential serial number
    /// Serial number is based on count of connected clients for the user
    /// </summary>
    private async Task<string?> CreateClientSessionAsync(
        string username,
        string clientId,
        string mountPointName,
        string? clientIpAddress,
        CancellationToken cancellationToken)
    {
        try
        {
            using var scope = _serviceProvider.CreateScope();
            var dbContext = scope.ServiceProvider.GetRequiredService<ApplicationDbContext>();
            var activityService = scope.ServiceProvider.GetRequiredService<IActivityService>();

            // Get mount point
            var mountPoint = await dbContext.MountPoints
                .FirstOrDefaultAsync(m => m.Name == mountPointName, cancellationToken);
            if (mountPoint == null)
            {
                _logger.LogWarning("Mount point not found: {MountPointName}", mountPointName);
                return null;
            }

            // Get user
            var user = await dbContext.Users
                .FirstOrDefaultAsync(u => u.UserName == username, cancellationToken);
            if (user == null)
            {
                _logger.LogWarning("User not found: {Username}", username);
                return null;
            }

            // Calculate serial number: count of active sessions for this user + 1
            var activeSessionCount = await dbContext.ClientSessions
                .Where(cs => cs.UserId == user.Id && cs.DisconnectedAt == null)
                .CountAsync(cancellationToken);
            var serialNumber = activeSessionCount + 1;

            // Create session
            var session = new ClientSession
            {
                Id = Guid.NewGuid().ToString(),
                UserId = user.Id,
                MountPointId = mountPoint.Id,
                ClientIpAddress = clientIpAddress,
                SerialNumber = serialNumber,
                ConnectedAt = DateTime.UtcNow,
                Status = ClientStreamStatus.Connected
            };

            dbContext.ClientSessions.Add(session);
            await dbContext.SaveChangesAsync(cancellationToken);

            _logger.LogInformation(
                "ClientSession created: {SessionId} for {Username} (serial #{SerialNumber})",
                session.Id, username, serialNumber);

            // Log activity
            await activityService.LogActivityAsync(
                ActivityType.ClientConnected,
                mountPoint.Id,
                user.Id,
                $"Rover '{username}' connected (#{serialNumber})");

            return session.Id;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error creating ClientSession for {Username}", username);
            return null;
        }
    }

    /// <summary>
    /// Update ClientSession position in database
    /// </summary>
    private async Task UpdateClientSessionPositionAsync(
        string sessionId,
        double latitude,
        double longitude,
        double accuracy,
        CancellationToken cancellationToken)
    {
        try
        {
            using var scope = _serviceProvider.CreateScope();
            var dbContext = scope.ServiceProvider.GetRequiredService<ApplicationDbContext>();

            var session = await dbContext.ClientSessions
                .FirstOrDefaultAsync(cs => cs.Id == sessionId, cancellationToken);
            if (session != null)
            {
                session.LastLatitude = latitude;
                session.LastLongitude = longitude;
                session.LastAccuracy = accuracy;
                session.LastPositionAt = DateTime.UtcNow;

                await dbContext.SaveChangesAsync(cancellationToken);
            }
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error updating ClientSession position: {SessionId}", sessionId);
        }
    }

    /// <summary>
    /// Mark ClientSession as disconnected
    /// </summary>
    private async Task MarkClientSessionDisconnectedAsync(
        string sessionId,
        CancellationToken cancellationToken,
        string? clientId = null)
    {
        try
        {
            using var scope = _serviceProvider.CreateScope();
            var dbContext = scope.ServiceProvider.GetRequiredService<ApplicationDbContext>();
            var activityService = scope.ServiceProvider.GetRequiredService<IActivityService>();

            var session = await dbContext.ClientSessions
                .Include(cs => cs.User)
                .FirstOrDefaultAsync(cs => cs.Id == sessionId, cancellationToken);
            if (session != null)
            {
                _logger.LogError("🔴 MARKING DISCONNECTED: sessionId={SessionId}, username={Username}, clientId={ClientId}",
                    sessionId, session.User?.UserName, clientId);

                session.DisconnectedAt = DateTime.UtcNow;
                session.Status = ClientStreamStatus.Disconnected;

                await dbContext.SaveChangesAsync(cancellationToken);

                _logger.LogError("🔴 SAVED TO DB: sessionId={SessionId}, DisconnectedAt={DisconnectedAt}",
                    sessionId, session.DisconnectedAt);

                // Log activity
                var userName = session.User?.UserName ?? "Unknown";
                await activityService.LogActivityAsync(
                    ActivityType.ClientDisconnected,
                    session.MountPointId,
                    session.UserId,
                    $"Rover '{userName}' disconnected (#{session.SerialNumber})");

                // Notify SignalR about client disconnection (for real-time dashboard updates)
                if (!string.IsNullOrEmpty(clientId))
                {
                    await _hubContext.Clients.All.SendAsync(
                        "ClientDisconnected",
                        new { clientId = clientId, username = userName });
                }
            }
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error marking ClientSession disconnected: {SessionId}", sessionId);
        }
    }

    /// <summary>
    /// Create SourceConnection in database
    /// </summary>
    private async Task<int?> CreateSourceConnectionAsync(
        string mountPointName,
        CancellationToken cancellationToken)
    {
        try
        {
            using var scope = _serviceProvider.CreateScope();
            var dbContext = scope.ServiceProvider.GetRequiredService<ApplicationDbContext>();
            var activityService = scope.ServiceProvider.GetRequiredService<IActivityService>();

            // Get mount point with Owner navigation property
            var mountPoint = await dbContext.MountPoints
                .Include(m => m.Owner)
                .FirstOrDefaultAsync(m => m.Name == mountPointName, cancellationToken);
            if (mountPoint == null)
            {
                _logger.LogWarning("Mount point not found: {MountPointName}", mountPointName);
                return null;
            }

            // Create connection
            var connection = new SourceConnection
            {
                MountPointId = mountPoint.Id,
                ConnectedAt = DateTime.UtcNow,
                Status = SourceConnectionStatus.Streaming
            };

            dbContext.SourceConnections.Add(connection);
            await dbContext.SaveChangesAsync(cancellationToken);

            _logger.LogInformation(
                "SourceConnection created for mount point {MountPointName}",
                mountPointName);

            // Log activity
            await activityService.LogActivityAsync(
                ActivityType.SourceConnected,
                mountPoint.Id,
                null,
                $"Base station '{mountPointName}' connected");

            // Send email notifications when source comes online
            try
            {
                var emailSettings = await _emailTriggerSettingsService.GetSettingsAsync();

                if (emailSettings.SendSourceOnlineEmail)
                {
                    // Get owner email (if user-owned) and admin email
                    var ownerEmail = mountPoint.Owner?.Email;
                    var adminEmail = emailSettings.AdminEmailForSourceNotifications;

                    // Send to owner if it's a user-owned source
                    if (!string.IsNullOrEmpty(ownerEmail))
                    {
                        await _emailService.SendSourceOnlineEmailAsync(
                            ownerEmail,
                            mountPoint.Owner!.FullName ?? mountPoint.Owner.UserName ?? "User",
                            mountPoint.Name,
                            mountPointName);
                        _logger.LogInformation("Source online email sent to owner: {Email}", ownerEmail);
                    }

                    // Always send to admin
                    if (!string.IsNullOrEmpty(adminEmail) && adminEmail != ownerEmail)
                    {
                        await _emailService.SendSourceOnlineEmailAsync(
                            adminEmail,
                            "Administrator",
                            mountPoint.Name,
                            mountPointName);
                        _logger.LogInformation("Source online email sent to admin: {Email}", adminEmail);
                    }
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error sending source online email for {MountPointName}", mountPointName);
                // Don't throw - let the connection succeed even if email fails
            }

            return connection.Id;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error creating SourceConnection for {MountPointName}", mountPointName);
            return null;
        }
    }

    /// <summary>
    /// Mark SourceConnection as disconnected
    /// </summary>
    private async Task MarkSourceConnectionDisconnectedAsync(
        string mountPointName,
        CancellationToken cancellationToken)
    {
        try
        {
            using var scope = _serviceProvider.CreateScope();
            var dbContext = scope.ServiceProvider.GetRequiredService<ApplicationDbContext>();
            var activityService = scope.ServiceProvider.GetRequiredService<IActivityService>();

            // Find the latest active connection for this mount point
            var connection = await dbContext.SourceConnections
                .Include(sc => sc.MountPoint)
                .ThenInclude(m => m!.Owner)
                .Where(sc => sc.MountPoint!.Name == mountPointName && sc.DisconnectedAt == null)
                .OrderByDescending(sc => sc.ConnectedAt)
                .FirstOrDefaultAsync(cancellationToken);

            if (connection != null)
            {
                connection.DisconnectedAt = DateTime.UtcNow;
                connection.Status = SourceConnectionStatus.Disconnected;

                await dbContext.SaveChangesAsync(cancellationToken);

                _logger.LogInformation(
                    "SourceConnection marked disconnected for mount point {MountPointName}",
                    mountPointName);

                // Log activity
                await activityService.LogActivityAsync(
                    ActivityType.SourceDisconnected,
                    connection.MountPointId,
                    null,
                    $"Base station '{mountPointName}' disconnected");

                // Send email notifications when source goes offline
                try
                {
                    var emailSettings = await _emailTriggerSettingsService.GetSettingsAsync();

                    if (emailSettings.SendSourceOfflineEmail && connection.MountPoint != null)
                    {
                        // Get owner email (if user-owned) and admin email
                        var ownerEmail = connection.MountPoint.Owner?.Email;
                        var adminEmail = emailSettings.AdminEmailForSourceNotifications;

                        // Send to owner if it's a user-owned source
                        if (!string.IsNullOrEmpty(ownerEmail))
                        {
                            await _emailService.SendSourceOfflineEmailAsync(
                                ownerEmail,
                                connection.MountPoint.Owner!.FullName ?? connection.MountPoint.Owner.UserName ?? "User",
                                connection.MountPoint.Name,
                                mountPointName);
                            _logger.LogInformation("Source offline email sent to owner: {Email}", ownerEmail);
                        }

                        // Always send to admin
                        if (!string.IsNullOrEmpty(adminEmail) && adminEmail != ownerEmail)
                        {
                            await _emailService.SendSourceOfflineEmailAsync(
                                adminEmail,
                                "Administrator",
                                connection.MountPoint.Name,
                                mountPointName);
                            _logger.LogInformation("Source offline email sent to admin: {Email}", adminEmail);
                        }
                    }
                }
                catch (Exception ex)
                {
                    _logger.LogError(ex, "Error sending source offline email for {MountPointName}", mountPointName);
                    // Don't throw - let the disconnection proceed even if email fails
                }
            }
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error marking SourceConnection disconnected for {MountPointName}", mountPointName);
        }
    }

    /// <summary>
    /// Update MountPoint with position data extracted from RTCM1005 messages
    /// </summary>
    private async Task UpdateMountPointPositionAsync(
        string mountPointName,
        decimal latitude,
        decimal longitude,
        int? referenceStationId,
        CancellationToken cancellationToken)
    {
        try
        {
            using var scope = _serviceProvider.CreateScope();
            var dbContext = scope.ServiceProvider.GetRequiredService<ApplicationDbContext>();

            var mountPoint = await dbContext.MountPoints
                .FirstOrDefaultAsync(m => m.Name == mountPointName, cancellationToken);

            if (mountPoint != null)
            {
                // Update with RTCM-extracted coordinates
                mountPoint.RtcmLatitude = latitude;
                mountPoint.RtcmLongitude = longitude;
                if (referenceStationId.HasValue)
                {
                    mountPoint.ReferenceStationId = referenceStationId.Value;
                }
                mountPoint.LastRtcmMessageTime = DateTime.UtcNow;
                mountPoint.MessageCount++;

                await dbContext.SaveChangesAsync(cancellationToken);

                _logger.LogDebug(
                    "Updated MountPoint {MountPointName} with RTCM1005 position: {Lat},{Lon}",
                    mountPointName, latitude, longitude);
            }
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error updating MountPoint position for {MountPointName}", mountPointName);
        }
    }
}
