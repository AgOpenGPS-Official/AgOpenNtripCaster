namespace NtripCaster.Server.Models.Entities;

public class ClientSession
{
    public string Id { get; set; } = Guid.NewGuid().ToString();
    public string UserId { get; set; } = string.Empty;
    public int MountPointId { get; set; }
    public DateTime ConnectedAt { get; set; } = DateTime.UtcNow;
    public DateTime? DisconnectedAt { get; set; }

    // Position tracking
    public double? LastLatitude { get; set; }
    public double? LastLongitude { get; set; }
    public double? LastAccuracy { get; set; }
    public DateTime? LastPositionAt { get; set; }

    // Stream status
    public ClientStreamStatus Status { get; set; } = ClientStreamStatus.Connected;
    public DateTime? LastStreamPauseAt { get; set; }

    // Statistics
    public long BytesReceived { get; set; }
    public long BytesSent { get; set; }

    // Relations
    public NtripUser? User { get; set; }
    public MountPoint? MountPoint { get; set; }
}

public enum ClientStreamStatus
{
    Connected,      // Just connected, awaiting position
    Streaming,      // Position fresh, actively streaming RTCM
    Paused,         // Position too old (>15 sec), stream on hold
    Disconnected    // Connection closed
}
