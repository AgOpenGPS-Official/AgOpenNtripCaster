using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using AgOpenNtripCaster.Server.Models.DTOs;
using AgOpenNtripCaster.Server.Data;
using Microsoft.EntityFrameworkCore;

namespace AgOpenNtripCaster.Server.Controllers;

/// <summary>
/// Analytics and reporting endpoints
/// Requires Admin or ReadOnly role (ReadOnly can view all data)
/// </summary>
[ApiController]
[Route("api/admin/analytics")]
[Authorize(Roles = "Admin,ReadOnly")]
public class AnalyticsController : ControllerBase
{
    private readonly ILogger<AnalyticsController> _logger;
    private readonly ApplicationDbContext _dbContext;

    public AnalyticsController(ILogger<AnalyticsController> logger, ApplicationDbContext dbContext)
    {
        _logger = logger;
        _dbContext = dbContext;
    }

    /// <summary>
    /// Get analytics overview
    /// </summary>
    [HttpGet("overview")]
    [ProducesResponseType(typeof(AnalyticsDto), 200)]
    public async Task<IActionResult> GetAnalyticsOverview([FromQuery] string dateRange = "7d")
    {
        try
        {
            var startDate = DateTime.UtcNow;
            startDate = dateRange switch
            {
                "24h" => startDate.AddHours(-24),
                "30d" => startDate.AddDays(-30),
                "90d" => startDate.AddDays(-90),
                "1y" => startDate.AddYears(-1),
                _ => startDate.AddDays(-7) // default 7d
            };

            // Get total unique clients in date range
            var totalConnections = await _dbContext.ClientSessions
                .Where(cs => cs.ConnectedAt >= startDate)
                .CountAsync();

            // Get total data transferred (sum of bytes sent/received)
            var dataTransferred = await _dbContext.ClientSessions
                .Where(cs => cs.ConnectedAt >= startDate)
                .SumAsync(cs => (long)cs.BytesReceived + (long)cs.BytesSent) / (1024.0 * 1024.0); // Convert to MB

            // Get average session duration
            var sessions = await _dbContext.ClientSessions
                .Where(cs => cs.ConnectedAt >= startDate && cs.DisconnectedAt.HasValue)
                .ToListAsync();

            var averageSessionDuration = "0m";
            if (sessions.Count > 0)
            {
#pragma warning disable CS8629 // Nullable value type may be null - safe because of Count check above
                var avgTicks = (long)sessions.Average(s => (s.DisconnectedAt.Value - s.ConnectedAt).Ticks)!;
                var avgTimespan = new TimeSpan(avgTicks);
#pragma warning restore CS8629

                if (avgTimespan.TotalHours >= 1)
                    averageSessionDuration = $"{(int)avgTimespan.TotalHours}h {avgTimespan.Minutes}m";
                else if (avgTimespan.TotalMinutes >= 1)
                    averageSessionDuration = $"{(int)avgTimespan.TotalMinutes}m {avgTimespan.Seconds}s";
                else
                    averageSessionDuration = $"{avgTimespan.Seconds}s";
            }

            // Get peak connection time (need to fetch to client first, then group by hour)
            var peakTime = (await _dbContext.ClientSessions
                .Where(cs => cs.ConnectedAt >= startDate)
                .Select(cs => cs.ConnectedAt)
                .ToListAsync())
                .GroupBy(dt => dt.Hour)
                .OrderByDescending(g => g.Count())
                .FirstOrDefault();

            var peakConnectionTime = peakTime != null
                ? $"{peakTime.Key:00}:00"
                : "N/A";

            var analytics = new AnalyticsDto
            {
                TotalConnections = totalConnections,
                TotalDataTransferred = dataTransferred,
                AverageSessionDuration = averageSessionDuration,
                PeakConnectionTime = peakConnectionTime
            };

            return Ok(analytics);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving analytics overview");
            return StatusCode(500, new { message = "Error retrieving analytics", error = ex.Message });
        }
    }

