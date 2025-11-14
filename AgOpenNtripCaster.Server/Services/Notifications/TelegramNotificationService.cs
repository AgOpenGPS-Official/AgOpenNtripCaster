using Telegram.Bot;
using Telegram.Bot.Types;

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
    private readonly TelegramBotClient? _botClient;
    private readonly long? _chatId;
    private readonly bool _enabled;
    private readonly ILogger<TelegramNotificationService> _logger;

    public TelegramNotificationService(
        IConfiguration configuration,
        ILogger<TelegramNotificationService> logger)
    {
        _logger = logger;

        var botToken = configuration["Telegram:BotToken"];
        var chatIdStr = configuration["Telegram:ChatId"];
        _enabled = configuration.GetValue<bool>("Telegram:Enabled", false);

        if (_enabled && !string.IsNullOrEmpty(botToken) && !string.IsNullOrEmpty(chatIdStr))
        {
            try
            {
                _botClient = new TelegramBotClient(botToken);
                _chatId = long.Parse(chatIdStr);
                _logger.LogInformation("Telegram notification service initialized for chat ID {ChatId}", _chatId);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Failed to initialize Telegram bot");
                _enabled = false;
            }
        }
        else
        {
            _logger.LogInformation("Telegram notifications are disabled or not configured");
        }
    }

    public async Task SendNotificationAsync(string message, CancellationToken cancellationToken = default)
    {
        if (!_enabled || _botClient == null || _chatId == null)
            return;

        try
        {
            await _botClient.SendMessage(
                chatId: _chatId.Value,
                text: message,
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
        var message = $"📡 *Source Connected*\n\n" +
                     $"Mount Point: `{mountPointName}`\n" +
                     $"Time: {DateTime.UtcNow:yyyy-MM-dd HH:mm:ss} UTC";

        await SendNotificationAsync(message, cancellationToken);
    }

    public async Task SendSourceDisconnectedAsync(string mountPointName, CancellationToken cancellationToken = default)
    {
        var message = $"⚠️ *Source Disconnected*\n\n" +
                     $"Mount Point: `{mountPointName}`\n" +
                     $"Time: {DateTime.UtcNow:yyyy-MM-dd HH:mm:ss} UTC";

        await SendNotificationAsync(message, cancellationToken);
    }

    public async Task SendErrorAsync(string errorMessage, CancellationToken cancellationToken = default)
    {
        var message = $"🔴 *Error*\n\n" +
                     $"{errorMessage}\n" +
                     $"Time: {DateTime.UtcNow:yyyy-MM-dd HH:mm:ss} UTC";

        await SendNotificationAsync(message, cancellationToken);
    }

    public async Task SendSystemStartedAsync(CancellationToken cancellationToken = default)
    {
        var message = $"✅ *NTRIP Caster Started*\n\n" +
                     $"System is now online and ready to accept connections.\n" +
                     $"Time: {DateTime.UtcNow:yyyy-MM-dd HH:mm:ss} UTC";

        await SendNotificationAsync(message, cancellationToken);
    }
}
