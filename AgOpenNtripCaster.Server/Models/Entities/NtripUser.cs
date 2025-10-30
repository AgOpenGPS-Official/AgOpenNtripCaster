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

    /// <summary>
    /// Last generated source password (plain text, temporary)
    /// Used to display to user after generation
    /// Should only be shown once and then cleared
    /// </summary>
    public string? LastGeneratedSourcePassword { get; set; }

    /// <summary>
    /// Timestamp when the source password was last generated
    /// Used to track when to clear the temporary plain password
    /// </summary>
    public DateTime? SourcePasswordGeneratedAt { get; set; }

    // Relations
    public ICollection<NtripGroup> Groups { get; set; } = new List<NtripGroup>();
    public ICollection<ClientSession> ClientSessions { get; set; } = new List<ClientSession>();
    public ICollection<MountPoint> OwnedMountPoints { get; set; } = new List<MountPoint>();  // Sources created by this user
}
