namespace AgOpenNtripCaster.Server.Models.Entities;

/// <summary>
/// Network operator information for NTRIP 2.0 sourcetable
/// NET;identifier;operator;auth;fee;website;email;startdate;enddate
/// </summary>
public class NetworkInfo
{
    public int Id { get; set; }

    /// <summary>
    /// Network identifier (e.g., "NTRIP", "RTK-NETWORK")
    /// </summary>
    public string Identifier { get; set; } = "NTRIP";

    /// <summary>
    /// Network operator name (e.g., "AgOpen", "RTK Provider")
    /// </summary>
    public string Operator { get; set; } = "AgOpen";

    /// <summary>
    /// Authentication required (Y/N)
    /// </summary>
    public char AuthenticationRequired { get; set; } = 'Y';

    /// <summary>
    /// Fee required (Y/N)
    /// </summary>
    public char FeeRequired { get; set; } = 'N';

    /// <summary>
    /// Website URL for more information
    /// </summary>
    public string Website { get; set; } = "https://github.com/AgOpenGPS";

    /// <summary>
    /// Contact email address
    /// </summary>
    public string Email { get; set; } = "info@agopenrtk.local";

    /// <summary>
    /// Service start date (YYYY-MM-DD format)
    /// </summary>
    public DateTime StartDate { get; set; } = DateTime.UtcNow;

    /// <summary>
    /// Service end date / license expiration (YYYY-MM-DD format)
    /// </summary>
    public DateTime EndDate { get; set; } = DateTime.UtcNow.AddYears(1);

    /// <summary>
    /// When this configuration was last updated
    /// </summary>
    public DateTime UpdatedAt { get; set; } = DateTime.UtcNow;
}
