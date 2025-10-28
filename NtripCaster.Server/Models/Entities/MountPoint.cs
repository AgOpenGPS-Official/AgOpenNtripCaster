namespace NtripCaster.Server.Models.Entities;

public class MountPoint
{
    public int Id { get; set; }
    public string Name { get; set; } = string.Empty;                    // "STATION_A"
    public string SourcePassword { get; set; } = string.Empty;         // Password for GNSS stations
    public string Description { get; set; } = string.Empty;
    public int Latitude { get; set; }                                  // For sourcetable
    public int Longitude { get; set; }
    public string Format { get; set; } = "RTCM3";
    public bool RequireClientAuthentication { get; set; } = true;
    public bool IsActive { get; set; } = true;
    public int MaxClients { get; set; } = 10;
    public DateTime CreatedAt { get; set; } = DateTime.UtcNow;

    // Relations
    public ICollection<NtripGroup> AllowedGroups { get; set; } = new List<NtripGroup>();
    public ICollection<ClientSession> ClientSessions { get; set; } = new List<ClientSession>();
    public ICollection<SourceConnection> SourceConnections { get; set; } = new List<SourceConnection>();
}
