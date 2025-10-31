using AgOpenNtripCaster.Server.Services.NTRIP;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

namespace AgOpenNtripCaster.Server.Controllers;

/// <summary>
/// Activity log endpoints for retrieving connection/disconnection events
/// </summary>
[ApiController]
[Route("api/activity")]
[Authorize]
public class ActivityController : ControllerBase
{
    private readonly IActivityService _activityService;
    private readonly ILogger<ActivityController> _logger;

    public ActivityController(
        IActivityService activityService,
        ILogger<ActivityController> logger)
    {
        _activityService = activityService;
        _logger = logger;
    }

    /// <summary>
    /// Get recent activities (all users for admin, own activities for regular users)
    /// </summary>
    [HttpGet("recent")]
    public async Task<ActionResult<List<ActivityDto>>> GetRecentActivities(int limit = 50)
    {
        try
        {
            if (limit < 1) limit = 1;
            if (limit > 500) limit = 500;

            var activities = await _activityService.GetRecentActivitiesAsync(limit);
            return Ok(activities);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving recent activities");
            return StatusCode(500, new { message = "Error retrieving activities" });
        }
    }

    /// <summary>
    /// Get activities for a specific mount point (requires access)
    /// </summary>
    [HttpGet("mount-point/{mountPointId}")]
    public async Task<ActionResult<List<ActivityDto>>> GetMountPointActivities(int mountPointId, int limit = 20)
    {
        try
        {
            if (limit < 1) limit = 1;
            if (limit > 500) limit = 500;

            var activities = await _activityService.GetActivitiesByMountPointAsync(mountPointId, limit);
            return Ok(activities);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving mount point activities for {MountPointId}", mountPointId);
            return StatusCode(500, new { message = "Error retrieving activities" });
        }
    }

    /// <summary>
    /// Get activities for the current user
    /// </summary>
    [HttpGet("my-activities")]
    public async Task<ActionResult<List<ActivityDto>>> GetMyActivities(int limit = 20)
    {
        try
        {
            if (limit < 1) limit = 1;
            if (limit > 500) limit = 500;

            var userId = User.FindFirst("sub")?.Value ?? User.FindFirst("http://schemas.xmlsoap.org/ws/2005/05/identity/claims/nameidentifier")?.Value;

            if (string.IsNullOrEmpty(userId))
            {
                return Unauthorized(new { message = "User ID not found in token" });
            }

            var activities = await _activityService.GetActivitiesByUserAsync(userId, limit);
            return Ok(activities);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving user activities");
            return StatusCode(500, new { message = "Error retrieving activities" });
        }
    }
}
