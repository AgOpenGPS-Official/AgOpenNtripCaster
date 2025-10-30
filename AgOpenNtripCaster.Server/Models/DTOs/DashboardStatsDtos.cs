namespace AgOpenNtripCaster.Server.Models.DTOs;

/// <summary>
/// Real-time dashboard statistics
/// </summary>
public class DashboardStatsDto
{
    /// <summary>
    /// Total number of active client sessions (rovers)
    /// </summary>
    public int ActiveClients { get; set; }

    /// <summary>
    /// Total number of active mount points (base stations)
    /// </summary>
    public int ActiveSources { get; set; }

    /// <summary>
    /// Total bytes received from all sources (in MB)
    /// </summary>
    public double TotalBytesReceived { get; set; }

    /// <summary>
    /// Total bytes sent to all clients (in MB)
    /// </summary>
    public double TotalBytesSent { get; set; }

    /// <summary>
    /// Combined total bytes (received + sent) in MB
    /// </summary>
    public double TotalBytesTransferred { get; set; }

    /// <summary>
    /// Server start time (for uptime calculation)
    /// </summary>
    public DateTime ServerStartTime { get; set; }

    /// <summary>
    /// Current time
    /// </summary>
    public DateTime CurrentTime { get; set; }

    /// <summary>
    /// Formatted uptime string (e.g., "5d 14h 32m")
    /// </summary>
    public string UptimeFormatted { get; set; } = string.Empty;
}
