namespace AgOpenNtripCaster.Server.Models.Entities;

/// <summary>
/// SMTP email configuration settings
/// Stored in database so admins can configure via UI
/// </summary>
public class EmailSmtpSettings
{
    public int Id { get; set; }

    // SMTP Server Configuration
    public string Host { get; set; } = string.Empty;                // e.g., smtp.gmail.com
    public int Port { get; set; } = 587;                           // 587 for TLS, 465 for SSL
    public string Username { get; set; } = string.Empty;
    public string Password { get; set; } = string.Empty;

    // Email Display Settings
    public string FromEmail { get; set; } = string.Empty;          // e.g., noreply@ntripcaster.local
    public string FromName { get; set; } = "NtripCaster";

    // Connection Settings
    public bool EnableSsl { get; set; } = true;
    public bool EnableTls { get; set; } = true;

    // Metadata
    public DateTime UpdatedAt { get; set; } = DateTime.UtcNow;
    public bool IsConfigured { get; set; } = false;                // Admin confirmation that settings are complete
}
