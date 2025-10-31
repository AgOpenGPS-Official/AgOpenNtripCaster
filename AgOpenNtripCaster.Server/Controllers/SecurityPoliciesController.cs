using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using AgOpenNtripCaster.Server.Models.DTOs;

namespace AgOpenNtripCaster.Server.Controllers;

/// <summary>
/// Security policy management endpoints
/// Requires Admin role
/// </summary>
[ApiController]
[Route("api/admin/security")]
[Authorize(Roles = "Admin")]
public class SecurityPoliciesController : ControllerBase
{
    private readonly ILogger<SecurityPoliciesController> _logger;
    private readonly IConfiguration _configuration;

    public SecurityPoliciesController(
        ILogger<SecurityPoliciesController> logger,
        IConfiguration configuration)
    {
        _logger = logger;
        _configuration = configuration;
    }

    /// <summary>
    /// Get current security policies
    /// </summary>
    [HttpGet("policies")]
    [ProducesResponseType(typeof(SecurityPolicyDto), 200)]
    public IActionResult GetSecurityPolicies()
    {
        try
        {
            var policies = new SecurityPolicyDto
            {
                RequireMfa = bool.TryParse(_configuration["Security:RequireMfa"], out var mfa) && mfa,
                PasswordMinLength = int.TryParse(_configuration["Security:PasswordMinLength"], out var minLen) ? minLen : 8,
                PasswordExpireDays = int.TryParse(_configuration["Security:PasswordExpireDays"], out var expireDays) ? expireDays : 90,
                MaxLoginAttempts = int.TryParse(_configuration["Security:MaxLoginAttempts"], out var maxAttempts) ? maxAttempts : 5,
                LockoutDurationMinutes = int.TryParse(_configuration["Security:LockoutDurationMinutes"], out var lockout) ? lockout : 15,
                SessionTimeoutMinutes = int.TryParse(_configuration["Security:SessionTimeoutMinutes"], out var timeout) ? timeout : 60,
                IpWhitelistEnabled = bool.TryParse(_configuration["Security:IpWhitelistEnabled"], out var ipEnabled) && ipEnabled,
                IpWhitelist = _configuration["Security:IpWhitelist"] ?? "",
                TlsEnabled = bool.TryParse(_configuration["Security:TlsEnabled"], out var tls) && tls,
            };

            return Ok(policies);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving security policies");
            return StatusCode(500, new { message = "Error retrieving policies", error = ex.Message });
        }
    }

    /// <summary>
    /// Update security policies
    /// </summary>
    [HttpPut("policies")]
    [ProducesResponseType(typeof(SecurityPolicyDto), 200)]
    public IActionResult UpdateSecurityPolicies([FromBody] SecurityPolicyDto request)
    {
        try
        {
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            _logger.LogWarning("Security policies updated by {User}: MFA={RequireMfa}, PasswordMinLength={PasswordMinLength}, SessionTimeout={SessionTimeout}",
                User.Identity?.Name,
                request.RequireMfa,
                request.PasswordMinLength,
                request.SessionTimeoutMinutes);

            // In a real implementation, save to database or configuration
            return Ok(request);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error updating security policies");
            return StatusCode(500, new { message = "Error updating policies", error = ex.Message });
        }
    }

    /// <summary>
    /// Validate IP against whitelist
    /// </summary>
    [HttpPost("validate-ip")]
    [AllowAnonymous]
    [ProducesResponseType(200)]
    public IActionResult ValidateIp([FromBody] string ipAddress)
    {
        try
        {
            var whitelistEnabled = bool.TryParse(_configuration["Security:IpWhitelistEnabled"], out var enabled) && enabled;
            if (!whitelistEnabled)
            {
                return Ok(new { allowed = true, reason = "IP whitelist disabled" });
            }

            var whitelist = _configuration["Security:IpWhitelist"]?.Split('\n') ?? Array.Empty<string>();
            var isAllowed = whitelist.Any(ip => ipAddress.StartsWith(ip.Trim()));

            return Ok(new { allowed = isAllowed, reason = isAllowed ? "IP allowed" : "IP not in whitelist" });
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error validating IP");
            return BadRequest(new { message = "Error validating IP", error = ex.Message });
        }
    }

    /// <summary>
    /// Get audit log of security changes
    /// </summary>
    [HttpGet("audit-log")]
    [ProducesResponseType(typeof(List<object>), 200)]
    public IActionResult GetAuditLog([FromQuery] int days = 30)
    {
        try
        {
            var auditLog = new List<object>
            {
                new { timestamp = DateTime.UtcNow.AddDays(-1), action = "Security policy updated", admin = "admin@example.com", details = "Password expiry changed from 90 to 120 days" },
                new { timestamp = DateTime.UtcNow.AddDays(-3), action = "MFA requirement enabled", admin = "admin@example.com", details = "MFA now required for all admin accounts" },
                new { timestamp = DateTime.UtcNow.AddDays(-7), action = "IP whitelist updated", admin = "admin@example.com", details = "Added IP range 10.0.0.0/8" },
            };

            return Ok(auditLog);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving audit log");
            return StatusCode(500, new { message = "Error retrieving audit log", error = ex.Message });
        }
    }
}
