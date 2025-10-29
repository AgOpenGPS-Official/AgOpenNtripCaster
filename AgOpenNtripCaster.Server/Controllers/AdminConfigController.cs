using AgOpenNtripCaster.Server.Models.DTOs;
using AgOpenNtripCaster.Server.Services.Configuration;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

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
    private readonly ILogger<AdminConfigController> _logger;

    public AdminConfigController(
        ICasterInfoService casterInfoService,
        INetworkInfoService networkInfoService,
        ILogger<AdminConfigController> logger)
    {
        _casterInfoService = casterInfoService;
        _networkInfoService = networkInfoService;
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
}
