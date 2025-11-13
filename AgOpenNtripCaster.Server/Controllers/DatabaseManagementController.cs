using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using AgOpenNtripCaster.Server.Models.DTOs;
using AgOpenNtripCaster.Server.Services.Data;

namespace AgOpenNtripCaster.Server.Controllers;

/// <summary>
/// Database management and maintenance endpoints
/// Requires Admin role
/// </summary>
[ApiController]
[Route("api/admin/database")]
[Authorize(Roles = "Admin")]
public class DatabaseManagementController : ControllerBase
{
    private readonly IDatabaseManagementService _databaseService;
    private readonly ILogger<DatabaseManagementController> _logger;

    public DatabaseManagementController(
        IDatabaseManagementService databaseService,
        ILogger<DatabaseManagementController> logger)
    {
        _databaseService = databaseService;
        _logger = logger;
    }

    /// <summary>
    /// Get database statistics
    /// </summary>
    [HttpGet("statistics")]
    [ProducesResponseType(typeof(DatabaseStatsDto), 200)]
    public async Task<IActionResult> GetDatabaseStatistics()
    {
        try
        {
            var stats = await _databaseService.GetDatabaseStatisticsAsync();
            return Ok(stats);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving database statistics");
            return StatusCode(500, new { message = "Error retrieving statistics", error = ex.Message });
        }
    }

    /// <summary>
    /// Get information about all tables
    /// </summary>
    [HttpGet("tables")]
    [ProducesResponseType(typeof(List<TableInfoDto>), 200)]
    public async Task<IActionResult> GetTableInformation()
    {
        try
        {
            var tables = await _databaseService.GetTableInformationAsync();
            return Ok(tables);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving table information");
            return StatusCode(500, new { message = "Error retrieving table info", error = ex.Message });
        }
    }

    /// <summary>
    /// Create database backup
    /// </summary>
    [HttpPost("backup")]
    [ProducesResponseType(typeof(MaintenanceResultDto), 200)]
    public async Task<IActionResult> BackupDatabase()
    {
        try
        {
            _logger.LogWarning("Database backup initiated by {User}", User.Identity?.Name);
            var result = await _databaseService.BackupDatabaseAsync();
            return Ok(result);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error backing up database");
            return StatusCode(500, new { success = false, message = "Error creating backup", error = ex.Message });
        }
    }

    /// <summary>
    /// Optimize database tables
    /// </summary>
    [HttpPost("optimize")]
    [ProducesResponseType(typeof(MaintenanceResultDto), 200)]
    public async Task<IActionResult> OptimizeTables()
    {
        try
        {
            _logger.LogWarning("Database optimization initiated by {User}", User.Identity?.Name);
            var result = await _databaseService.OptimizeTablesAsync();
            return Ok(result);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error optimizing database");
            return StatusCode(500, new { success = false, message = "Error optimizing database", error = ex.Message });
        }
    }

    /// <summary>
    /// Vacuum database to reclaim space
    /// </summary>
    [HttpPost("vacuum")]
    [ProducesResponseType(typeof(MaintenanceResultDto), 200)]
    public async Task<IActionResult> VacuumDatabase()
    {
        try
        {
            _logger.LogWarning("Database vacuum initiated by {User}", User.Identity?.Name);
            var result = await _databaseService.VacuumDatabaseAsync();
            return Ok(result);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error vacuuming database");
            return StatusCode(500, new { success = false, message = "Error vacuuming database", error = ex.Message });
        }
    }

    /// <summary>
    /// Reindex all database indexes
    /// </summary>
    [HttpPost("reindex")]
    [ProducesResponseType(typeof(MaintenanceResultDto), 200)]
    public async Task<IActionResult> ReindexDatabase()
    {
        try
        {
            _logger.LogWarning("Database reindex initiated by {User}", User.Identity?.Name);
            var result = await _databaseService.ReindexDatabaseAsync();
            return Ok(result);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error reindexing database");
            return StatusCode(500, new { success = false, message = "Error reindexing database", error = ex.Message });
        }
    }

    /// <summary>
    /// Rebuild database statistics
    /// </summary>
    [HttpPost("rebuild-statistics")]
    [ProducesResponseType(typeof(MaintenanceResultDto), 200)]
    public async Task<IActionResult> RebuildStatistics()
    {
        try
        {
            _logger.LogWarning("Database statistics rebuild initiated by {User}", User.Identity?.Name);
            var result = await _databaseService.RebuildStatisticsAsync();
            return Ok(result);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error rebuilding statistics");
            return StatusCode(500, new { success = false, message = "Error rebuilding statistics", error = ex.Message });
        }
    }

    /// <summary>
    /// Remove old activity logs
    /// </summary>
    [HttpPost("cleanup-old-logs")]
    [ProducesResponseType(typeof(MaintenanceResultDto), 200)]
    public async Task<IActionResult> CleanupOldLogs([FromQuery] int olderThanDays = 30)
    {
        try
        {
            _logger.LogWarning("Database cleanup initiated by {User}. Removing logs older than {Days} days", User.Identity?.Name, olderThanDays);
            var result = await _databaseService.CleanupOldLogsAsync(olderThanDays);
            return Ok(result);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error cleaning up old logs");
            return StatusCode(500, new { success = false, message = "Error cleaning up logs", error = ex.Message });
        }
    }

    /// <summary>
    /// Remove orphaned records
    /// </summary>
    [HttpPost("cleanup-orphaned")]
    [ProducesResponseType(typeof(MaintenanceResultDto), 200)]
    public async Task<IActionResult> CleanupOrphanedRecords()
    {
        try
        {
            _logger.LogWarning("Orphaned records cleanup initiated by {User}", User.Identity?.Name);
            var result = await _databaseService.CleanupOrphanedRecordsAsync();
            return Ok(result);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error cleaning up orphaned records");
            return StatusCode(500, new { success = false, message = "Error cleaning up records", error = ex.Message });
        }
    }

    /// <summary>
    /// Repair database integrity (PostgreSQL equivalent: VACUUM FULL ANALYZE)
    /// </summary>
    [HttpPost("repair")]
    [ProducesResponseType(typeof(MaintenanceResultDto), 200)]
    public async Task<IActionResult> RepairDatabase()
    {
        try
        {
            _logger.LogWarning("Database repair initiated by {User}", User.Identity?.Name);
            // PostgreSQL doesn't have a "repair" command like MySQL
            // We'll use VACUUM FULL which reclaims space and reorganizes
            var result = await _databaseService.VacuumDatabaseAsync();
            return Ok(result);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error repairing database");
            return StatusCode(500, new { success = false, message = "Error repairing database", error = ex.Message });
        }
    }
}
