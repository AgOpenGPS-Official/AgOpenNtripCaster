namespace AgOpenNtripCaster.Server.Models.Entities;

/// <summary>
/// Email trigger configuration settings
/// Controls which email notifications are sent
/// </summary>
public class EmailTriggerSettings
{
    public int Id { get; set; }

    /// <summary>
    /// Send verification email when user registers
    /// </summary>
    public bool SendVerificationEmail { get; set; } = true;

    /// <summary>
    /// Send welcome email after email verification
    /// </summary>
    public bool SendWelcomeEmail { get; set; } = true;

    /// <summary>
    /// Send email when GNSS source goes offline
    /// </summary>
    public bool SendSourceOfflineEmail { get; set; } = true;

    /// <summary>
    /// Send email when GNSS source comes back online
    /// </summary>
    public bool SendSourceOnlineEmail { get; set; } = true;

    /// <summary>
    /// Email address to notify when source goes offline/online (usually admin)
    /// </summary>
    public string AdminEmailForSourceNotifications { get; set; } = "admin@ntripcaster.local";

    /// <summary>
    /// When this settings was last updated
    /// </summary>
    public DateTime UpdatedAt { get; set; } = DateTime.UtcNow;
}
