using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.DTOs;
using AgOpenNtripCaster.Server.Services.Configuration;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;

namespace AgOpenNtripCaster.Server.Controllers;

/// <summary>
/// Admin configuration endpoints for CAS/NET sourcetable information
/// Requires Admin role
/// </summary>
[ApiController]
[Route("api/admin/config")]
[Authorize(Roles = "Admin")]
public class AdminConfigController : ControllerBase
{
    private readonly ICasterInfoService _casterInfoService;
    private readonly INetworkInfoService _networkInfoService;
    private readonly ApplicationDbContext _dbContext;
    private readonly ILogger<AdminConfigController> _logger;
    private static DateTime _serverStartTime = DateTime.UtcNow;

    public AdminConfigController(
        ICasterInfoService casterInfoService,
        INetworkInfoService networkInfoService,
        ApplicationDbContext dbContext,
        ILogger<AdminConfigController> logger)
    {
        _casterInfoService = casterInfoService;
        _networkInfoService = networkInfoService;
        _dbContext = dbContext;
        _logger = logger;
    }

    /// <summary>
    /// Get current caster configuration
    /// </summary>
    [HttpGet("caster")]
    public async Task<ActionResult<CasterInfoDto>> GetCasterConfig()
    {
        try
        {
            var casterInfo = await _casterInfoService.GetCasterInfoAsync();

            if (casterInfo == null)
            {
                _logger.LogWarning("No caster configuration found");
                return NotFound(new { message = "Caster configuration not found. Create one first." });
            }

            return Ok(casterInfo);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error getting caster configuration");
            return StatusCode(500, new { message = "Error retrieving caster configuration", error = ex.Message });
        }
    }

    /// <summary>
    /// Update caster configuration (create if not exists)
    /// </summary>
    [HttpPut("caster")]
    public async Task<ActionResult<CasterInfoDto>> UpdateCasterConfig([FromBody] UpdateCasterInfoRequest request)
    {
        try
        {
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            var result = await _casterInfoService.UpdateCasterInfoAsync(request);

            _logger.LogInformation("Caster configuration updated by {User}", User.Identity?.Name);

            return Ok(result);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error updating caster configuration");
            return StatusCode(500, new { message = "Error updating caster configuration", error = ex.Message });
        }
    }

    /// <summary>
    /// Get current network configuration
    /// </summary>
    [HttpGet("network")]
    public async Task<ActionResult<NetworkInfoDto>> GetNetworkConfig()
    {
        try
        {
            var networkInfo = await _networkInfoService.GetNetworkInfoAsync();

            if (networkInfo == null)
            {
                _logger.LogWarning("No network configuration found");
                return NotFound(new { message = "Network configuration not found. Create one first." });
            }

            return Ok(networkInfo);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error getting network configuration");
            return StatusCode(500, new { message = "Error retrieving network configuration", error = ex.Message });
        }
    }

    /// <summary>
    /// Update network configuration (create if not exists)
    /// </summary>
    [HttpPut("network")]
    public async Task<ActionResult<NetworkInfoDto>> UpdateNetworkConfig([FromBody] UpdateNetworkInfoRequest request)
    {
        try
        {
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            var result = await _networkInfoService.UpdateNetworkInfoAsync(request);

            _logger.LogInformation("Network configuration updated by {User}", User.Identity?.Name);

            return Ok(result);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error updating network configuration");
            return StatusCode(500, new { message = "Error updating network configuration", error = ex.Message });
        }
    }

    /// <summary>
    /// Get both caster and network config in one call (useful for frontend)
    /// </summary>
    [HttpGet("all")]
    public async Task<ActionResult<dynamic>> GetAllConfig()
    {
        try
        {
            var casterInfo = await _casterInfoService.GetCasterInfoAsync();
            var networkInfo = await _networkInfoService.GetNetworkInfoAsync();

            return Ok(new
            {
                caster = casterInfo,
                network = networkInfo
            });
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error getting all configurations");
            return StatusCode(500, new { message = "Error retrieving configurations", error = ex.Message });
        }
    }

    /// <summary>
    /// Get real-time dashboard statistics
    /// Includes active clients/sources count and data transfer statistics
    /// </summary>
    [HttpGet("stats")]
    public async Task<ActionResult<DashboardStatsDto>> GetDashboardStats()
    {
        try
        {
            // Get active client sessions (not disconnected)
            var activeClients = await _dbContext.ClientSessions
                .Where(cs => cs.DisconnectedAt == null)
                .ToListAsync();

            // Get active mount points
            var activeMountPoints = await _dbContext.MountPoints
                .Where(mp => mp.IsActive)
                .ToListAsync();

            // Calculate total bytes from all sessions
            var totalBytesReceived = activeClients.Sum(cs => cs.BytesReceived);
            var totalBytesSent = activeClients.Sum(cs => cs.BytesSent);

            // Convert to MB (1 MB = 1,048,576 bytes)
            const long bytesPerMB = 1048576;
            var receivedMB = totalBytesReceived / (double)bytesPerMB;
            var sentMB = totalBytesSent / (double)bytesPerMB;

            // Calculate uptime
            var now = DateTime.UtcNow;
            var uptime = now - _serverStartTime;
            var uptimeFormatted = FormatUptime(uptime);

            var stats = new DashboardStatsDto
            {
                ActiveClients = activeClients.Count,
                ActiveSources = activeMountPoints.Count(mp => mp.IsActive),
                TotalBytesReceived = receivedMB,
                TotalBytesSent = sentMB,
                TotalBytesTransferred = receivedMB + sentMB,
                ServerStartTime = _serverStartTime,
                CurrentTime = now,
                UptimeFormatted = uptimeFormatted
            };

            return Ok(stats);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving dashboard statistics");
            return StatusCode(500, new { message = "Error retrieving statistics", error = ex.Message });
        }
    }

    /// <summary>
    /// Format TimeSpan to human-readable uptime string
    /// </summary>
    private static string FormatUptime(TimeSpan uptime)
    {
        var days = uptime.Days;
        var hours = uptime.Hours;
        var minutes = uptime.Minutes;

        if (days > 0)
        {
            return $"{days}d {hours}h {minutes}m";
        }
        else if (hours > 0)
        {
            return $"{hours}h {minutes}m";
        }
        else
        {
            return $"{minutes}m";
        }
    }
}
