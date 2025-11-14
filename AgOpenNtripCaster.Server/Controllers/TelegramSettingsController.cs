using AgOpenNtripCaster.Server.Models.Entities;
using AgOpenNtripCaster.Server.Services.Notifications;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

namespace AgOpenNtripCaster.Server.Controllers;

[ApiController]
[Route("api/telegram-settings")]
[Authorize(Roles = "Admin")]
public class TelegramSettingsController : ControllerBase
{
    private readonly ITelegramSettingsService _telegramSettingsService;
    private readonly ILogger<TelegramSettingsController> _logger;

    public TelegramSettingsController(
        ITelegramSettingsService telegramSettingsService,
        ILogger<TelegramSettingsController> logger)
    {
        _telegramSettingsService = telegramSettingsService;
        _logger = logger;
    }

    [HttpGet]
    public async Task<ActionResult<TelegramSettings>> GetSettings()
    {
        try
        {
            var settings = await _telegramSettingsService.GetSettingsAsync();
            return Ok(settings);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving Telegram settings");
            return StatusCode(500, new { message = "Error retrieving settings" });
        }
    }

    [HttpPut]
    public async Task<ActionResult<TelegramSettings>> UpdateSettings([FromBody] TelegramSettings settings)
    {
        try
        {
            var updated = await _telegramSettingsService.UpdateSettingsAsync(settings);
            return Ok(updated);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error updating Telegram settings");
            return StatusCode(500, new { message = "Error updating settings" });
        }
    }

    [HttpPost("test")]
    public async Task<ActionResult> TestConnection()
    {
        try
        {
            var success = await _telegramSettingsService.TestConnectionAsync();

            if (success)
            {
                return Ok(new { message = "Test message sent successfully!" });
            }
            else
            {
                return BadRequest(new { message = "Failed to send test message. Check your settings and logs." });
            }
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error testing Telegram connection");
            return StatusCode(500, new { message = "Error testing connection" });
        }
    }
}
