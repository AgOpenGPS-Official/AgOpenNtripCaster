namespace AgOpenNtripCaster.Server.Models.DTOs;

/// <summary>
/// Performance metrics response DTO
/// Real-time zero-copy broadcasting performance statistics
/// </summary>
public class PerformanceMetricsResponse
{
    public DateTime Timestamp { get; set; }

    // Zero-copy metrics
    public long TotalBytesSent { get; set; }
    public long MemorySavedBytes { get; set; }
    public double MemorySavedMB => MemorySavedBytes / 1024.0 / 1024.0;
    public int ActiveZeroCopyBuffers { get; set; }
    public int PeakZeroCopyBuffers { get; set; }

    // Broadcast performance
    public double AverageBroadcastTimeMs { get; set; }
    public double PeakBroadcastTimeMs { get; set; }
    public int TotalBroadcasts { get; set; }

    // Client/Source statistics
    public int ActiveClients { get; set; }
    public int ActiveSources { get; set; }
    public int TotalMountPoints { get; set; }

    // Memory statistics
    public long TotalMemoryUsageBytes { get; set; }
    public double TotalMemoryUsageMB => TotalMemoryUsageBytes / 1024.0 / 1024.0;
    public long GcCollectionCount { get; set; }
}
