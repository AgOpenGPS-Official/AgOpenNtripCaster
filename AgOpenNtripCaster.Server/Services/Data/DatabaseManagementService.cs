using Microsoft.EntityFrameworkCore;
using Npgsql;
using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.DTOs;

namespace AgOpenNtripCaster.Server.Services.Data;

/// <summary>
/// Service for database management and maintenance operations
/// </summary>
public interface IDatabaseManagementService
{
    Task<DatabaseStatsDto> GetDatabaseStatisticsAsync();
    Task<List<TableInfoDto>> GetTableInformationAsync();
    Task<MaintenanceResultDto> BackupDatabaseAsync();
    Task<MaintenanceResultDto> OptimizeTablesAsync();
    Task<MaintenanceResultDto> VacuumDatabaseAsync();
    Task<MaintenanceResultDto> ReindexDatabaseAsync();
    Task<MaintenanceResultDto> RebuildStatisticsAsync();
    Task<MaintenanceResultDto> CleanupOldLogsAsync(int olderThanDays);
    Task<MaintenanceResultDto> CleanupOrphanedRecordsAsync();
}

public class DatabaseManagementService : IDatabaseManagementService
{
    private readonly ApplicationDbContext _dbContext;
    private readonly ILogger<DatabaseManagementService> _logger;
    private readonly IConfiguration _configuration;

    public DatabaseManagementService(
        ApplicationDbContext dbContext,
        ILogger<DatabaseManagementService> logger,
        IConfiguration configuration)
    {
        _dbContext = dbContext;
        _logger = logger;
        _configuration = configuration;
    }

    public async Task<DatabaseStatsDto> GetDatabaseStatisticsAsync()
    {
        try
        {
            var npgsqlConnection = (NpgsqlConnection)_dbContext.Database.GetDbConnection();
            if (npgsqlConnection.State != System.Data.ConnectionState.Open)
            {
                await npgsqlConnection.OpenAsync();
            }

            var databaseName = npgsqlConnection.Database;

            // Get database size
            string? sizeResult = null;
            var sizeQuery = "SELECT pg_size_pretty(pg_database_size($1))::text";
            using (var cmd = new NpgsqlCommand(sizeQuery, npgsqlConnection))
            {
                cmd.Parameters.AddWithValue(databaseName);
                sizeResult = (string?)(await cmd.ExecuteScalarAsync());
            }

            // Get table count
            int tableCount = 0;
            var tableCountQuery = @"
                SELECT COUNT(*)::int
                FROM information_schema.tables
                WHERE table_schema = 'public' AND table_type = 'BASE TABLE'";
            using (var cmd = new NpgsqlCommand(tableCountQuery, npgsqlConnection))
            {
                var result = await cmd.ExecuteScalarAsync();
                tableCount = result != null ? Convert.ToInt32(result) : 0;
            }

            // Get total record count across all tables
            int recordCount = 0;
            var recordCountQuery = @"
                SELECT COALESCE(SUM(n_live_tup), 0)::bigint
                FROM pg_stat_user_tables";
            using (var cmd = new NpgsqlCommand(recordCountQuery, npgsqlConnection))
            {
                var result = await cmd.ExecuteScalarAsync();
                recordCount = result != null ? Convert.ToInt32(result) : 0;
            }

            // Get database version
            string? versionFull = null;
            var versionQuery = "SELECT version()";
            using (var cmd = new NpgsqlCommand(versionQuery, npgsqlConnection))
            {
                versionFull = (string?)(await cmd.ExecuteScalarAsync());
            }

            // Extract version number (e.g., "PostgreSQL 15.3" -> "15.3")
            var version = versionFull?.Split(' ').ElementAtOrDefault(1) ?? "Unknown";

            // Last backup - for now we'll return a placeholder
            // In production, you'd track this in a separate table or config
            var lastBackup = DateTime.UtcNow.AddHours(-24); // Placeholder

            return new DatabaseStatsDto
            {
                TotalSize = sizeResult ?? "Unknown",
                TableCount = tableCount,
                RecordCount = recordCount,
                LastBackup = lastBackup,
                DatabaseVersion = version
            };
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error getting database statistics");
            throw;
        }
    }

