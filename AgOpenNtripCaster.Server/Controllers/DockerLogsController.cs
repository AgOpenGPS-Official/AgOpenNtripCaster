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
    private static readonly Dictionary<string, string> ContainerNames = new()
    {
        { "backend", "ntripcaster-backend" },
        { "nginx", "ntripcaster-nginx" },
        { "postgres", "ntripcaster-postgres" }
    };

    public DockerLogsController(ILogger<DockerLogsController> logger, IWebHostEnvironment env)
    {
        _logger = logger;
        // Assume deploy folder is at project root level
        _deployPath = Path.Combine(env.ContentRootPath, "..", "deploy");
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
    /// Execute docker compose logs command
    /// </summary>
    private async Task<string> ExecuteDockerLogsCommand(string containerName, int lines)
    {
        var processStartInfo = new ProcessStartInfo
        {
            FileName = "docker",
            Arguments = $"compose -f docker-compose.yml logs --tail {lines} {GetServiceNameFromContainer(containerName)}",
            WorkingDirectory = _deployPath,
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
            CreateNoWindow = true
        };

        using var process = Process.Start(processStartInfo);
        if (process == null)
        {
            throw new InvalidOperationException("Failed to start docker process");
        }

        var output = await process.StandardOutput.ReadToEndAsync();
        var error = await process.StandardError.ReadToEndAsync();

        await process.WaitForExitAsync();

        if (process.ExitCode != 0)
        {
            throw new InvalidOperationException($"Docker command failed: {error}");
        }

        return output;
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
            // Docker compose logs format: container-name | log message
            var parts = line.Split('|', 2, StringSplitOptions.TrimEntries);

            if (parts.Length == 2)
            {
                var message = parts[1];
                var level = DetectLogLevel(message);
                var timestamp = ExtractTimestamp(message);

                result.Add(new LogLine
                {
                    Timestamp = timestamp,
                    Level = level,
                    Message = message,
                    RawLine = line
                });
            }
            else
            {
                // Fallback for lines without container prefix
                result.Add(new LogLine
                {
                    Timestamp = DateTime.UtcNow,
                    Level = "INFO",
                    Message = line,
                    RawLine = line
                });
            }
        }

        return result;
    }

    /// <summary>
    /// Detect log level from message content
    /// </summary>
    private string DetectLogLevel(string message)
    {
        var upper = message.ToUpperInvariant();

        if (upper.Contains("[ERR]") || upper.Contains("ERROR") || upper.Contains("FATAL"))
            return "ERROR";
        if (upper.Contains("[WRN]") || upper.Contains("WARN"))
            return "WARNING";
        if (upper.Contains("[DBG]") || upper.Contains("DEBUG"))
            return "DEBUG";

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
