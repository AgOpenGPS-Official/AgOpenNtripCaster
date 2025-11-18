using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.Entities;
using AgOpenNtripCaster.Server.Services.NTRIP;
using Microsoft.EntityFrameworkCore;

namespace AgOpenNtripCaster.Server.Services.Performance;

/// <summary>
/// Real-time performance metrics tracking for zero-copy broadcasting
/// Samples metrics periodically and provides current stats
/// </summary>
public class PerformanceMetricsService : BackgroundService
{
    private readonly IServiceProvider _serviceProvider;
    private readonly ILogger<PerformanceMetricsService> _logger;
    private readonly ConnectionPool _connectionPool;

    // Real-time metrics (in-memory)
    private long _totalBytesSent;
    private long _totalBroadcasts;
    private double _averageBroadcastTimeMs;
    private double _peakBroadcastTimeMs;
    private int _peakZeroCopyBuffers;
    private readonly object _metricsLock = new();

    private const int SampleIntervalSeconds = 60; // Sample every 60 seconds
    private const int PersistIntervalMinutes = 5; // Persist to DB every 5 minutes

    public PerformanceMetricsService(
        IServiceProvider serviceProvider,
        ILogger<PerformanceMetricsService> logger,
        ConnectionPool connectionPool)
    {
        _serviceProvider = serviceProvider;
        _logger = logger;
        _connectionPool = connectionPool;
    }

    /// <summary>
    /// Get current performance metrics snapshot
    /// </summary>
    public PerformanceSnapshot GetCurrentMetrics()
    {
        lock (_metricsLock)
        {
            var activeClients = _connectionPool.GetActiveClientCount();
            var activeSources = _connectionPool.GetActiveSourceCount();

            // Calculate active zero-copy buffers (pending buffers across all clients)
            var allClients = _connectionPool.GetAllActiveClients();
            var activeBuffers = allClients.Sum(c => c.PendingBufferCount);

            // Calculate total bytes sent across all clients
            var totalBytesSent = allClients.Sum(c => c.BytesSent);

            // Calculate memory saved vs old approach
            // Old approach: each client had its own 8KB buffer
            // New approach: clients share buffers via ReadOnlyMemory
            // Estimated savings = (total_broadcasts * active_clients * 8KB) - (active_buffers * 8KB)
            long estimatedMemorySavedBytes = 0;
            if (activeClients > 0 && _totalBroadcasts > 0)
            {
                const long avgBufferSize = 8192; // 8KB average RTCM buffer
                long oldApproachMemory = _totalBroadcasts * activeClients * avgBufferSize;
                long newApproachMemory = _totalBroadcasts * avgBufferSize; // Single buffer per broadcast
                estimatedMemorySavedBytes = oldApproachMemory - newApproachMemory;
            }

            // Get current memory usage
            var totalMemoryBytes = GC.GetTotalMemory(forceFullCollection: false);
            var gcCollectionCount = GC.CollectionCount(0) + GC.CollectionCount(1) + GC.CollectionCount(2);

            return new PerformanceSnapshot
            {
                TotalBytesSent = totalBytesSent,
                MemorySavedBytes = estimatedMemorySavedBytes,
                ActiveZeroCopyBuffers = activeBuffers,
                PeakZeroCopyBuffers = _peakZeroCopyBuffers,
                AverageBroadcastTimeMs = _averageBroadcastTimeMs,
                PeakBroadcastTimeMs = _peakBroadcastTimeMs,
                TotalBroadcasts = (int)_totalBroadcasts,
                ActiveClients = activeClients,
                ActiveSources = activeSources,
                TotalMemoryUsageBytes = totalMemoryBytes,
                GcCollectionCount = gcCollectionCount,
                Timestamp = DateTime.UtcNow
            };
        }
    }

