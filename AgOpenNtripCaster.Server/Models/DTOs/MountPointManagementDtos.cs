namespace AgOpenNtripCaster.Server.Models.DTOs;

/// <summary>
/// Create mount point request
/// </summary>
public class CreateMountPointRequest
{
    public string Name { get; set; } = string.Empty;
    public string Description { get; set; } = string.Empty;
    public string SourcePassword { get; set; } = string.Empty;
    public bool RequireClientAuthentication { get; set; } = true;
    public bool IsActive { get; set; } = true;
    public List<int> AllowedGroupIds { get; set; } = new();

    // Fallback coordinates
    public decimal? Latitude { get; set; }
    public decimal? Longitude { get; set; }

    // NTRIP 2.0 Sourcetable configuration
    public string? Identifier { get; set; }          // Source identifier (location name)
    public string? FormatDetails { get; set; }       // RTCM message types: "1005(10),1074(1),..."
    public int? Carrier { get; set; }                // 0=No, 1=L1, 2=L1+L2
    public string? NavSystem { get; set; }           // GPS, GPS+GLO+GAL+BDS, etc.
    public string? Network { get; set; }             // Network name
    public string? Country { get; set; }             // ISO 3166 country code (3 chars)
    public bool? NmeaRequired { get; set; }          // Requires NMEA/GGA input
    public int? Solution { get; set; }               // 0=Single base, 1=Network
    public string? Generator { get; set; }           // Software/hardware generator
    public string? Compression { get; set; }         // Compression algorithm
    public string? Authentication { get; set; }      // N, B (Basic), D (Digest), or B,D
    public bool? FeeRequired { get; set; }           // Fee required
    public string? Misc { get; set; }                // Miscellaneous (URL, etc.)
}

/// <summary>
/// Update mount point request
/// </summary>
public class UpdateMountPointRequest
{
    public string? Description { get; set; }
    public string? SourcePassword { get; set; }
    public bool? RequireClientAuthentication { get; set; }
    public bool? IsActive { get; set; }
    public List<int>? AllowedGroupIds { get; set; }

    // Fallback coordinates
    public decimal? Latitude { get; set; }
    public decimal? Longitude { get; set; }

    // NTRIP 2.0 Sourcetable configuration
    public string? Identifier { get; set; }          // Source identifier (location name)
    public string? FormatDetails { get; set; }       // RTCM message types: "1005(10),1074(1),..."
    public int? Carrier { get; set; }                // 0=No, 1=L1, 2=L1+L2
    public string? NavSystem { get; set; }           // GPS, GPS+GLO+GAL+BDS, etc.
    public string? Network { get; set; }             // Network name
    public string? Country { get; set; }             // ISO 3166 country code (3 chars)
    public bool? NmeaRequired { get; set; }          // Requires NMEA/GGA input
    public int? Solution { get; set; }               // 0=Single base, 1=Network
    public string? Generator { get; set; }           // Software/hardware generator
    public string? Compression { get; set; }         // Compression algorithm
    public string? Authentication { get; set; }      // N, B (Basic), D (Digest), or B,D
    public bool? FeeRequired { get; set; }           // Fee required
    public string? Misc { get; set; }                // Miscellaneous (URL, etc.)
}

/// <summary>
/// Mount point data transfer object
/// </summary>
public class MountPointDto
{
    public int Id { get; set; }
    public string Name { get; set; } = string.Empty;
    public string Description { get; set; } = string.Empty;
    public bool RequireClientAuthentication { get; set; }
    public bool IsActive { get; set; }
    public DateTime CreatedAt { get; set; }

    // Coordinates - fallback (manual entry)
    public decimal? Latitude { get; set; }
    public decimal? Longitude { get; set; }

    // Coordinates - extracted from RTCM1005
    public decimal? RtcmLatitude { get; set; }
    public decimal? RtcmLongitude { get; set; }
    public int? ReferenceStationId { get; set; }

    // RTCM message tracking
    public DateTime? LastRtcmMessageTime { get; set; }
    public int MessageCount { get; set; }

    // NTRIP 2.0 Sourcetable configuration
    public string? Identifier { get; set; }          // Source identifier (location name)
    public string? FormatDetails { get; set; }       // RTCM message types: "1005(10),1074(1),..."
    public int Carrier { get; set; }                 // 0=No, 1=L1, 2=L1+L2
    public string? NavSystem { get; set; }           // GPS, GPS+GLO+GAL+BDS, etc.
    public string Network { get; set; } = string.Empty;    // Network name
    public string Country { get; set; } = string.Empty;    // ISO 3166 country code
    public bool NmeaRequired { get; set; }           // Requires NMEA/GGA input
    public int Solution { get; set; }                // 0=Single base, 1=Network
    public string Generator { get; set; } = string.Empty;  // Software/hardware generator
    public string Compression { get; set; } = string.Empty; // Compression algorithm
    public string Authentication { get; set; } = string.Empty; // N, B, D, or B,D
    public bool FeeRequired { get; set; }            // Fee required
    public string? Misc { get; set; }                // Miscellaneous (URL, etc.)

    public int ActiveSourceCount { get; set; }
    public int ActiveClientCount { get; set; }
    public List<string> AllowedGroupNames { get; set; } = new();
    public string? UserId { get; set; }
    public string? OwnerFullName { get; set; }
    public string? OwnerEmail { get; set; }
}

/// <summary>
/// Mount point list response
/// </summary>
public class MountPointListResponse
{
    public int Total { get; set; }
    public int Page { get; set; }
    public int PageSize { get; set; }
    public List<MountPointDto> MountPoints { get; set; } = new();
}

/// <summary>
/// Create mount point response
/// </summary>
public class CreateMountPointResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
    public MountPointDto? MountPoint { get; set; }
}

/// <summary>
/// Update mount point response
/// </summary>
public class UpdateMountPointResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
    public MountPointDto? MountPoint { get; set; }
}

/// <summary>
/// Delete mount point response
/// </summary>
public class DeleteMountPointResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
}

/// <summary>
/// Allow group to access mount point
/// </summary>
public class AllowGroupRequest
{
    public int GroupId { get; set; }
}

/// <summary>
/// Deny group access to mount point
/// </summary>
public class DenyGroupRequest
{
    public int GroupId { get; set; }
}

/// <summary>
/// Mount point permission response
/// </summary>
public class MountPointPermissionResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
    public MountPointDto? MountPoint { get; set; }
}
