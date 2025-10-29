namespace AgOpenNtripCaster.Server.Models.DTOs;

/// <summary>
/// Response DTO for CasterInfo
/// </summary>
public class CasterInfoDto
{
    public int Id { get; set; }
    public string Identifier { get; set; } = "agopencast";
    public string Operator { get; set; } = "AgOpen NtripCaster";
    public int NmeaSupport { get; set; } = 0;
    public string Country { get; set; } = "NL";
    public decimal Latitude { get; set; } = 52.0m;
    public decimal Longitude { get; set; } = 5.0m;
    public string? FallbackHost { get; set; }
    public int Port { get; set; } = 2101;
    public string Description { get; set; } = "AgOpen GNSS RTK Server";
    public DateTime UpdatedAt { get; set; }
}

/// <summary>
/// Request DTO for creating/updating CasterInfo
/// </summary>
public class UpdateCasterInfoRequest
{
    public string Identifier { get; set; } = "agopencast";
    public string Operator { get; set; } = "AgOpen NtripCaster";
    public int NmeaSupport { get; set; } = 0;
    public string Country { get; set; } = "NL";
    public decimal Latitude { get; set; } = 52.0m;
    public decimal Longitude { get; set; } = 5.0m;
    public string? FallbackHost { get; set; }
    public int Port { get; set; } = 2101;
    public string Description { get; set; } = "AgOpen GNSS RTK Server";
}
