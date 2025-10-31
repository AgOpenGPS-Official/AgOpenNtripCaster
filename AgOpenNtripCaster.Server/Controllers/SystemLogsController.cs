using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using AgOpenNtripCaster.Server.Models.DTOs;

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

    public SystemLogsController(ILogger<SystemLogsController> logger)
    {
        _logger = logger;
    }

    /// <summary>
    /// Get system logs with optional filtering
    /// </summary>
    [HttpGet]
    [ProducesResponseType(typeof(List<SystemLogDto>), 200)]
    public IActionResult GetLogs(
        [FromQuery] string? level = null,
        [FromQuery] int limit = 100,
        [FromQuery] int offset = 0)
    {
        try
        {
            // Mock data - in real implementation, read from log files or database
            var allLogs = new List<SystemLogDto>
            {
                new SystemLogDto { Id = 1, Timestamp = DateTime.UtcNow, Level = "INFO", Message = "Server started successfully" },
                new SystemLogDto { Id = 2, Timestamp = DateTime.UtcNow.AddMinutes(-1), Level = "INFO", Message = "Client connected: user@example.com" },
                new SystemLogDto { Id = 3, Timestamp = DateTime.UtcNow.AddMinutes(-2), Level = "WARNING", Message = "High memory usage detected: 85%" },
                new SystemLogDto { Id = 4, Timestamp = DateTime.UtcNow.AddMinutes(-3), Level = "ERROR", Message = "Database connection timeout" },
                new SystemLogDto { Id = 5, Timestamp = DateTime.UtcNow.AddMinutes(-4), Level = "INFO", Message = "Mount point created: TEST_MP" },
                new SystemLogDto { Id = 6, Timestamp = DateTime.UtcNow.AddMinutes(-5), Level = "DEBUG", Message = "Processing RTCM message from source" },
                new SystemLogDto { Id = 7, Timestamp = DateTime.UtcNow.AddMinutes(-6), Level = "INFO", Message = "Client disconnected: user@example.com" },
                new SystemLogDto { Id = 8, Timestamp = DateTime.UtcNow.AddMinutes(-7), Level = "WARNING", Message = "Slow database query detected: 2500ms" },
            };

            // Filter by level
            var filtered = allLogs;
            if (!string.IsNullOrEmpty(level) && level != "all")
            {
                filtered = allLogs.Where(l => l.Level == level).ToList();
            }

            // Apply pagination
            var paginated = filtered
                .OrderByDescending(l => l.Timestamp)
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
    public IActionResult GetLogStatistics()
    {
        try
        {
            var stats = new
            {
                totalLogs = 15234,
                errors = 234,
                warnings = 567,
                infos = 12340,
                debugs = 2093,
                averagePerHour = 634,
                lastLogTime = DateTime.UtcNow,
                logFileSize = "45.2 MB"
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

            // In real implementation, delete old log files or database records
            await Task.Delay(100);

            return Ok(new { message = $"Cleared logs older than {olderThanDays} days", deletedCount = 1245 });
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

            // In real implementation, generate and return log file
            var logsContent = "Log entries would be here...\n";
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

            // In real implementation, generate CSV file
            var csvContent = "Timestamp,Level,Message\n";
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
