namespace AgOpenNtripCaster.Server.Models.Entities;

public class MountPoint
{
    public int Id { get; set; }
    public string Name { get; set; } = string.Empty;                    // "STATION_A"
    public string SourcePassword { get; set; } = string.Empty;         // Password for GNSS stations
    public string Description { get; set; } = string.Empty;
    public decimal? Latitude { get; set; }                             // Static/fallback latitude
    public decimal? Longitude { get; set; }                            // Static/fallback longitude
    public string Format { get; set; } = "RTCM3";
    public string? FormatDetails { get; set; }                          // Format details for sourcetable: "1005(10),1074(1),..."
    public string? Identifier { get; set; }                             // Source identifier for sourcetable (location name)
    public bool RequireClientAuthentication { get; set; } = true;
    public bool IsActive { get; set; } = true;
    public int MaxClients { get; set; } = 10;
    public DateTime CreatedAt { get; set; } = DateTime.UtcNow;

    // Owner tracking - user who created this source
    public string? UserId { get; set; }                                 // Optional: null = admin-owned source, value = user-owned source

    // RTCM-extracted data (from 1005 messages)
    public decimal? RtcmLatitude { get; set; }                          // Extracted from RTCM 1005
    public decimal? RtcmLongitude { get; set; }                         // Extracted from RTCM 1005
    public int? ReferenceStationId { get; set; }                        // From RTCM 1005

    // Auto-detection fields
    public string? DetectedFormat { get; set; }                         // Auto-detected: RTCM3, RTCM2.3, SPARTN
    public string? DetectedNavSystems { get; set; }                     // Auto-detected: GPS, GLONASS, GALILEO, combined
    public DateTime? LastRtcmMessageTime { get; set; }                  // Last RTCM message received
    public int MessageCount { get; set; } = 0;                          // Total RTCM messages received
    public int? BytesPerSecond { get; set; }                            // Calculated bitrate

    // Relations
    public NtripUser? Owner { get; set; }                               // User who owns this source (if user-created)
    public ICollection<NtripGroup> AllowedGroups { get; set; } = new List<NtripGroup>();
    public ICollection<ClientSession> ClientSessions { get; set; } = new List<ClientSession>();
    public ICollection<SourceConnection> SourceConnections { get; set; } = new List<SourceConnection>();
}
