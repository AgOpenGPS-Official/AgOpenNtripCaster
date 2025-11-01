using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using AgOpenNtripCaster.Server.Models.Entities;
using AgOpenNtripCaster.Server.Services.Email;

namespace AgOpenNtripCaster.Server.Controllers;

/// <summary>
/// Admin endpoints for managing SMTP email configuration
/// </summary>
[ApiController]
[Route("api/admin/smtp")]
[Authorize(Roles = "Admin")]
public class AdminSmtpController : ControllerBase
{
    private readonly IEmailSmtpSettingsService _smtpSettingsService;
    private readonly IEmailService _emailService;
    private readonly ILogger<AdminSmtpController> _logger;

    public AdminSmtpController(
        IEmailSmtpSettingsService smtpSettingsService,
        IEmailService emailService,
        ILogger<AdminSmtpController> logger)
    {
        _smtpSettingsService = smtpSettingsService;
        _emailService = emailService;
        _logger = logger;
    }

    /// <summary>
    /// Get current SMTP settings
    /// </summary>
    [HttpGet("settings")]
    public async Task<ActionResult<EmailSmtpSettings>> GetSettings()
    {
        try
        {
            var settings = await _smtpSettingsService.GetSettingsAsync();
            // Don't return password in response for security
            settings.Password = string.IsNullOrEmpty(settings.Password) ? "" : "•••••••••";
            return Ok(settings);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error getting SMTP settings");
            return StatusCode(500, new { message = "Failed to get SMTP settings" });
        }
    }

    /// <summary>
    /// Update SMTP settings
    /// </summary>
    [HttpPut("settings")]
    public async Task<ActionResult<EmailSmtpSettings>> UpdateSettings([FromBody] EmailSmtpSettingsRequest request)
    {
        try
        {
            if (string.IsNullOrEmpty(request.Host))
                return BadRequest(new { message = "SMTP host is required" });

            if (request.Port < 1 || request.Port > 65535)
                return BadRequest(new { message = "SMTP port must be between 1 and 65535" });

            if (string.IsNullOrEmpty(request.FromEmail))
                return BadRequest(new { message = "From email is required" });

            var currentSettings = await _smtpSettingsService.GetSettingsAsync();

            // Keep existing password if not provided (masked password means no change)
            if (string.IsNullOrEmpty(request.Password) || request.Password.StartsWith("•"))
            {
                request.Password = currentSettings.Password;
            }

            var settings = new EmailSmtpSettings
            {
                Host = request.Host,
                Port = request.Port,
                Username = request.Username,
                Password = request.Password,
                FromEmail = request.FromEmail,
                FromName = request.FromName,
                EnableSsl = request.EnableSsl,
                EnableTls = request.EnableTls,
                IsConfigured = request.IsConfigured
            };

            var updated = await _smtpSettingsService.UpdateSettingsAsync(settings);
            // Don't return password in response for security
            updated.Password = string.IsNullOrEmpty(updated.Password) ? "" : "•••••••••";

            _logger.LogInformation("SMTP settings updated by admin");
            return Ok(updated);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error updating SMTP settings");
            return StatusCode(500, new { message = "Failed to update SMTP settings" });
        }
    }

    /// <summary>
    /// Test SMTP connection with provided settings
    /// </summary>
    [HttpPost("test")]
    public async Task<ActionResult<object>> TestSmtpConnection([FromBody] EmailSmtpSettingsRequest request)
    {
        try
        {
            if (string.IsNullOrEmpty(request.Host))
                return BadRequest(new { message = "SMTP host is required" });

            if (string.IsNullOrEmpty(request.FromEmail))
                return BadRequest(new { message = "From email is required" });

            // Try to send a test email
            var success = await _emailService.SendTestEmailAsync(request.FromEmail);

            if (success)
            {
                return Ok(new { message = "SMTP connection successful", success = true });
            }
            else
            {
                return BadRequest(new { message = "Failed to send test email", success = false });
            }
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error testing SMTP connection");
            return BadRequest(new { message = $"SMTP connection failed: {ex.Message}", success = false });
        }
    }
}

/// <summary>
/// Request model for SMTP settings (password is masked in response)
/// </summary>
public class EmailSmtpSettingsRequest
{
    public string Host { get; set; } = string.Empty;
    public int Port { get; set; } = 587;
    public string Username { get; set; } = string.Empty;
    public string Password { get; set; } = string.Empty;
    public string FromEmail { get; set; } = string.Empty;
    public string FromName { get; set; } = "NtripCaster";
    public bool EnableSsl { get; set; } = true;
    public bool EnableTls { get; set; } = true;
    public bool IsConfigured { get; set; } = false;
}
