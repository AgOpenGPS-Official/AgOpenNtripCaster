using Microsoft.AspNetCore.Identity;
using System.Collections.Generic;

namespace AgOpenNtripCaster.Server.Models.Entities;

public class NtripUser : IdentityUser
{
    public string FullName { get; set; } = string.Empty;
    public DateTime CreatedAt { get; set; } = DateTime.UtcNow;
    public int MaxConnections { get; set; } = 5;
    public bool IsActive { get; set; } = true;

    // Refresh token for JWT refresh flow
    public string? RefreshToken { get; set; }
    public DateTime? RefreshTokenExpires { get; set; }

    /// <summary>
    /// Source password used by BaseStations to authenticate when uploading RTCM data
    /// Stored as hashed value (bcrypt)
    /// </summary>
    public string? SourcePassword { get; set; }

    // Relations
    public ICollection<NtripGroup> Groups { get; set; } = new List<NtripGroup>();
    public ICollection<ClientSession> ClientSessions { get; set; } = new List<ClientSession>();
    public ICollection<MountPoint> OwnedMountPoints { get; set; } = new List<MountPoint>();  // Sources created by this user
}
