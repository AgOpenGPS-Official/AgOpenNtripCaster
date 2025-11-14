using Telegram.Bot;
using Telegram.Bot.Types;
using AgOpenNtripCaster.Server.Data;
using Microsoft.EntityFrameworkCore;

namespace AgOpenNtripCaster.Server.Services.Notifications;

public interface ITelegramNotificationService
{
    Task SendNotificationAsync(string message, CancellationToken cancellationToken = default);
    Task SendSourceConnectedAsync(string mountPointName, CancellationToken cancellationToken = default);
    Task SendSourceDisconnectedAsync(string mountPointName, CancellationToken cancellationToken = default);
    Task SendErrorAsync(string errorMessage, CancellationToken cancellationToken = default);
    Task SendSystemStartedAsync(CancellationToken cancellationToken = default);
}

public class TelegramNotificationService : ITelegramNotificationService
{
    private readonly IServiceProvider _serviceProvider;
    private readonly ILogger<TelegramNotificationService> _logger;

    public TelegramNotificationService(
        IServiceProvider serviceProvider,
        ILogger<TelegramNotificationService> logger)
    {
        _serviceProvider = serviceProvider;
        _logger = logger;
    }

    private async Task<(bool enabled, string? botToken, string? chatId, bool notifySourceConnected, bool notifySourceDisconnected, bool notifySystemStarted, bool notifyErrors)> GetSettingsAsync()
    {
        try
        {
            using var scope = _serviceProvider.CreateScope();
            var dbContext = scope.ServiceProvider.GetRequiredService<ApplicationDbContext>();

            var settings = await dbContext.TelegramSettings.FirstOrDefaultAsync();

            if (settings != null && settings.Enabled && !string.IsNullOrEmpty(settings.BotToken) && !string.IsNullOrEmpty(settings.ChatId))
            {
                return (settings.Enabled, settings.BotToken, settings.ChatId, settings.NotifySourceConnected, settings.NotifySourceDisconnected, settings.NotifySystemStarted, settings.NotifyErrors);
            }

            return (false, null, null, false, false, false, false);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error loading Telegram settings from database");
            return (false, null, null, false, false, false, false);
        }
    }

    public async Task SendNotificationAsync(string message, CancellationToken cancellationToken = default)
    {
        var settings = await GetSettingsAsync();

        if (!settings.enabled || string.IsNullOrEmpty(settings.botToken) || string.IsNullOrEmpty(settings.chatId))
            return;

        try
        {
            var botClient = new TelegramBotClient(settings.botToken);
            var chatId = long.Parse(settings.chatId);

            await botClient.SendMessage(
                chatId: chatId,
                text: message,
                parseMode: Telegram.Bot.Types.Enums.ParseMode.Markdown,
                cancellationToken: cancellationToken);

            _logger.LogDebug("Telegram notification sent: {Message}", message);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to send Telegram notification");
        }
    }

    public async Task SendSourceConnectedAsync(string mountPointName, CancellationToken cancellationToken = default)
    {
        var settings = await GetSettingsAsync();
        if (!settings.enabled || !settings.notifySourceConnected)
            return;

        var message = $"📡 *Source Connected*\n\n" +
                     $"Mount Point: `{mountPointName}`\n" +
                     $"Time: {DateTime.UtcNow:yyyy-MM-dd HH:mm:ss} UTC";

        await SendNotificationAsync(message, cancellationToken);
    }

    public async Task SendSourceDisconnectedAsync(string mountPointName, CancellationToken cancellationToken = default)
    {
        var settings = await GetSettingsAsync();
        if (!settings.enabled || !settings.notifySourceDisconnected)
            return;

        var message = $"⚠️ *Source Disconnected*\n\n" +
                     $"Mount Point: `{mountPointName}`\n" +
                     $"Time: {DateTime.UtcNow:yyyy-MM-dd HH:mm:ss} UTC";

        await SendNotificationAsync(message, cancellationToken);
    }

    public async Task SendErrorAsync(string errorMessage, CancellationToken cancellationToken = default)
    {
        var settings = await GetSettingsAsync();
        if (!settings.enabled || !settings.notifyErrors)
            return;

        var message = $"🔴 *Error*\n\n" +
                     $"{errorMessage}\n" +
                     $"Time: {DateTime.UtcNow:yyyy-MM-dd HH:mm:ss} UTC";

        await SendNotificationAsync(message, cancellationToken);
    }

    public async Task SendSystemStartedAsync(CancellationToken cancellationToken = default)
    {
        var settings = await GetSettingsAsync();
        if (!settings.enabled || !settings.notifySystemStarted)
            return;

        var message = $"✅ *NTRIP Caster Started*\n\n" +
                     $"System is now online and ready to accept connections.\n" +
                     $"Time: {DateTime.UtcNow:yyyy-MM-dd HH:mm:ss} UTC";

        await SendNotificationAsync(message, cancellationToken);
    }
}
