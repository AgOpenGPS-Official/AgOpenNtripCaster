namespace AgOpenNtripCaster.Server.Models.Entities;

/// <summary>
/// Telegram notification settings stored in database
/// </summary>
public class TelegramSettings
{
    public int Id { get; set; }
    public bool Enabled { get; set; } = false;
    public string BotToken { get; set; } = string.Empty;
    public string ChatId { get; set; } = string.Empty;

    // Notification preferences
    public bool NotifySourceConnected { get; set; } = true;
    public bool NotifySourceDisconnected { get; set; } = true;
    public bool NotifySystemStarted { get; set; } = true;
    public bool NotifyErrors { get; set; } = true;

    public DateTime CreatedAt { get; set; } = DateTime.UtcNow;
    public DateTime UpdatedAt { get; set; } = DateTime.UtcNow;
}
