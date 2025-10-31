using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using AgOpenNtripCaster.Server.Models.DTOs;
using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.Entities;
using Microsoft.EntityFrameworkCore;

namespace AgOpenNtripCaster.Server.Controllers;

/// <summary>
/// System logs retrieval and management endpoints
/// Requires Admin role
/// </summary>
[ApiController]
[Route("api/admin/logs")]
[Authorize(Roles = "Admin")]
public class SystemLogsController : ControllerBase
{
    private readonly ILogger<SystemLogsController> _logger;
    private readonly ApplicationDbContext _dbContext;

    public SystemLogsController(ILogger<SystemLogsController> logger, ApplicationDbContext dbContext)
    {
        _logger = logger;
        _dbContext = dbContext;
    }

    /// <summary>
    /// Get system logs with optional filtering
    /// </summary>
    [HttpGet]
    [ProducesResponseType(typeof(object), 200)]
    public async Task<IActionResult> GetLogs(
        [FromQuery] string? level = null,
        [FromQuery] int limit = 100,
        [FromQuery] int offset = 0)
    {
        try
        {
            // Get activities from database and map to system logs
            var allActivities = await _dbContext.Activities
                .OrderByDescending(a => a.CreatedAt)
                .ToListAsync();

            // Map activities to system logs
            var allLogs = allActivities.Select((activity, index) => new SystemLogDto
            {
                Id = activity.Id,
                Timestamp = activity.CreatedAt,
                Level = activity.LogLevel,
                Message = activity.Description ?? $"{activity.Type} event occurred"
            }).ToList();

            // Filter by level
            var filtered = allLogs;
            if (!string.IsNullOrEmpty(level) && level != "all")
            {
                filtered = allLogs.Where(l => l.Level.Equals(level, StringComparison.OrdinalIgnoreCase)).ToList();
            }

            // Apply pagination
            var paginated = filtered
                .Skip(offset)
                .Take(limit)
                .ToList();

            return Ok(new
            {
                total = filtered.Count,
                count = paginated.Count,
                logs = paginated
            });
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving logs");
            return StatusCode(500, new { message = "Error retrieving logs", error = ex.Message });
        }
    }

    /// <summary>
    /// Get log statistics
    /// </summary>
    [HttpGet("statistics")]
    [ProducesResponseType(typeof(object), 200)]
    public async Task<IActionResult> GetLogStatistics()
    {
        try
        {
            // Get activities from database
            var activities = await _dbContext.Activities.ToListAsync();

            // Calculate statistics
            var totalLogs = activities.Count;

            // Count by log level
            var errors = activities.Count(a => a.LogLevel == "ERROR");
            var warnings = activities.Count(a => a.LogLevel == "WARNING");
            var infos = activities.Count(a => a.LogLevel == "INFO");
            var debugs = activities.Count(a => a.LogLevel == "DEBUG");

            // Get the latest activity
            var lastActivity = activities.OrderByDescending(a => a.CreatedAt).FirstOrDefault();
            var lastLogTime = lastActivity?.CreatedAt ?? DateTime.UtcNow;

            // Calculate average per hour
            var now = DateTime.UtcNow;
            var oneHourAgo = now.AddHours(-1);
            var logsInLastHour = activities.Count(a => a.CreatedAt >= oneHourAgo);

            var stats = new
            {
                totalLogs,
                errors,
                warnings,
                infos,
                debugs,
                averagePerHour = logsInLastHour,
                lastLogTime,
                logFileSize = "N/A"
            };

            return Ok(stats);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving log statistics");
            return StatusCode(500, new { message = "Error retrieving statistics", error = ex.Message });
        }
    }

    /// <summary>
    /// Clear logs older than specified days
    /// </summary>
    [HttpPost("clear-old")]
    [ProducesResponseType(200)]
    public async Task<IActionResult> ClearOldLogs([FromQuery] int olderThanDays = 30)
    {
        try
        {
            _logger.LogWarning("Clearing logs older than {Days} days by {User}", olderThanDays, User.Identity?.Name);

            // Delete activities older than specified days
            var cutoffDate = DateTime.UtcNow.AddDays(-olderThanDays);
            var activitiesToDelete = await _dbContext.Activities
                .Where(a => a.CreatedAt < cutoffDate)
                .ToListAsync();

            var deletedCount = activitiesToDelete.Count;

            if (deletedCount > 0)
            {
                _dbContext.Activities.RemoveRange(activitiesToDelete);
                await _dbContext.SaveChangesAsync();
            }

            return Ok(new { message = $"Cleared logs older than {olderThanDays} days", deletedCount });
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error clearing old logs");
            return StatusCode(500, new { message = "Error clearing logs", error = ex.Message });
        }
    }

    /// <summary>
    /// Download logs as file
    /// </summary>
    [HttpGet("download")]
    [ProducesResponseType(typeof(FileResult), 200)]
    public async Task<IActionResult> DownloadLogs([FromQuery] string? level = null)
    {
        try
        {
            _logger.LogInformation("Downloading logs by {User}", User.Identity?.Name);

            // Get activities from database
            var activities = await _dbContext.Activities
                .OrderByDescending(a => a.CreatedAt)
                .ToListAsync();

            // Generate text content
            var logsContent = "System Activity Log\n";
            logsContent += $"Generated: {DateTime.UtcNow:yyyy-MM-dd HH:mm:ss}\n";
            logsContent += "=".PadRight(80, '=') + "\n\n";

            foreach (var activity in activities)
            {
                logsContent += $"[{activity.CreatedAt:yyyy-MM-dd HH:mm:ss}] {activity.Type}\n";
                logsContent += $"  {activity.Description}\n\n";
            }

            var bytes = System.Text.Encoding.UTF8.GetBytes(logsContent);

            return File(bytes, "text/plain", $"logs_{DateTime.UtcNow:yyyyMMdd_HHmmss}.txt");
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error downloading logs");
            return StatusCode(500, new { message = "Error downloading logs", error = ex.Message });
        }
    }

    /// <summary>
    /// Export logs as CSV
    /// </summary>
    [HttpGet("export-csv")]
    [ProducesResponseType(typeof(FileResult), 200)]
    public async Task<IActionResult> ExportLogsCsv([FromQuery] int days = 7)
    {
        try
        {
            _logger.LogInformation("Exporting logs as CSV by {User}", User.Identity?.Name);

            // Get activities from last N days
            var cutoffDate = DateTime.UtcNow.AddDays(-days);
            var activities = await _dbContext.Activities
                .Where(a => a.CreatedAt >= cutoffDate)
                .OrderByDescending(a => a.CreatedAt)
                .ToListAsync();

            // Generate CSV content
            var csvContent = "Timestamp,ActivityType,Level,Description\n";

            foreach (var activity in activities)
            {
                var level = "INFO";
                var timestamp = activity.CreatedAt.ToString("yyyy-MM-dd HH:mm:ss");
                var type = activity.Type.ToString();
                var description = (activity.Description ?? "").Replace("\"", "\"\""); // Escape quotes for CSV

                csvContent += $"\"{timestamp}\",\"{type}\",\"{level}\",\"{description}\"\n";
            }

            var bytes = System.Text.Encoding.UTF8.GetBytes(csvContent);

            return File(bytes, "text/csv", $"logs_{DateTime.UtcNow:yyyyMMdd_HHmmss}.csv");
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error exporting logs");
            return StatusCode(500, new { message = "Error exporting logs", error = ex.Message });
        }
    }
}
