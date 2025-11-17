using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using System.Diagnostics;
using System.Text;

namespace AgOpenNtripCaster.Server.Controllers;

[ApiController]
[Route("api/admin/docker-logs")]
[Authorize(Roles = "Admin")]
public class DockerLogsController : ControllerBase
{
    private readonly ILogger<DockerLogsController> _logger;
    private readonly string _deployPath;

    // Available containers that can be monitored
    // Note: Only backend logs are accessible (Serilog files)
    // NGINX and PostgreSQL logs require SSH access and docker compose logs command
    private static readonly Dictionary<string, string> ContainerNames = new()
    {
        { "backend", "ntripcaster-backend" }
    };

    public DockerLogsController(ILogger<DockerLogsController> logger, IWebHostEnvironment env)
    {
        _logger = logger;
        // In Docker, logs are mounted at /app/logs
        _deployPath = "/app/logs";
    }

    /// <summary>
    /// Get list of available containers
    /// </summary>
    [HttpGet("containers")]
    public ActionResult<IEnumerable<ContainerInfo>> GetContainers()
    {
        return Ok(ContainerNames.Select(kvp => new ContainerInfo
        {
            Id = kvp.Key,
            Name = kvp.Value,
            DisplayName = char.ToUpper(kvp.Key[0]) + kvp.Key.Substring(1)
        }));
    }

    /// <summary>
    /// Get tail of container logs (last N lines)
    /// </summary>
    [HttpGet("{containerId}/tail")]
    public async Task<ActionResult<ContainerLogsResponse>> GetContainerLogs(
        string containerId,
        [FromQuery] int lines = 100)
    {
        if (!ContainerNames.TryGetValue(containerId, out var containerName))
        {
            return NotFound(new { error = $"Container '{containerId}' not found" });
        }

        try
        {
            var logs = await ExecuteDockerLogsCommand(containerName, lines);

            return Ok(new ContainerLogsResponse
            {
                ContainerId = containerId,
                ContainerName = containerName,
                Lines = ParseLogLines(logs),
                TotalLines = logs.Split('\n', StringSplitOptions.RemoveEmptyEntries).Length
            });
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to get Docker logs for container {Container}", containerName);
            return StatusCode(500, new { error = "Failed to retrieve container logs", details = ex.Message });
        }
    }

    /// <summary>
    /// Download container logs as text file
    /// </summary>
    [HttpGet("{containerId}/download")]
    public async Task<IActionResult> DownloadContainerLogs(
        string containerId,
        [FromQuery] int lines = 1000)
    {
        if (!ContainerNames.TryGetValue(containerId, out var containerName))
        {
            return NotFound(new { error = $"Container '{containerId}' not found" });
        }

        try
        {
            var logs = await ExecuteDockerLogsCommand(containerName, lines);
            var bytes = Encoding.UTF8.GetBytes(logs);
            var fileName = $"{containerName}-logs-{DateTime.UtcNow:yyyyMMdd-HHmmss}.txt";

            return File(bytes, "text/plain", fileName);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to download Docker logs for container {Container}", containerName);
            return StatusCode(500, new { error = "Failed to download container logs" });
        }
    }

    /// <summary>
    /// Read application log files directly (Serilog output)
    /// </summary>
    private async Task<string> ExecuteDockerLogsCommand(string containerName, int lines)
    {
        // For backend container, read Serilog files
        if (containerName == "ntripcaster-backend")
        {
            return await ReadSerilogFiles(lines);
        }

        // For other containers, we can't access logs from inside backend container
        // Return a helpful message
        throw new InvalidOperationException(
            $"Container logs for '{containerName}' are not accessible from the backend. " +
            "Only backend application logs are available through this interface. " +
            "To view other container logs, use SSH and 'docker compose logs' command."
        );
    }

