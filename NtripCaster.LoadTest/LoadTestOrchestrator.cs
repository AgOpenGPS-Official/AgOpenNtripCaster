namespace NtripCaster.LoadTest;

/// <summary>
/// Orchestrates a load test with multiple source and client connections
/// </summary>
public class LoadTestOrchestrator
{
    private readonly string _serverHost;
    private readonly int _serverPort;
    private readonly string _apiBaseUrl;
    private readonly ApiClient _apiClient;
    private readonly List<SourceClient> _sources = new();
    private readonly List<RoverClient> _clients = new();
    private readonly List<int> _createdMountPointIds = new();
    private readonly CancellationTokenSource _cancellationTokenSource = new();

    public LoadTestOrchestrator(string serverHost, int serverPort, string apiBaseUrl)
    {
        _serverHost = serverHost;
        _serverPort = serverPort;
        _apiBaseUrl = apiBaseUrl;
        _apiClient = new ApiClient(apiBaseUrl);
    }

    public async Task RunAsync(int numSources, int numClients, int durationSeconds, string? adminEmail = null, string? adminPassword = null, int connectDelayMs = 100)
    {
        Console.WriteLine($"\n╔════════════════════════════════════════════════════════════╗");
        Console.WriteLine($"║ NTRIP Load Test: {numSources} Sources × {numClients} Clients         ║");
        Console.WriteLine($"║ Server: {_serverHost}:{_serverPort,-40} ║");
        Console.WriteLine($"║ API: {_apiBaseUrl,-51} ║");
        Console.WriteLine($"║ Duration: {durationSeconds}s                                         ║");
        Console.WriteLine($"╚════════════════════════════════════════════════════════════╝\n");

        try
        {
            // Setup: Login and create mount points if credentials provided
            if (!string.IsNullOrEmpty(adminEmail) && !string.IsNullOrEmpty(adminPassword))
            {
                await SetupMountPointsAsync(numSources, adminEmail, adminPassword);
            }

            // Create and connect sources
            Console.WriteLine($"Connecting {numSources} sources...");
            var sourceStartTime = DateTime.UtcNow;
            for (int i = 0; i < numSources; i++)
            {
                var sourceId = $"SOURCE_{i:D3}";
                var source = new SourceClient(_serverHost, _serverPort, sourceId, "dLhSSmNjM4", rtcmIntervalMs: 100);

                try
                {
                    await source.ConnectAsync(_cancellationTokenSource.Token);
                    _sources.Add(source);
                    Console.WriteLine($"  ✓ {sourceId}");
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"  ✗ {sourceId}: {ex.Message}");
                }

                if (i < numSources - 1)
                    await Task.Delay(connectDelayMs, _cancellationTokenSource.Token);
            }
            var sourceConnectDuration = DateTime.UtcNow - sourceStartTime;
            Console.WriteLine($"Completed in {sourceConnectDuration.TotalSeconds:F2}s ({_sources.Count}/{numSources} connected)\n");

            // Create and connect clients
            Console.WriteLine($"Connecting {numClients} clients...");
            var clientStartTime = DateTime.UtcNow;
            for (int i = 0; i < numClients; i++)
            {
                var sourceIndex = i % numSources; // Distribute clients across sources
                var sourceId = $"SOURCE_{sourceIndex:D3}";
                var clientId = $"CLIENT_{i:D4}";

                var client = new RoverClient(
                    _serverHost,
                    _serverPort,
                    sourceId,
                    "client_user",
                    "client_password_123",
                    positionIntervalMs: 10000 // Send position every 10 seconds
                );

                try
                {
                    await client.ConnectAsync(_cancellationTokenSource.Token);
                    _clients.Add(client);
                    if ((i + 1) % 50 == 0)
                        Console.WriteLine($"  {i + 1}/{numClients} clients connected");
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"  ✗ {clientId}: {ex.Message}");
                }

                if (i < numClients - 1)
                    await Task.Delay(connectDelayMs, _cancellationTokenSource.Token);
            }
            var clientConnectDuration = DateTime.UtcNow - clientStartTime;
            Console.WriteLine($"Completed in {clientConnectDuration.TotalSeconds:F2}s ({_clients.Count}/{numClients} connected)\n");