    /// <summary>
    /// Get connection trends over time
    /// </summary>
    [HttpGet("connection-trends")]
    [ProducesResponseType(typeof(List<ConnectionTrendDto>), 200)]
    public async Task<IActionResult> GetConnectionTrends([FromQuery] string dateRange = "7d")
    {
        try
        {
            var startDate = DateTime.UtcNow;
            var daysBack = dateRange switch
            {
                "24h" => 1,
                "30d" => 30,
                "90d" => 90,
                "1y" => 365,
                _ => 7 // default 7d
            };
            startDate = startDate.AddDays(-daysBack);

            var trends = new List<ConnectionTrendDto>();

            // Get client sessions grouped by date
            var clientData = await _dbContext.ClientSessions
                .Where(cs => cs.ConnectedAt >= startDate)
                .GroupBy(cs => cs.ConnectedAt.Date)
                .Select(g => new
                {
                    Date = g.Key,
                    ClientCount = g.Count()
                })
                .OrderBy(x => x.Date)
                .ToListAsync();

            // Get source connections grouped by date
            var sourceData = await _dbContext.SourceConnections
                .Where(sc => sc.ConnectedAt >= startDate)
                .GroupBy(sc => sc.ConnectedAt.Date)
                .Select(g => new
                {
                    Date = g.Key,
                    SourceCount = g.Count()
                })
                .OrderBy(x => x.Date)
                .ToListAsync();

            // Merge the data
            var allDates = clientData.Select(c => c.Date)
                .Union(sourceData.Select(s => s.Date))
                .OrderBy(d => d)
                .ToList();

            foreach (var date in allDates)
            {
                var clientCount = clientData.FirstOrDefault(c => c.Date == date)?.ClientCount ?? 0;
                var sourceCount = sourceData.FirstOrDefault(s => s.Date == date)?.SourceCount ?? 0;

                trends.Add(new ConnectionTrendDto
                {
                    Timestamp = date,
                    ClientCount = clientCount,
                    SourceCount = sourceCount
                });
            }

            return Ok(trends);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving connection trends");
            return StatusCode(500, new { message = "Error retrieving trends", error = ex.Message });
        }
    }

    /// <summary>
    /// Get data transfer statistics
    /// </summary>
    [HttpGet("data-transfer")]
    [ProducesResponseType(typeof(List<DataTransferStatsDto>), 200)]
    public async Task<IActionResult> GetDataTransferStats([FromQuery] string dateRange = "7d")
    {
        try
        {
            var startDate = DateTime.UtcNow;
            var daysBack = dateRange switch
            {
                "24h" => 1,
                "30d" => 30,
                "90d" => 90,
                "1y" => 365,
                _ => 7 // default 7d
            };
            startDate = startDate.AddDays(-daysBack);

            var stats = new List<DataTransferStatsDto>();

            // Get data transfer grouped by date from client sessions
            var transferData = await _dbContext.ClientSessions
                .Where(cs => cs.ConnectedAt >= startDate)
                .GroupBy(cs => cs.ConnectedAt.Date)
                .Select(g => new
                {
                    Date = g.Key,
                    BytesSent = g.Sum(cs => (long)cs.BytesSent),
                    BytesReceived = g.Sum(cs => (long)cs.BytesReceived)
                })
                .OrderBy(x => x.Date)
                .ToListAsync();

            foreach (var data in transferData)
            {
                stats.Add(new DataTransferStatsDto
                {
                    Timestamp = data.Date,
                    BytesSent = data.BytesSent,
                    BytesReceived = data.BytesReceived
                });
            }

            return Ok(stats);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving data transfer stats");
            return StatusCode(500, new { message = "Error retrieving stats", error = ex.Message });
        }
    }

    /// <summary>
    /// Get user activity report
    /// </summary>
    [HttpGet("user-activity")]
    [ProducesResponseType(typeof(object), 200)]
    public async Task<IActionResult> GetUserActivityReport([FromQuery] string dateRange = "7d")
    {
        try
        {
            var startDate = DateTime.UtcNow;
            startDate = dateRange switch
            {
                "24h" => startDate.AddHours(-24),
                "30d" => startDate.AddDays(-30),
                "90d" => startDate.AddDays(-90),
                "1y" => startDate.AddYears(-1),
                _ => startDate.AddDays(-7) // default 7d
            };

            // Get total unique users with sessions
            var totalUsers = await _dbContext.ClientSessions
                .Where(cs => cs.ConnectedAt >= startDate)
                .Select(cs => cs.UserId)
                .Distinct()
                .CountAsync();

            // Get active users (connected in last 24 hours)
            var activeUsers = await _dbContext.ClientSessions
                .Where(cs => cs.ConnectedAt >= DateTime.UtcNow.AddHours(-24))
                .Select(cs => cs.UserId)
                .Distinct()
                .CountAsync();

            // Get new users (created in date range)
            var newUsers = await _dbContext.Users
                .Where(u => u.CreatedAt >= startDate)
                .CountAsync();

            // Get avg sessions per user
            var userSessions = await _dbContext.ClientSessions
                .Where(cs => cs.ConnectedAt >= startDate)
                .GroupBy(cs => cs.UserId)
                .Select(g => new { UserId = g.Key, Sessions = g.Count() })
                .ToListAsync();

            var avgSessionsPerUser = userSessions.Count > 0
                ? (double)userSessions.Sum(us => us.Sessions) / userSessions.Count
                : 0;

            // Get users by group
            var usersByGroup = await _dbContext.NtripGroups
                .Include(g => g.Users)
                .Select(g => new
                {
                    groupName = g.Name,
                    users = g.Users.Count,
                    sessions = _dbContext.ClientSessions
                        .Where(cs => cs.ConnectedAt >= startDate && g.Users.Select(u => u.Id).Contains(cs.UserId ?? ""))
                        .Count()
                })
                .ToListAsync();

            var report = new
            {
                totalUsers,
                activeUsers,
                newUsers,
                avgSessionsPerUser = Math.Round(avgSessionsPerUser, 2),
                usersByGroup
            };

            return Ok(report);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving user activity report");
            return StatusCode(500, new { message = "Error retrieving report", error = ex.Message });
        }
    }