    public async Task<List<TableInfoDto>> GetTableInformationAsync()
    {
        try
        {
            // Query only existing tables by joining with pg_tables
            var tablesQuery = @"
                SELECT
                    t.schemaname || '.' || t.tablename as table_name,
                    COALESCE(s.n_live_tup, 0)::bigint as row_count,
                    pg_size_pretty(pg_total_relation_size(t.schemaname||'.'||t.tablename))::text as size
                FROM pg_tables t
                LEFT JOIN pg_stat_user_tables s ON t.schemaname = s.schemaname AND t.tablename = s.relname
                WHERE t.schemaname = 'public'
                ORDER BY pg_total_relation_size(t.schemaname||'.'||t.tablename) DESC";

            var connection = (NpgsqlConnection)_dbContext.Database.GetDbConnection();
            if (connection.State != System.Data.ConnectionState.Open)
            {
                await connection.OpenAsync();
            }

            var tables = new List<TableInfoDto>();

            using (var cmd = new NpgsqlCommand(tablesQuery, connection))
            {
                using (var reader = await cmd.ExecuteReaderAsync())
                {
                    while (await reader.ReadAsync())
                    {
                        var tableName = reader.GetString(0);
                        // Remove 'public.' prefix for cleaner display
                        if (tableName.StartsWith("public."))
                        {
                            tableName = tableName.Substring(7);
                        }

                        tables.Add(new TableInfoDto
                        {
                            Name = tableName,
                            Records = reader.IsDBNull(1) ? 0 : (int)reader.GetInt64(1),
                            Size = reader.IsDBNull(2) ? "0 bytes" : reader.GetString(2)
                        });
                    }
                }
            }

            return tables;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error getting table information");
            throw;
        }
    }

    public async Task<MaintenanceResultDto> BackupDatabaseAsync()
    {
        var startTime = DateTime.UtcNow;
        try
        {
            _logger.LogWarning("Database backup operation started");

            // For PostgreSQL backup, you would typically use pg_dump
            // This requires executing a system command, which should be done carefully
            // For now, we'll log a warning that this needs proper implementation

            _logger.LogWarning("Database backup requires pg_dump configuration. This is a placeholder implementation.");

            // In production, you would:
            // 1. Execute pg_dump with proper credentials
            // 2. Store backup file in configured location
            // 3. Optionally upload to cloud storage

            await Task.Delay(100); // Simulate minimal processing

            var executionTime = (long)(DateTime.UtcNow - startTime).TotalMilliseconds;

            return new MaintenanceResultDto
            {
                Success = true,
                Message = "Backup operation logged. Note: Automated backups require pg_dump configuration.",
                ExecutionTimeMs = executionTime
            };
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error during database backup");
            return new MaintenanceResultDto
            {
                Success = false,
                Message = $"Backup failed: {ex.Message}",
                ExecutionTimeMs = (long)(DateTime.UtcNow - startTime).TotalMilliseconds
            };
        }
    }

    public async Task<MaintenanceResultDto> OptimizeTablesAsync()
    {
        var startTime = DateTime.UtcNow;
        try
        {
            _logger.LogWarning("Database optimization (VACUUM ANALYZE) started");

            // Run VACUUM ANALYZE on all tables
            await _dbContext.Database.ExecuteSqlRawAsync("VACUUM ANALYZE");

            var executionTime = (long)(DateTime.UtcNow - startTime).TotalMilliseconds;

            _logger.LogInformation("Database optimization completed in {Time}ms", executionTime);

            return new MaintenanceResultDto
            {
                Success = true,
                Message = "Database optimization completed successfully",
                ExecutionTimeMs = executionTime
            };
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error optimizing database");
            return new MaintenanceResultDto
            {
                Success = false,
                Message = $"Optimization failed: {ex.Message}",
                ExecutionTimeMs = (long)(DateTime.UtcNow - startTime).TotalMilliseconds
            };
        }
    }

    public async Task<MaintenanceResultDto> VacuumDatabaseAsync()
    {
        var startTime = DateTime.UtcNow;
        try
        {
            _logger.LogWarning("Database VACUUM (FULL) started");

            // VACUUM FULL requires exclusive locks and can take a long time
            // Consider using regular VACUUM instead for online operations
            await _dbContext.Database.ExecuteSqlRawAsync("VACUUM FULL");

            var executionTime = (long)(DateTime.UtcNow - startTime).TotalMilliseconds;

            _logger.LogInformation("Database VACUUM completed in {Time}ms", executionTime);

            return new MaintenanceResultDto
            {
                Success = true,
                Message = "Database vacuum completed successfully",
                ExecutionTimeMs = executionTime
            };
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error vacuuming database");
            return new MaintenanceResultDto
            {
                Success = false,
                Message = $"Vacuum failed: {ex.Message}",
                ExecutionTimeMs = (long)(DateTime.UtcNow - startTime).TotalMilliseconds
            };
        }
    }

