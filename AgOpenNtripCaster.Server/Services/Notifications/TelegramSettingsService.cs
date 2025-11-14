using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.Entities;
using Microsoft.EntityFrameworkCore;

namespace AgOpenNtripCaster.Server.Services.Notifications;

public interface ITelegramSettingsService
{
    Task<TelegramSettings> GetSettingsAsync();
    Task<TelegramSettings> UpdateSettingsAsync(TelegramSettings settings);
    Task<bool> TestConnectionAsync();
}

public class TelegramSettingsService : ITelegramSettingsService
{
    private readonly ApplicationDbContext _dbContext;
    private readonly ITelegramNotificationService _telegramService;
    private readonly ILogger<TelegramSettingsService> _logger;

    public TelegramSettingsService(
        ApplicationDbContext dbContext,
        ITelegramNotificationService telegramService,
        ILogger<TelegramSettingsService> logger)
    {
        _dbContext = dbContext;
        _telegramService = telegramService;
        _logger = logger;
    }

    public async Task<TelegramSettings> GetSettingsAsync()
    {
        var settings = await _dbContext.TelegramSettings.FirstOrDefaultAsync();

        if (settings == null)
        {
            // Create default settings if none exist
            settings = new TelegramSettings
            {
                Enabled = false,
                BotToken = string.Empty,
                ChatId = string.Empty,
                NotifySourceConnected = true,
                NotifySourceDisconnected = true,
                NotifySystemStarted = true,
                NotifyErrors = true
            };

            _dbContext.TelegramSettings.Add(settings);
            await _dbContext.SaveChangesAsync();
        }

        return settings;
    }

    public async Task<TelegramSettings> UpdateSettingsAsync(TelegramSettings settings)
    {
        var existing = await _dbContext.TelegramSettings.FirstOrDefaultAsync();

        if (existing == null)
        {
            // Create new
            settings.CreatedAt = DateTime.UtcNow;
            settings.UpdatedAt = DateTime.UtcNow;
            _dbContext.TelegramSettings.Add(settings);
        }
        else
        {
            // Update existing
            existing.Enabled = settings.Enabled;
            existing.BotToken = settings.BotToken;
            existing.ChatId = settings.ChatId;
            existing.NotifySourceConnected = settings.NotifySourceConnected;
            existing.NotifySourceDisconnected = settings.NotifySourceDisconnected;
            existing.NotifySystemStarted = settings.NotifySystemStarted;
            existing.NotifyErrors = settings.NotifyErrors;
            existing.UpdatedAt = DateTime.UtcNow;
        }

        await _dbContext.SaveChangesAsync();

        _logger.LogInformation("Telegram settings updated: Enabled={Enabled}", settings.Enabled);

        return existing ?? settings;
    }

    public async Task<bool> TestConnectionAsync()
    {
        try
        {
            await _telegramService.SendNotificationAsync(
                "🧪 *Test Message*\n\nTelegram notifications are working correctly!",
                CancellationToken.None);
            return true;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to send test Telegram message");
            return false;
        }
    }
}
