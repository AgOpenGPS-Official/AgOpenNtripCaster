namespace AgOpenNtripCaster.Server.Models.DTOs;

/// <summary>
/// Response DTO for NetworkInfo
/// </summary>
public class NetworkInfoDto
{
    public int Id { get; set; }
    public string Identifier { get; set; } = "NTRIP";
    public string Operator { get; set; } = "AgOpen";
    public char AuthenticationRequired { get; set; } = 'Y';
    public char FeeRequired { get; set; } = 'N';
    public string Website { get; set; } = "https://github.com/AgOpenGPS";
    public string Email { get; set; } = "info@agopenrtk.local";
    public DateTime StartDate { get; set; }
    public DateTime EndDate { get; set; }
    public DateTime UpdatedAt { get; set; }
}

/// <summary>
/// Request DTO for creating/updating NetworkInfo
/// </summary>
public class UpdateNetworkInfoRequest
{
    public string Identifier { get; set; } = "NTRIP";
    public string Operator { get; set; } = "AgOpen";
    public string AuthenticationRequired { get; set; } = "Y";
    public string FeeRequired { get; set; } = "N";
    public string Website { get; set; } = "https://github.com/AgOpenGPS";
    public string Email { get; set; } = "info@agopenrtk.local";
    public DateTime StartDate { get; set; } = DateTime.UtcNow;
    public DateTime EndDate { get; set; } = DateTime.UtcNow.AddYears(1);
}
