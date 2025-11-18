using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.DTOs;
using AgOpenNtripCaster.Server.Models.Entities;
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
    [Authorize(Roles = "Admin,ReadOnly")] // ReadOnly users can view
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
    [Authorize(Roles = "Admin,ReadOnly")] // ReadOnly users can view
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
    [Authorize(Roles = "Admin,ReadOnly")] // ReadOnly users can view
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
    /// Clean up orphaned client sessions (mark as disconnected)
    /// </summary>
    [HttpPost("cleanup-sessions")]
    public async Task<ActionResult<object>> CleanupOrphanedSessions()
    {
        try
        {
            // Find all active sessions (DisconnectedAt == null)
            var activeSessions = await _dbContext.ClientSessions
                .Where(cs => cs.DisconnectedAt == null)
                .ToListAsync();

            _logger.LogWarning("🧹 Cleaning up {Count} orphaned ClientSessions", activeSessions.Count);

            // Mark all as disconnected
            var now = DateTime.UtcNow;
            foreach (var session in activeSessions)
            {
                session.DisconnectedAt = now;
                session.Status = ClientStreamStatus.Disconnected;
                _logger.LogWarning("  - Marked disconnected: {Id} ({Username})", session.Id, session.UserId);
            }

            await _dbContext.SaveChangesAsync();

            _logger.LogWarning("🧹 Cleanup complete! Disconnected {Count} sessions", activeSessions.Count);

            return Ok(new {
                message = $"Cleaned up {activeSessions.Count} orphaned sessions",
                cleanedCount = activeSessions.Count
            });
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error cleaning up sessions");
            return BadRequest(new { error = ex.Message });
        }
    }

    /// <summary>
    /// Get real-time dashboard statistics
    /// Includes active clients/sources count and data transfer statistics
    /// </summary>
    [HttpGet("stats")]
    [Authorize(Roles = "Admin,ReadOnly")] // ReadOnly users can view
    public async Task<ActionResult<DashboardStatsDto>> GetDashboardStats()
    {
        try
        {
            // Get active client sessions (not disconnected)
            var activeClients = await _dbContext.ClientSessions
                .Where(cs => cs.DisconnectedAt == null)
                .ToListAsync();

            _logger.LogError("🔥 STATS: ActiveClients count = {Count}", activeClients.Count);
            foreach (var client in activeClients)
            {
                _logger.LogError("  - {Id}: {Status}, DisconnectedAt={DisconnectedAt}",
                    client.Id, client.Status, client.DisconnectedAt);
            }

            // Count unique mount points with active connections (not disconnected)
            // Use DistinctBy to avoid counting multiple connections for same mount point
            var activeSources = await _dbContext.SourceConnections
                .Where(sc => sc.DisconnectedAt == null)
                .Include(sc => sc.MountPoint)
                .ToListAsync();

            // Get count of unique mount points
            var uniqueActiveMountPoints = activeSources.DistinctBy(sc => sc.MountPointId).Count();

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
                ActiveSources = uniqueActiveMountPoints,
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
