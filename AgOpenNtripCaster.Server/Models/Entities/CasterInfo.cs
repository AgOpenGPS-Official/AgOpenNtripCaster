namespace AgOpenNtripCaster.Server.Models.Entities;

/// <summary>
/// Caster information for NTRIP 2.0 sourcetable
/// CAS;identifier;operator;nmea;country;lat;lon;fallback_host;port;misc
/// </summary>
public class CasterInfo
{
    public int Id { get; set; }

    /// <summary>
    /// CAS identifier (e.g., "agopencast", "ntripcaster")
    /// </summary>
    public string Identifier { get; set; } = "agopencast";

    /// <summary>
    /// Operator/caster name (e.g., "AgOpen NtripCaster", "RTK Provider XYZ")
    /// </summary>
    public string Operator { get; set; } = "AgOpen NtripCaster";

    /// <summary>
    /// NMEA support flag (0=no, 1=yes)
    /// </summary>
    public int NmeaSupport { get; set; } = 0;

    /// <summary>
    /// Country code (e.g., "NL", "DE", "US")
    /// </summary>
    public string Country { get; set; } = "NL";

    /// <summary>
    /// Caster latitude (decimal degrees)
    /// </summary>
    public decimal Latitude { get; set; } = 52.0m;

    /// <summary>
    /// Caster longitude (decimal degrees)
    /// </summary>
    public decimal Longitude { get; set; } = 5.0m;

    /// <summary>
    /// Fallback host (empty for primary caster)
    /// </summary>
    public string? FallbackHost { get; set; }

    /// <summary>
    /// NTRIP port (default 2101)
    /// </summary>
    public int Port { get; set; } = 2101;

    /// <summary>
    /// Miscellaneous description (e.g., "AgOpen GNSS RTK Server")
    /// </summary>
    public string Description { get; set; } = "AgOpen GNSS RTK Server";

    /// <summary>
    /// When this configuration was last updated
    /// </summary>
    public DateTime UpdatedAt { get; set; } = DateTime.UtcNow;
}
