namespace AgOpenNtripCaster.Server.Models.Entities;

/// <summary>
/// Performance metrics for zero-copy broadcasting
/// Tracks memory savings, buffer usage, and broadcast performance
/// </summary>
public class PerformanceMetric
{
    public int Id { get; set; }
    public DateTime Timestamp { get; set; } = DateTime.UtcNow;

    // Zero-copy metrics
    public long TotalBytesSent { get; set; }
    public long MemorySavedBytes { get; set; }
    public int ActiveZeroCopyBuffers { get; set; }
    public int PeakZeroCopyBuffers { get; set; }

    // Broadcast performance
    public double AverageBroadcastTimeMs { get; set; }
    public double PeakBroadcastTimeMs { get; set; }
    public int TotalBroadcasts { get; set; }

    // Client statistics
    public int ActiveClients { get; set; }
    public int ActiveSources { get; set; }
    public int TotalMountPoints { get; set; }

    // Memory statistics
    public long TotalMemoryUsageBytes { get; set; }
    public long GcCollectionCount { get; set; }
}
