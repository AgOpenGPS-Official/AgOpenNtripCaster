using Microsoft.AspNetCore.SignalR;
using AgOpenNtripCaster.Server.Hubs;

namespace AgOpenNtripCaster.Server.Services.NTRIP;

/// <summary>
/// Real-time connection statistics DTO
/// </summary>
public class ConnectionStats
{
    public int TotalConnections { get; set; }
    public int ActiveClientCount { get; set; }
    public int ActiveSourceCount { get; set; }
    public decimal ThroughputMbps { get; set; }
    public decimal UploadMbps { get; set; }
    public decimal DownloadMbps { get; set; }
    public int AverageLatencyMs { get; set; }
    public double CpuUsagePercent { get; set; }
    public double MemoryUsagePercent { get; set; }
    public DateTime CollectedAt { get; set; } = DateTime.UtcNow;
}

/// <summary>
/// Service for collecting and broadcasting connection statistics
/// </summary>
public interface IConnectionStatsService
{
    Task StartCollectionAsync();
    Task StopCollectionAsync();
}

public class ConnectionStatsService : IConnectionStatsService
{
    private readonly IHubContext<NtripHub> _hubContext;
    private readonly ILogger<ConnectionStatsService> _logger;
    private readonly ConnectionPool _connectionPool;
    private Timer? _statsCollectionTimer;
    private const int StatsCollectionIntervalMs = 10000; // Every 10 seconds

    public ConnectionStatsService(
        IHubContext<NtripHub> hubContext,
        ILogger<ConnectionStatsService> logger,
        ConnectionPool connectionPool)
    {
        _hubContext = hubContext;
        _logger = logger;
        _connectionPool = connectionPool;
    }

    public Task StartCollectionAsync()
    {
        _statsCollectionTimer = new Timer(
            async _ => await CollectAndBroadcastStatsAsync(),
            null,
            StatsCollectionIntervalMs,
            StatsCollectionIntervalMs);

        _logger.LogInformation("Connection statistics collection started");
        return Task.CompletedTask;
    }

    public Task StopCollectionAsync()
    {
        if (_statsCollectionTimer != null)
        {
            _statsCollectionTimer.Dispose();
            _logger.LogInformation("Connection statistics collection stopped");
        }

        return Task.CompletedTask;
    }

    private async Task CollectAndBroadcastStatsAsync()
    {
        try
        {
            var stats = CollectCurrentStats();
            await _hubContext.Clients.All.SendAsync("ConnectionStatsUpdated", stats);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error collecting or broadcasting connection stats");
        }
    }

    private ConnectionStats CollectCurrentStats()
    {
        // Get actual connection counts from ConnectionPool
        var activeClientCount = _connectionPool.GetActiveClientCount();
        var activeSourceCount = _connectionPool.GetActiveSourceCount();
        var totalConnections = activeClientCount + activeSourceCount;

        // Get system info for CPU/Memory
        var cpuUsage = GetCpuUsage();
        var memoryUsage = GetMemoryUsage();

        var stats = new ConnectionStats
        {
            TotalConnections = totalConnections,
            ActiveClientCount = activeClientCount,
            ActiveSourceCount = activeSourceCount,
            ThroughputMbps = CalculateThroughput(),
            UploadMbps = 0,
            DownloadMbps = 0,
            AverageLatencyMs = 0,
            CpuUsagePercent = cpuUsage,
            MemoryUsagePercent = memoryUsage,
            CollectedAt = DateTime.UtcNow
        };

        return stats;
    }

    private double GetCpuUsage()
    {
        try
        {
            // On Windows/Linux, we can get CPU info from /proc/stat, but for simplicity
            // we'll use process-level CPU and estimate system CPU
            var process = System.Diagnostics.Process.GetCurrentProcess();
            var totalProcessorTime = process.TotalProcessorTime.TotalMilliseconds;
            var userProcessorTime = process.UserProcessorTime.TotalMilliseconds;

            // This gives us process CPU usage as percentage of total available
            // In a real scenario, you'd track this over time
            return 0; // Placeholder - would need historical data to calculate properly
        }
        catch
        {
            return 0;
        }
    }

    private double GetMemoryUsage()
    {
        try
        {
            var process = System.Diagnostics.Process.GetCurrentProcess();
            var workingSetMb = process.WorkingSet64 / (1024.0 * 1024.0);

            // Estimate system memory - this is crude but works
            // In production, you'd query actual system memory info
            var estimatedSystemMemoryMb = 8000;

            var usage = (workingSetMb / estimatedSystemMemoryMb) * 100;
            return Math.Round(Math.Min(usage, 100), 2);
        }
        catch
        {
            return 0;
        }
    }

    private decimal CalculateThroughput()
    {
        // This is a placeholder - in production you would track actual bytes transferred
        // For now, returning 0 - NtripServerService should track this
        return 0m;
    }
}