            // Monitor metrics
            await MonitorMetricsAsync(durationSeconds);
        }
        catch (OperationCanceledException)
        {
            Console.WriteLine("\nTest cancelled");
        }
        finally
        {
            // Cleanup
            Console.WriteLine("\nDisconnecting all clients...");
            await DisconnectAllAsync();

            // Delete test mount points
            await CleanupMountPointsAsync();
        }
    }

    private async Task MonitorMetricsAsync(int durationSeconds)
    {
        var testEndTime = DateTime.UtcNow.AddSeconds(durationSeconds);
        var lastReportTime = DateTime.UtcNow;

        Console.WriteLine($"Test running... (Ctrl+C to stop)\n");
        Console.WriteLine("TIME    | SOURCES | CLIENTS | SRC FRAMES | SRC BYTES(MB) | CLT FRAMES | CLT BYTES(MB) | AVG LATENCY(ms) |");
        Console.WriteLine("--------|---------|---------|------------|---------------|------------|---------------|-----------------|");

        while (DateTime.UtcNow < testEndTime && !_cancellationTokenSource.Token.IsCancellationRequested)
        {
            try
            {
                await Task.Delay(5000, _cancellationTokenSource.Token);

                var elapsedSeconds = (int)(DateTime.UtcNow - lastReportTime).TotalSeconds;
                var activeSources = _sources.Count(s => s.IsConnected);
                var activeClients = _clients.Count(c => c.IsConnected);
                var totalSourceFrames = _sources.Sum(s => s.FramesSent);
                var totalSourceBytes = _sources.Sum(s => s.BytesSent);
                var totalClientFrames = _clients.Sum(c => c.FramesReceived);
                var totalClientBytes = _clients.Sum(c => c.BytesReceived);

                var avgLatency = CalculateAverageLatency();

                Console.WriteLine($"{elapsedSeconds:D3}s    | {activeSources:D7} | {activeClients:D7} | {totalSourceFrames:D10} | {(totalSourceBytes / 1024.0 / 1024.0):F2,11} | {totalClientFrames:D10} | {(totalClientBytes / 1024.0 / 1024.0):F2,11} | {avgLatency:F2,13}  |");

                // Stop if all clients disconnected unexpectedly
                if (activeClients == 0 && _clients.Count > 0)
                {
                    Console.WriteLine("All clients disconnected. Test stopped.");
                    break;
                }
            }
            catch (OperationCanceledException)
            {
                break;
            }
        }
    }

    private async Task SetupMountPointsAsync(int numSources, string adminEmail, string adminPassword)
    {
        Console.WriteLine("\n📋 SETUP: Creating test mount points...");

        // Login
        Console.WriteLine($"Authenticating as {adminEmail}...");
        bool loggedIn = await _apiClient.LoginAsync(adminEmail, adminPassword);
        if (!loggedIn)
        {
            Console.WriteLine("❌ Failed to login. Continuing without mount point creation.");
            return;
        }
        Console.WriteLine("✓ Authentication successful");

        // Create mount points
        Console.WriteLine($"Creating {numSources} mount points...");
        for (int i = 0; i < numSources; i++)
        {
            var mountPointName = $"SOURCE_{i:D3}";
            var sourcePassword = "dLhSSmNjM4";  // Shared password for all sources
            var description = $"Load test source #{i}";

            try
            {
                var mountPointId = await _apiClient.CreateMountPointAsync(mountPointName, sourcePassword, description);
                if (mountPointId.HasValue)
                {
                    _createdMountPointIds.Add(mountPointId.Value);
                    if ((i + 1) % 5 == 0)
                        Console.WriteLine($"  ✓ {i + 1}/{numSources} mount points created");
                }
                else
                {
                    Console.WriteLine($"  ⚠️  Failed to create {mountPointName}");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"  ⚠️  Error creating {mountPointName}: {ex.Message}");
            }
        }

        Console.WriteLine($"✓ Setup complete: {_createdMountPointIds.Count}/{numSources} mount points created\n");
    }

    private async Task CleanupMountPointsAsync()
    {
        if (_createdMountPointIds.Count == 0)
            return;

        Console.WriteLine($"\n🧹 CLEANUP: Deleting {_createdMountPointIds.Count} test mount points...");

        int deleted = 0;
        foreach (var mountPointId in _createdMountPointIds)
        {
            var success = await _apiClient.DeleteMountPointAsync(mountPointId);
            if (success) deleted++;
        }

        Console.WriteLine($"✓ Cleanup complete: {deleted}/{_createdMountPointIds.Count} mount points deleted\n");
    }

    private double CalculateAverageLatency()
    {
        // Estimate latency based on frame synchronization
        // This is simplified - real latency measurement would need packet timestamps
        if (_sources.Count == 0 || _clients.Count == 0)
            return 0;

        var avgSourceFrames = _sources.Average(s => s.FramesSent);
        var avgClientFrames = _clients.Average(c => c.FramesReceived);

        if (avgSourceFrames == 0)
            return 0;

        // Estimate based on frame lag (simplified)
        return (avgSourceFrames - avgClientFrames) / avgSourceFrames * 100; // Very rough estimate
    }

    private async Task DisconnectAllAsync()
    {
        var tasks = new List<Task>();

        foreach (var source in _sources)
        {
            if (source.IsConnected)
                tasks.Add(source.DisconnectAsync());
        }

        foreach (var client in _clients)
        {
            if (client.IsConnected)
                tasks.Add(client.DisconnectAsync());
        }

        await Task.WhenAll(tasks);
    }

    public void PrintFinalReport()
    {
        Console.WriteLine("\n╔════════════════════════════════════════════════════════════╗");
        Console.WriteLine("║ LOAD TEST FINAL REPORT                                     ║");
        Console.WriteLine("╚════════════════════════════════════════════════════════════╝\n");

        // Source Statistics
        Console.WriteLine("📊 SOURCE STATISTICS");
        Console.WriteLine("─────────────────────────────────────────────────────────────");
        Console.WriteLine($"Total Sources Connected: {_sources.Count}");
        Console.WriteLine($"Currently Connected:    {_sources.Count(s => s.IsConnected)}");

        if (_sources.Count > 0)
        {
            var totalFramesSent = _sources.Sum(s => s.FramesSent);
            var totalBytesSent = _sources.Sum(s => s.BytesSent);
            var avgFrameSize = _sources.Average(s => s.FramesSent > 0 ? (double)s.BytesSent / s.FramesSent : 0);
            var avgDuration = _sources.Average(s => s.GetConnectionDuration().TotalSeconds);
            var avgThroughput = _sources.Sum(s => s.BytesSent) / Math.Max(1, _sources.Sum(s => s.GetConnectionDuration().TotalSeconds));

            Console.WriteLine($"Total Frames Sent:      {totalFramesSent:N0}");
            Console.WriteLine($"Total Data Sent:        {(totalBytesSent / 1024.0 / 1024.0):F2} MB");
            Console.WriteLine($"Average Frame Size:     {avgFrameSize:F0} bytes");
            Console.WriteLine($"Average Duration:       {avgDuration:F2} seconds");
            Console.WriteLine($"Average Throughput:     {(avgThroughput / 1024.0):F2} KB/s");
            Console.WriteLine($"Avg Frames/sec:         {totalFramesSent / Math.Max(1, avgDuration):F1} fps");
        }

        // Client Statistics
        Console.WriteLine("\n📊 CLIENT STATISTICS");
        Console.WriteLine("─────────────────────────────────────────────────────────────");
        Console.WriteLine($"Total Clients Connected: {_clients.Count}");
        Console.WriteLine($"Currently Connected:    {_clients.Count(c => c.IsConnected)}");

        if (_clients.Count > 0)
        {
            var totalFramesReceived = _clients.Sum(c => c.FramesReceived);
            var totalBytesReceived = _clients.Sum(c => c.BytesReceived);
            var avgFrameSize = _clients.Average(c => c.FramesReceived > 0 ? (double)c.BytesReceived / c.FramesReceived : 0);
            var avgDuration = _clients.Average(c => c.GetConnectionDuration().TotalSeconds);
            var avgThroughput = _clients.Sum(c => c.BytesReceived) / Math.Max(1, _clients.Sum(c => c.GetConnectionDuration().TotalSeconds));
            var totalPositionsSent = _clients.Sum(c => c.PositionsSent);

            Console.WriteLine($"Total Frames Received:  {totalFramesReceived:N0}");
            Console.WriteLine($"Total Data Received:    {(totalBytesReceived / 1024.0 / 1024.0):F2} MB");
            Console.WriteLine($"Average Frame Size:     {avgFrameSize:F0} bytes");
            Console.WriteLine($"Average Duration:       {avgDuration:F2} seconds");
            Console.WriteLine($"Average Throughput:     {(avgThroughput / 1024.0):F2} KB/s");
            Console.WriteLine($"Avg Frames/sec:         {totalFramesReceived / Math.Max(1, avgDuration):F1} fps");
            Console.WriteLine($"Total Positions Sent:   {totalPositionsSent:N0}");
        }

        // Performance Analysis
        Console.WriteLine("\n📈 PERFORMANCE ANALYSIS");
        Console.WriteLine("─────────────────────────────────────────────────────────────");

        var sourceSuccessRate = _sources.Count > 0 ? (_sources.Count(s => s.IsConnected) / (double)_sources.Count * 100) : 0;
        var clientSuccessRate = _clients.Count > 0 ? (_clients.Count(c => c.IsConnected) / (double)_clients.Count * 100) : 0;

        Console.WriteLine($"Source Connection Success Rate:  {sourceSuccessRate:F1}%");
        Console.WriteLine($"Client Connection Success Rate:  {clientSuccessRate:F1}%");

        if (_sources.Count > 0 && _clients.Count > 0)
        {
            var sourceThroughput = _sources.Sum(s => s.BytesSent) / Math.Max(1, _sources.Sum(s => s.GetConnectionDuration().TotalSeconds));
            var clientThroughput = _clients.Sum(c => c.BytesReceived) / Math.Max(1, _clients.Sum(c => c.GetConnectionDuration().TotalSeconds));
            var totalFrameFramesMissed = _sources.Sum(s => s.FramesSent) - _clients.Sum(c => c.FramesReceived);
            var packetLossRate = (_sources.Sum(s => s.FramesSent) > 0)
                ? (totalFrameFramesMissed / (double)_sources.Sum(s => s.FramesSent) * 100)
                : 0;

            Console.WriteLine($"Source Total Throughput:        {(sourceThroughput / 1024.0):F2} KB/s");
            Console.WriteLine($"Client Total Throughput:        {(clientThroughput / 1024.0):F2} KB/s");
            Console.WriteLine($"Estimated Frame Loss Rate:      {packetLossRate:F2}%");
        }

        Console.WriteLine("\n✓ Test Report Complete\n");
    }
}
