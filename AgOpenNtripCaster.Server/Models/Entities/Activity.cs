namespace AgOpenNtripCaster.Server.Models.Entities;

public class Activity
{
    public int Id { get; set; }
    public ActivityType Type { get; set; }
    public int MountPointId { get; set; }
    public string? UserId { get; set; }  // For client activities; null for source activities
    public string Description { get; set; } = string.Empty;
    public DateTime CreatedAt { get; set; } = DateTime.UtcNow;

    // Relations
    public MountPoint? MountPoint { get; set; }
    public NtripUser? User { get; set; }
}

public enum ActivityType
{
    SourceConnected,
    SourceDisconnected,
    ClientConnected,
    ClientDisconnected
}