    /// <summary>
    /// Get performance metrics
    /// </summary>
    [HttpGet("performance")]
    [ProducesResponseType(typeof(object), 200)]
    public async Task<IActionResult> GetPerformanceMetrics()
    {
        try
        {
            // Get database stats
            var totalActiveSessions = await _dbContext.ClientSessions
                .Where(cs => cs.DisconnectedAt == null)
                .CountAsync();

            var totalRecords = await _dbContext.ClientSessions.CountAsync()
                + await _dbContext.SourceConnections.CountAsync()
                + await _dbContext.Activities.CountAsync();

            // Calculate approximate metrics (fetch to client first for calculation)
#pragma warning disable CS8629 // Nullable value type may be null - safe because of HasValue check in Where
            var sessionDurations = await _dbContext.ClientSessions
                .Where(cs => cs.DisconnectedAt.HasValue)
                .Select(cs => new { Duration = (cs.DisconnectedAt.Value - cs.ConnectedAt).TotalSeconds })
                .ToListAsync();
#pragma warning restore CS8629

#pragma warning disable CS8629 // Nullable value type may be null - safe because of Count check
            var avgSessionDuration = sessionDurations.Count > 0
                ? sessionDurations.Average(s => s.Duration)!
                : 0;
#pragma warning restore CS8629

            var metrics = new
            {
                activeSessions = totalActiveSessions,
                totalDatabaseRecords = totalRecords,
                avgSessionDuration = $"{(int)avgSessionDuration}s",
                totalConnections = await _dbContext.ClientSessions.CountAsync(),
                totalSources = await _dbContext.SourceConnections.CountAsync(),
                totalActivities = await _dbContext.Activities.CountAsync(),
                cpuUsage = "N/A",
                memoryUsage = "N/A"
            };

            return Ok(metrics);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving performance metrics");
            return StatusCode(500, new { message = "Error retrieving metrics", error = ex.Message });
        }
    }

    /// <summary>
    /// Export analytics report as CSV
    /// </summary>
    [HttpGet("export-csv")]
    [ProducesResponseType(typeof(FileResult), 200)]
    public IActionResult ExportAnalyticsCsv([FromQuery] string dateRange = "7d")
    {
        try
        {
            _logger.LogInformation("Exporting analytics report by {User}", User.Identity?.Name);

            var csvContent = "Date,Connections,DataTransferred,AvgSessionDuration\n";
            var bytes = System.Text.Encoding.UTF8.GetBytes(csvContent);

            return File(bytes, "text/csv", $"analytics_{DateTime.UtcNow:yyyyMMdd_HHmmss}.csv");
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error exporting analytics");
            return StatusCode(500, new { message = "Error exporting analytics", error = ex.Message });
        }
    }

    /// <summary>
    /// Export analytics report as PDF
    /// </summary>
    [HttpGet("export-pdf")]
    [ProducesResponseType(typeof(FileResult), 200)]
    public IActionResult ExportAnalyticsPdf([FromQuery] string dateRange = "7d")
    {
        try
        {
            _logger.LogInformation("Exporting analytics report as PDF by {User}", User.Identity?.Name);

            // In real implementation, generate PDF
            var pdfContent = System.Text.Encoding.UTF8.GetBytes("PDF content would be here...");

            return File(pdfContent, "application/pdf", $"analytics_{DateTime.UtcNow:yyyyMMdd_HHmmss}.pdf");
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error exporting analytics as PDF");
            return StatusCode(500, new { message = "Error exporting PDF", error = ex.Message });
        }
    }
}
