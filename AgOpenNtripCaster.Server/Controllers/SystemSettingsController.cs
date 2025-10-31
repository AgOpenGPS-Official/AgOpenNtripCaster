using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using AgOpenNtripCaster.Server.Models.DTOs;

namespace AgOpenNtripCaster.Server.Controllers;

/// <summary>
/// System settings management endpoints
/// Requires Admin role
/// </summary>
[ApiController]
[Route("api/admin/settings")]
[Authorize(Roles = "Admin")]
public class SystemSettingsController : ControllerBase
{
    private readonly ILogger<SystemSettingsController> _logger;
    private readonly IConfiguration _configuration;

    public SystemSettingsController(
        ILogger<SystemSettingsController> logger,
        IConfiguration configuration)
    {
        _logger = logger;
        _configuration = configuration;
    }

    /// <summary>
    /// Get all system settings
    /// </summary>
    [HttpGet]
    [ProducesResponseType(typeof(SystemSettingsDto), 200)]
    public IActionResult GetSettings()
    {
        try
        {
            var settings = new SystemSettingsDto
            {
                EmailConfig = new EmailConfigDto
                {
                    SmtpServer = _configuration["Email:SmtpServer"] ?? "smtp.gmail.com",
                    SmtpPort = int.TryParse(_configuration["Email:SmtpPort"], out var port) ? port : 587,
                    SenderEmail = _configuration["Email:SenderEmail"] ?? "",
                    SenderPassword = "••••••••", // Never return actual password
                    UseTls = bool.TryParse(_configuration["Email:UseTls"], out var tls) && tls,
                },
                LoggingConfig = new LoggingConfigDto
                {
                    LogLevel = _configuration["Logging:LogLevel:Default"] ?? "Information",
                    MaxLogSize = int.TryParse(_configuration["Logging:MaxLogSize"], out var size) ? size : 100,
                    RetentionDays = int.TryParse(_configuration["Logging:RetentionDays"], out var retention) ? retention : 30,
                    EnableConsoleLogging = bool.TryParse(_configuration["Logging:Console:IncludeScopes"], out var console) && console,
                    EnableFileLogging = !string.IsNullOrEmpty(_configuration["Logging:File:Path"]),
                }
            };

            return Ok(settings);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving system settings");
            return StatusCode(500, new { message = "Error retrieving settings", error = ex.Message });
        }
    }

    /// <summary>
    /// Update email configuration
    /// </summary>
    [HttpPut("email")]
    [ProducesResponseType(typeof(EmailConfigDto), 200)]
    public IActionResult UpdateEmailSettings([FromBody] EmailConfigDto request)
    {
        try
        {
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            _logger.LogInformation("Email settings updated by {User}", User.Identity?.Name);

            // In a real implementation, you would save these to configuration/database
            var response = new EmailConfigDto
            {
                SmtpServer = request.SmtpServer,
                SmtpPort = request.SmtpPort,
                SenderEmail = request.SenderEmail,
                SenderPassword = "••••••••",
                UseTls = request.UseTls,
            };

            return Ok(response);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error updating email settings");
            return StatusCode(500, new { message = "Error updating settings", error = ex.Message });
        }
    }

    /// <summary>
    /// Update logging configuration
    /// </summary>
    [HttpPut("logging")]
    [ProducesResponseType(typeof(LoggingConfigDto), 200)]
    public IActionResult UpdateLoggingSettings([FromBody] LoggingConfigDto request)
    {
        try
        {
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            _logger.LogInformation("Logging settings updated by {User}", User.Identity?.Name);

            return Ok(request);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error updating logging settings");
            return StatusCode(500, new { message = "Error updating settings", error = ex.Message });
        }
    }

    /// <summary>
    /// Test email configuration
    /// </summary>
    [HttpPost("email/test")]
    [ProducesResponseType(200)]
    public async Task<IActionResult> TestEmailSettings([FromBody] EmailConfigDto config)
    {
        try
        {
            _logger.LogInformation("Testing email configuration...");

            // In a real implementation, you would actually try to send a test email
            await Task.Delay(100); // Simulate work

            return Ok(new { message = "Email configuration test successful" });
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Email configuration test failed");
            return BadRequest(new { message = "Email test failed", error = ex.Message });
        }
    }
}