    /// <summary>
    /// Record a broadcast event (called by NtripServerService after each broadcast)
    /// </summary>
    public void RecordBroadcast(int clientCount, double durationMs)
    {
        lock (_metricsLock)
        {
            _totalBroadcasts++;

            // Update average broadcast time (running average)
            _averageBroadcastTimeMs = ((_averageBroadcastTimeMs * (_totalBroadcasts - 1)) + durationMs) / _totalBroadcasts;

            // Update peak broadcast time
            if (durationMs > _peakBroadcastTimeMs)
            {
                _peakBroadcastTimeMs = durationMs;
            }

            // Update peak buffer count
            var allClients = _connectionPool.GetAllActiveClients();
            var currentBuffers = allClients.Sum(c => c.PendingBufferCount);
            if (currentBuffers > _peakZeroCopyBuffers)
            {
                _peakZeroCopyBuffers = currentBuffers;
            }
        }
    }

    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        _logger.LogInformation("PerformanceMetricsService started");

        var lastPersist = DateTime.UtcNow;

        while (!stoppingToken.IsCancellationRequested)
        {
            try
            {
                await Task.Delay(TimeSpan.FromSeconds(SampleIntervalSeconds), stoppingToken);

                var snapshot = GetCurrentMetrics();

                _logger.LogInformation(
                    "📊 Performance: {Clients} clients, {Sources} sources, {Buffers} active buffers, {MemorySavedMB:F2}MB saved, {AvgBroadcastMs:F2}ms avg broadcast",
                    snapshot.ActiveClients, snapshot.ActiveSources, snapshot.ActiveZeroCopyBuffers,
                    snapshot.MemorySavedBytes / 1024.0 / 1024.0, snapshot.AverageBroadcastTimeMs);

                // Persist to database every PersistIntervalMinutes
                if ((DateTime.UtcNow - lastPersist).TotalMinutes >= PersistIntervalMinutes)
                {
                    await PersistMetricsAsync(snapshot, stoppingToken);
                    lastPersist = DateTime.UtcNow;
                }
            }
            catch (OperationCanceledException)
            {
                break;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error sampling performance metrics");
            }
        }

        _logger.LogInformation("PerformanceMetricsService stopped");
    }

    private async Task PersistMetricsAsync(PerformanceSnapshot snapshot, CancellationToken cancellationToken)
    {
        try
        {
            using var scope = _serviceProvider.CreateScope();
            var dbContext = scope.ServiceProvider.GetRequiredService<ApplicationDbContext>();

            var metric = new PerformanceMetric
            {
                Timestamp = snapshot.Timestamp,
                TotalBytesSent = snapshot.TotalBytesSent,
                MemorySavedBytes = snapshot.MemorySavedBytes,
                ActiveZeroCopyBuffers = snapshot.ActiveZeroCopyBuffers,
                PeakZeroCopyBuffers = snapshot.PeakZeroCopyBuffers,
                AverageBroadcastTimeMs = snapshot.AverageBroadcastTimeMs,
                PeakBroadcastTimeMs = snapshot.PeakBroadcastTimeMs,
                TotalBroadcasts = snapshot.TotalBroadcasts,
                ActiveClients = snapshot.ActiveClients,
                ActiveSources = snapshot.ActiveSources,
                TotalMemoryUsageBytes = snapshot.TotalMemoryUsageBytes,
                GcCollectionCount = snapshot.GcCollectionCount
            };

            dbContext.PerformanceMetrics.Add(metric);
            await dbContext.SaveChangesAsync(cancellationToken);

            _logger.LogDebug("Persisted performance metrics to database");
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to persist performance metrics");
        }
    }
}

/// <summary>
/// Performance metrics snapshot (in-memory, not persisted)
/// </summary>
public class PerformanceSnapshot
{
    public DateTime Timestamp { get; set; }
    public long TotalBytesSent { get; set; }
    public long MemorySavedBytes { get; set; }
    public int ActiveZeroCopyBuffers { get; set; }
    public int PeakZeroCopyBuffers { get; set; }
    public double AverageBroadcastTimeMs { get; set; }
    public double PeakBroadcastTimeMs { get; set; }
    public int TotalBroadcasts { get; set; }
    public int ActiveClients { get; set; }
    public int ActiveSources { get; set; }
    public long TotalMemoryUsageBytes { get; set; }
    public long GcCollectionCount { get; set; }
}
