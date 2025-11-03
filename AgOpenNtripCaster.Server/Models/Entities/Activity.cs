namespace AgOpenNtripCaster.Server.Models.Entities;

public class Activity
{
    public int Id { get; set; }
    public ActivityType Type { get; set; }
    public string LogLevel { get; set; } = "INFO"; // DEBUG, INFO, WARNING, ERROR
    public int? MountPointId { get; set; }  // Nullable for user actions not tied to mount points
    public string? UserId { get; set; }  // For client activities; null for source activities
    public string Description { get; set; } = string.Empty;
    public DateTime CreatedAt { get; set; } = DateTime.UtcNow;

    // Relations
    public MountPoint? MountPoint { get; set; }
    public NtripUser? User { get; set; }
}

public enum ActivityType
{
    // NTRIP Connection Events
    SourceConnected,
    SourceDisconnected,
    ClientConnected,
    ClientDisconnected,

    // User Authentication Events
    UserLogin,
    UserLogout,

    // Mount Point Management Events
    MountPointCreated,
    MountPointUpdated,
    MountPointDeleted,

    // Group Management Events
    GroupCreated,
    GroupUpdated,
    GroupDeleted,

    // Permission Management Events
    PermissionsChanged,
    GroupPermissionGranted,
    GroupPermissionRevoked,

    // User Management Events
    UserCreated,
    UserUpdated,
    UserDeleted,

    // Configuration Events
    ConfigurationChanged,

    // System Events
    SystemStarted,
    SystemShutdown
}
