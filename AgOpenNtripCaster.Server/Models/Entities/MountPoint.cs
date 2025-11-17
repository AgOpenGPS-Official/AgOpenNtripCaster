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

    // NTRIP 2.0 Sourcetable fields (STR entry)
    public string? Identifier { get; set; }                             // Field 3: Source identifier (location name)
    public string? FormatDetails { get; set; }                          // Field 5: Format details: "1005(10),1074(1),..."
    public int Carrier { get; set; } = 2;                               // Field 6: 0=No, 1=L1, 2=L1+L2
    public string? NavSystem { get; set; }                              // Field 7: GPS, GPS+GLO+GAL+BDS, etc. (null = auto-detect)
    public string Network { get; set; } = "NONE";                       // Field 8: Network name
    public string Country { get; set; } = "ENG";                        // Field 9: ISO 3166 country code (3 chars)
    public bool NmeaRequired { get; set; } = false;                     // Field 12: Requires NMEA/GGA input
    public int Solution { get; set; } = 0;                              // Field 13: 0=Single base, 1=Network
    public string Generator { get; set; } = "sNTRIP";                   // Field 14: Software/hardware generator
    public string Compression { get; set; } = "NONE";                   // Field 15: Compression algorithm
    public string Authentication { get; set; } = "N";                   // Field 16: N, B (Basic), D (Digest), or B,D
    public bool FeeRequired { get; set; } = false;                      // Field 17: Fee required for access
    public string? Misc { get; set; }                                   // Field 19: Miscellaneous (URL, etc.)

    public bool RequireClientAuthentication { get; set; } = true;       // Legacy field - maps to Authentication
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
