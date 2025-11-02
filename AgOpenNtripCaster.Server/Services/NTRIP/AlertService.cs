using Microsoft.AspNetCore.SignalR;
using AgOpenNtripCaster.Server.Hubs;

namespace AgOpenNtripCaster.Server.Services.NTRIP;

/// <summary>
/// Alert severity levels
/// </summary>
public enum AlertSeverity
{
    Info,
    Warning,
    Error,
    Critical
}

/// <summary>
/// System alert data transfer object
/// </summary>
public class SystemAlert
{
    public string Id { get; set; } = Guid.NewGuid().ToString();
    public AlertSeverity Severity { get; set; }
    public string Title { get; set; } = string.Empty;
    public string Message { get; set; } = string.Empty;
    public DateTime CreatedAt { get; set; } = DateTime.UtcNow;
    public string? Code { get; set; } // Error/warning code for tracking
    public Dictionary<string, object>? Metadata { get; set; } // Additional context
}

/// <summary>
/// Service for creating and broadcasting system alerts
/// </summary>
public interface IAlertService
{
    Task SendInfoAsync(string title, string message, string? code = null, Dictionary<string, object>? metadata = null);
    Task SendWarningAsync(string title, string message, string? code = null, Dictionary<string, object>? metadata = null);
    Task SendErrorAsync(string title, string message, string? code = null, Dictionary<string, object>? metadata = null);
    Task SendCriticalAsync(string title, string message, string? code = null, Dictionary<string, object>? metadata = null);
}

public class AlertService : IAlertService
{
    private readonly IHubContext<NtripHub> _hubContext;
    private readonly ILogger<AlertService> _logger;

    public AlertService(IHubContext<NtripHub> hubContext, ILogger<AlertService> logger)
    {
        _hubContext = hubContext;
        _logger = logger;
    }

    public async Task SendInfoAsync(string title, string message, string? code = null, Dictionary<string, object>? metadata = null)
    {
        await SendAlertAsync(AlertSeverity.Info, title, message, code, metadata);
    }

    public async Task SendWarningAsync(string title, string message, string? code = null, Dictionary<string, object>? metadata = null)
    {
        await SendAlertAsync(AlertSeverity.Warning, title, message, code, metadata);
    }

    public async Task SendErrorAsync(string title, string message, string? code = null, Dictionary<string, object>? metadata = null)
    {
        await SendAlertAsync(AlertSeverity.Error, title, message, code, metadata);
    }

    public async Task SendCriticalAsync(string title, string message, string? code = null, Dictionary<string, object>? metadata = null)
    {
        await SendAlertAsync(AlertSeverity.Critical, title, message, code, metadata);
    }

    private async Task SendAlertAsync(
        AlertSeverity severity,
        string title,
        string message,
        string? code = null,
        Dictionary<string, object>? metadata = null)
    {
        try
        {
            var alert = new SystemAlert
            {
                Severity = severity,
                Title = title,
                Message = message,
                Code = code,
                Metadata = metadata,
                CreatedAt = DateTime.UtcNow
            };

            // Log the alert
            var logLevel = severity switch
            {
                AlertSeverity.Info => LogLevel.Information,
                AlertSeverity.Warning => LogLevel.Warning,
                AlertSeverity.Error => LogLevel.Error,
                AlertSeverity.Critical => LogLevel.Critical,
                _ => LogLevel.Information
            };

            _logger.Log(logLevel, "System alert: {Title} - {Message}", title, message);

            // Broadcast to all connected SignalR clients
            await _hubContext.Clients.All.SendAsync("SystemAlert", alert);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error sending system alert: {Title}", title);
        }
    }
}
