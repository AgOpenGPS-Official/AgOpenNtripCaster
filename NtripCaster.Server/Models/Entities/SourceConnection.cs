namespace NtripCaster.Server.Models.Entities;

public class SourceConnection
{
    public int Id { get; set; }
    public int MountPointId { get; set; }
    public DateTime ConnectedAt { get; set; } = DateTime.UtcNow;
    public DateTime? DisconnectedAt { get; set; }
    public long BytesReceived { get; set; }
    public long BytesSent { get; set; }
    public SourceConnectionStatus Status { get; set; } = SourceConnectionStatus.Connected;

    // Relations
    public MountPoint? MountPoint { get; set; }
}

public enum SourceConnectionStatus
{
    Connected,
    Streaming,      // Actively receiving RTCM from source
    Disconnected
}