    /// <summary>
    /// Read Serilog log files from /app/logs
    /// </summary>
    private async Task<string> ReadSerilogFiles(int lines)
    {
        var logFiles = Directory.GetFiles(_deployPath, "ntripcaster-*.txt")
            .OrderByDescending(f => System.IO.File.GetLastWriteTimeUtc(f))
            .ToList();

        if (!logFiles.Any())
        {
            return "No log files found.";
        }

        var allLines = new List<string>();

        // Read latest log file first
        foreach (var logFile in logFiles)
        {
            try
            {
                var fileLines = await System.IO.File.ReadAllLinesAsync(logFile);
                allLines.AddRange(fileLines);

                if (allLines.Count >= lines)
                    break;
            }
            catch (Exception ex)
            {
                _logger.LogWarning(ex, "Failed to read log file {File}", logFile);
            }
        }

        // Take last N lines
        var result = allLines.TakeLast(lines).ToList();
        return string.Join("\n", result);
    }

    /// <summary>
    /// Parse log lines and extract metadata
    /// </summary>
    private List<LogLine> ParseLogLines(string logs)
    {
        var lines = logs.Split('\n', StringSplitOptions.RemoveEmptyEntries);
        var result = new List<LogLine>();

        foreach (var line in lines)
        {
            // Serilog format: [23:47:25 ERR] message
            // or [2025-11-16 23:47:25 INF] message
            var level = DetectLogLevel(line);
            var timestamp = ExtractTimestamp(line);

            result.Add(new LogLine
            {
                Timestamp = timestamp,
                Level = level,
                Message = line,
                RawLine = line
            });
        }

        return result;
    }

    /// <summary>
    /// Detect log level from message content
    /// </summary>
    private string DetectLogLevel(string message)
    {
        // Serilog format: [HH:mm:ss LVL] or [yyyy-MM-dd HH:mm:ss LVL]
        if (message.Contains("[ERR]") || message.Contains("ERROR") || message.Contains("FATAL"))
            return "ERROR";
        if (message.Contains("[WRN]") || message.Contains("WARN"))
            return "WARNING";
        if (message.Contains("[DBG]") || message.Contains("DEBUG"))
            return "DEBUG";
        if (message.Contains("[INF]") || message.Contains("INFO"))
            return "INFO";

        return "INFO";
    }

    /// <summary>
    /// Extract timestamp from log message if present
    /// </summary>
    private DateTime ExtractTimestamp(string message)
    {
        // Try to parse common timestamp formats
        // Example: 2025/11/16 23:15:01 or [2025-11-16 23:15:01]

        var dateFormats = new[]
        {
            "yyyy/MM/dd HH:mm:ss",
            "yyyy-MM-dd HH:mm:ss",
            "[yyyy-MM-dd HH:mm:ss]"
        };

        foreach (var format in dateFormats)
        {
            var prefix = message.Length > 25 ? message.Substring(0, 25) : message;
            if (DateTime.TryParseExact(prefix.Trim('[', ']'), format, null,
                System.Globalization.DateTimeStyles.None, out var timestamp))
            {
                return timestamp;
            }
        }

        return DateTime.UtcNow;
    }

    /// <summary>
    /// Convert container name to docker-compose service name
    /// </summary>
    private string GetServiceNameFromContainer(string containerName)
    {
        // Container: ntripcaster-backend → Service: backend
        return containerName.Replace("ntripcaster-", "");
    }
}

// DTOs
public class ContainerInfo
{
    public string Id { get; set; } = string.Empty;
    public string Name { get; set; } = string.Empty;
    public string DisplayName { get; set; } = string.Empty;
}

public class ContainerLogsResponse
{
    public string ContainerId { get; set; } = string.Empty;
    public string ContainerName { get; set; } = string.Empty;
    public List<LogLine> Lines { get; set; } = new();
    public int TotalLines { get; set; }
}

public class LogLine
{
    public DateTime Timestamp { get; set; }
    public string Level { get; set; } = string.Empty;
    public string Message { get; set; } = string.Empty;
    public string RawLine { get; set; } = string.Empty;
}
