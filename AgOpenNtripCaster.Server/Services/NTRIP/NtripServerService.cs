using System.Net;
using System.Net.Sockets;
using System.Text;
using Microsoft.EntityFrameworkCore;
using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Services.Auth;

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
    private readonly Dictionary<string, RingBuffer> _mountPointBuffers;

    private TcpListener? _tcpListener;
    private CancellationTokenSource? _cancellationTokenSource;
    private Task? _acceptTask;

    private const int Port = 2101;
    private const int ListenBacklog = 128;

    public NtripServerService(
        ILogger<NtripServerService> logger,
        IServiceProvider serviceProvider,
        ConnectionPool connectionPool)
    {
        _logger = logger;
        _serviceProvider = serviceProvider;
        _connectionPool = connectionPool;
        _mountPointBuffers = new Dictionary<string, RingBuffer>();
    }

    public async Task StartAsync(CancellationToken cancellationToken)
    {
        try
        {
            _logger.LogInformation("Starting NTRIP Server on port {Port}...", Port);

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
    /// SOURCE STATION_A:sourcePassword123
    /// </summary>
    private async Task HandleSourceConnectionAsync(
        string sourceId,
        TcpClient tcpClient,
        StreamReader reader,
        string requestLine,
        CancellationToken cancellationToken)
    {
        try
        {
            // Parse: "SOURCE STATION_A:password"
            var parts = requestLine.Split(' ');
            if (parts.Length < 2)
            {
                await SendResponseAsync(tcpClient.GetStream(), "400 Bad Request\r\n\r\n", cancellationToken);
                return;
            }

            var credentials = parts[1].Split(':');
            if (credentials.Length != 2)
            {
                await SendResponseAsync(tcpClient.GetStream(), "400 Bad Request\r\n\r\n", cancellationToken);
                return;
            }

            var mountPointName = credentials[0];
            var password = credentials[1];

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

            // Get or create ring buffer for this mount point
            if (!_mountPointBuffers.ContainsKey(mountPointName))
            {
                _mountPointBuffers[mountPointName] = new RingBuffer();
            }

            var ringBuffer = _mountPointBuffers[mountPointName];

            // Stream RTCM data from source
            await HandleSourceStreamAsync(sourceId, ringBuffer, reader, tcpClient.GetStream(), cancellationToken);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error handling source connection {SourceId}", sourceId);
        }
        finally
        {
            _connectionPool.UnregisterSource(sourceId);
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
            _connectionPool.UnregisterClient(clientId);
        }
    }

    /// <summary>
    /// Handle source streaming - read RTCM from source, write to ring buffer
    /// </summary>
    private async Task HandleSourceStreamAsync(
        string sourceId,
        RingBuffer ringBuffer,
        StreamReader reader,
        NetworkStream stream,
        CancellationToken cancellationToken)
    {
        var buffer = new byte[4096];
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
        var rtcmBuffer = new byte[4096];
        var readPos = ringBuffer.GetCurrentPosition();
        var lastPositionTime = DateTime.MinValue;
        const int MaxPositionAgeSec = 15;

        _logger.LogInformation("Client {ClientId} starting stream", clientId);

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
                while ((line = await sr.ReadLineAsync(ct)) != null)
                {
                    if (line.StartsWith("POS|"))
                    {
                        var parts = line.Split('|');
                        if (parts.Length >= 4)
                        {
                            if (double.TryParse(parts[1], out var lat) &&
                                double.TryParse(parts[2], out var lon) &&
                                double.TryParse(parts[3], out var acc))
                            {
                                lastPositionTime = DateTime.UtcNow;
                                var clientInfo = _connectionPool.GetClient(cId);
                                if (clientInfo != null)
                                {
                                    clientInfo.LastLatitude = lat;
                                    clientInfo.LastLongitude = lon;
                                    clientInfo.LastAccuracy = acc;
                                    clientInfo.LastPositionAt = lastPositionTime;
                                }

                                _logger.LogDebug("Position update from {ClientId}: {Lat},{Lon}", cId, lat, lon);
                            }
                        }
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
                while (!cancellationToken.IsCancellationRequested)
                {
                    // Check position freshness
                    var timeSinceLastPos = DateTime.UtcNow - lastPositionTime;
                    if (timeSinceLastPos.TotalSeconds > MaxPositionAgeSec)
                    {
                        // Pause stream
                        var clientInfo = _connectionPool.GetClient(clientId);
                        if (clientInfo != null && clientInfo.IsStreaming)
                        {
                            clientInfo.IsStreaming = false;
                            _logger.LogWarning("Client {ClientId} stream paused (no position)", clientId);
                        }

                        await Task.Delay(1000, cancellationToken);
                        continue;
                    }

                    // Resume/stream data
                    var client = _connectionPool.GetClient(clientId);
                    if (client != null && !client.IsStreaming)
                    {
                        client.IsStreaming = true;
                        _logger.LogInformation("Client {ClientId} stream resumed", clientId);
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
                    }
                    else
                    {
                        await Task.Delay(10, cancellationToken);
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
}
