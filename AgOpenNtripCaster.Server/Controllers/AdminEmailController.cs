using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using AgOpenNtripCaster.Server.Models.Entities;
using AgOpenNtripCaster.Server.Services.Email;

namespace AgOpenNtripCaster.Server.Controllers;

[ApiController]
[Route("api/admin/email")]
[Authorize(Roles = "Admin")]
public class AdminEmailController : ControllerBase
{
    private readonly IEmailService _emailService;
    private readonly IEmailTriggerSettingsService _triggerSettingsService;
    private readonly ILogger<AdminEmailController> _logger;

    public AdminEmailController(
        IEmailService emailService,
        IEmailTriggerSettingsService triggerSettingsService,
        ILogger<AdminEmailController> logger)
    {
        _emailService = emailService;
        _triggerSettingsService = triggerSettingsService;
        _logger = logger;
    }

    /// <summary>
    /// Send a test email to verify SMTP configuration
    /// </summary>
    [HttpPost("test")]
    [ProducesResponseType(typeof(object), StatusCodes.Status200OK)]
    [ProducesResponseType(typeof(object), StatusCodes.Status400BadRequest)]
    public async Task<IActionResult> SendTestEmail([FromQuery] string email)
    {
        if (string.IsNullOrWhiteSpace(email))
        {
            return BadRequest(new { message = "Email address is required" });
        }

        try
        {
            var sent = await _emailService.SendTestEmailAsync(email);

            if (sent)
            {
                _logger.LogInformation($"Test email sent to {email}");
                return Ok(new { message = "Test email sent successfully", email });
            }
            else
            {
                return BadRequest(new { message = "Failed to send test email. Check your SMTP configuration." });
            }
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error sending test email");
            return StatusCode(500, new { message = "Internal server error", error = ex.Message });
        }
    }

    /// <summary>
    /// Get current email trigger settings
    /// </summary>
    [HttpGet("settings")]
    [ProducesResponseType(typeof(EmailTriggerSettings), StatusCodes.Status200OK)]
    public async Task<IActionResult> GetEmailSettings()
    {
        try
        {
            var settings = await _triggerSettingsService.GetSettingsAsync();
            return Ok(settings);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error getting email settings");
            return StatusCode(500, new { message = "Internal server error", error = ex.Message });
        }
    }

    /// <summary>
    /// Update email trigger settings
    /// </summary>
    [HttpPut("settings")]
    [ProducesResponseType(typeof(EmailTriggerSettings), StatusCodes.Status200OK)]
    [ProducesResponseType(typeof(object), StatusCodes.Status400BadRequest)]
    public async Task<IActionResult> UpdateEmailSettings([FromBody] EmailTriggerSettings settings)
    {
        if (settings == null)
        {
            return BadRequest(new { message = "Settings cannot be null" });
        }

        try
        {
            var updated = await _triggerSettingsService.UpdateSettingsAsync(settings);
            _logger.LogInformation("Email trigger settings updated");
            return Ok(updated);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error updating email settings");
            return StatusCode(500, new { message = "Internal server error", error = ex.Message });
        }
    }
}
