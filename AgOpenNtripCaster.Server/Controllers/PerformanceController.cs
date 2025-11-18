using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using AgOpenNtripCaster.Server.Models.DTOs;
using AgOpenNtripCaster.Server.Services.Performance;

namespace AgOpenNtripCaster.Server.Controllers;

[ApiController]
[Route("api/[controller]")]
[Authorize(Roles = "Admin")] // Only admins can view performance metrics
public class PerformanceController : ControllerBase
{
    private readonly PerformanceMetricsService _metricsService;
    private readonly ILogger<PerformanceController> _logger;

    public PerformanceController(
        PerformanceMetricsService metricsService,
        ILogger<PerformanceController> logger)
    {
        _metricsService = metricsService;
        _logger = logger;
    }

    /// <summary>
    /// Get current performance metrics (real-time)
    /// </summary>
    [HttpGet("current")]
    [ProducesResponseType(typeof(PerformanceMetricsResponse), 200)]
    public ActionResult<PerformanceMetricsResponse> GetCurrentMetrics()
    {
        try
        {
            var snapshot = _metricsService.GetCurrentMetrics();

            var response = new PerformanceMetricsResponse
            {
                Timestamp = snapshot.Timestamp,
                TotalBytesSent = snapshot.TotalBytesSent,
                MemorySavedBytes = snapshot.MemorySavedBytes,
                ActiveZeroCopyBuffers = snapshot.ActiveZeroCopyBuffers,
                PeakZeroCopyBuffers = snapshot.PeakZeroCopyBuffers,
                AverageBroadcastTimeMs = snapshot.AverageBroadcastTimeMs,
                PeakBroadcastTimeMs = snapshot.PeakBroadcastTimeMs,
                TotalBroadcasts = snapshot.TotalBroadcasts,
                ActiveClients = snapshot.ActiveClients,
                ActiveSources = snapshot.ActiveSources,
                TotalMountPoints = 0, // TODO: Get from mount point service
                TotalMemoryUsageBytes = snapshot.TotalMemoryUsageBytes,
                GcCollectionCount = snapshot.GcCollectionCount
            };

            return Ok(response);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to get performance metrics");
            return StatusCode(500, new { message = "Failed to retrieve performance metrics" });
        }
    }
}