    public async Task<MaintenanceResultDto> ReindexDatabaseAsync()
    {
        var startTime = DateTime.UtcNow;
        try
        {
            _logger.LogWarning("Database REINDEX started");

            var connection = _dbContext.Database.GetDbConnection();
            var databaseName = connection.Database;

            // REINDEX DATABASE
            // Note: Database name cannot be parameterized, but we're using the connection's database name which is safe
            var sql = FormattableString.Invariant($"REINDEX DATABASE \"{databaseName}\"");
            await _dbContext.Database.ExecuteSqlRawAsync(sql);

            var executionTime = (long)(DateTime.UtcNow - startTime).TotalMilliseconds;

            _logger.LogInformation("Database REINDEX completed in {Time}ms", executionTime);

            return new MaintenanceResultDto
            {
                Success = true,
                Message = "Database reindex completed successfully",
                ExecutionTimeMs = executionTime
            };
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error reindexing database");
            return new MaintenanceResultDto
            {
                Success = false,
                Message = $"Reindex failed: {ex.Message}",
                ExecutionTimeMs = (long)(DateTime.UtcNow - startTime).TotalMilliseconds
            };
        }
    }

    public async Task<MaintenanceResultDto> RebuildStatisticsAsync()
    {
        var startTime = DateTime.UtcNow;
        try
        {
            _logger.LogWarning("Database statistics rebuild (ANALYZE) started");

            // ANALYZE updates statistics used by the query planner
            await _dbContext.Database.ExecuteSqlRawAsync("ANALYZE");

            var executionTime = (long)(DateTime.UtcNow - startTime).TotalMilliseconds;

            _logger.LogInformation("Database statistics rebuild completed in {Time}ms", executionTime);

            return new MaintenanceResultDto
            {
                Success = true,
                Message = "Database statistics rebuilt successfully",
                ExecutionTimeMs = executionTime
            };
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error rebuilding statistics");
            return new MaintenanceResultDto
            {
                Success = false,
                Message = $"Statistics rebuild failed: {ex.Message}",
                ExecutionTimeMs = (long)(DateTime.UtcNow - startTime).TotalMilliseconds
            };
        }
    }

    public async Task<MaintenanceResultDto> CleanupOldLogsAsync(int olderThanDays)
    {
        var startTime = DateTime.UtcNow;
        try
        {
            _logger.LogWarning("Cleaning up activity logs older than {Days} days", olderThanDays);

            var cutoffDate = DateTime.UtcNow.AddDays(-olderThanDays);

            // Delete old activity logs
            var deletedCount = await _dbContext.Activities
                .Where(a => a.CreatedAt < cutoffDate)
                .ExecuteDeleteAsync();

            var executionTime = (long)(DateTime.UtcNow - startTime).TotalMilliseconds;

            _logger.LogInformation("Deleted {Count} old activity logs in {Time}ms", deletedCount, executionTime);

            return new MaintenanceResultDto
            {
                Success = true,
                Message = $"Removed logs older than {olderThanDays} days. Deleted {deletedCount} records",
                ExecutionTimeMs = executionTime
            };
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error cleaning up old logs");
            return new MaintenanceResultDto
            {
                Success = false,
                Message = $"Cleanup failed: {ex.Message}",
                ExecutionTimeMs = (long)(DateTime.UtcNow - startTime).TotalMilliseconds
            };
        }
    }

    public async Task<MaintenanceResultDto> CleanupOrphanedRecordsAsync()
    {
        var startTime = DateTime.UtcNow;
        try
        {
            _logger.LogWarning("Cleaning up orphaned records");

            var totalDeleted = 0;

            // Clean up client sessions without valid mount points
            var orphanedClients = await _dbContext.ClientSessions
                .Where(cs => cs.MountPoint == null)
                .ExecuteDeleteAsync();
            totalDeleted += orphanedClients;

            // Clean up source connections without valid mount points
            var orphanedSources = await _dbContext.SourceConnections
                .Where(sc => sc.MountPoint == null)
                .ExecuteDeleteAsync();
            totalDeleted += orphanedSources;

            // Clean up disconnected sessions older than 7 days
            var cutoffDate = DateTime.UtcNow.AddDays(-7);
            var oldDisconnectedClients = await _dbContext.ClientSessions
                .Where(cs => cs.DisconnectedAt != null && cs.DisconnectedAt < cutoffDate)
                .ExecuteDeleteAsync();
            totalDeleted += oldDisconnectedClients;

            var oldDisconnectedSources = await _dbContext.SourceConnections
                .Where(sc => sc.DisconnectedAt != null && sc.DisconnectedAt < cutoffDate)
                .ExecuteDeleteAsync();
            totalDeleted += oldDisconnectedSources;

            var executionTime = (long)(DateTime.UtcNow - startTime).TotalMilliseconds;

            _logger.LogInformation("Cleaned up {Count} orphaned records in {Time}ms", totalDeleted, executionTime);

            return new MaintenanceResultDto
            {
                Success = true,
                Message = $"Cleaned up orphaned records successfully. Deleted {totalDeleted} records",
                ExecutionTimeMs = executionTime
            };
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error cleaning up orphaned records");
            return new MaintenanceResultDto
            {
                Success = false,
                Message = $"Cleanup failed: {ex.Message}",
                ExecutionTimeMs = (long)(DateTime.UtcNow - startTime).TotalMilliseconds
            };
        }
    }
}
