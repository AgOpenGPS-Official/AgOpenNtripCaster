namespace AgOpenNtripCaster.Server.Models.DTOs;

/// <summary>
/// Email configuration settings
/// </summary>
public class EmailConfigDto
{
    public string SmtpServer { get; set; } = "smtp.gmail.com";
    public int SmtpPort { get; set; } = 587;
    public string SenderEmail { get; set; } = string.Empty;
    public string SenderPassword { get; set; } = string.Empty;
    public bool UseTls { get; set; } = true;
}

/// <summary>
/// Logging configuration settings
/// </summary>
public class LoggingConfigDto
{
    public string LogLevel { get; set; } = "Information";
    public int MaxLogSize { get; set; } = 100;
    public int RetentionDays { get; set; } = 30;
    public bool EnableConsoleLogging { get; set; } = true;
    public bool EnableFileLogging { get; set; } = true;
}

/// <summary>
/// System settings response
/// </summary>
public class SystemSettingsDto
{
    public EmailConfigDto EmailConfig { get; set; } = new();
    public LoggingConfigDto LoggingConfig { get; set; } = new();
}

/// <summary>
/// Security policy settings
/// </summary>
public class SecurityPolicyDto
{
    public bool RequireMfa { get; set; } = false;
    public int PasswordMinLength { get; set; } = 8;
    public int PasswordExpireDays { get; set; } = 90;
    public int MaxLoginAttempts { get; set; } = 5;
    public int LockoutDurationMinutes { get; set; } = 15;
    public int SessionTimeoutMinutes { get; set; } = 60;
    public bool IpWhitelistEnabled { get; set; } = false;
    public string IpWhitelist { get; set; } = string.Empty;
    public bool TlsEnabled { get; set; } = true;
}

/// <summary>
/// System log entry
/// </summary>
public class SystemLogDto
{
    public int Id { get; set; }
    public DateTime Timestamp { get; set; }
    public string Level { get; set; } = string.Empty;
    public string Message { get; set; } = string.Empty;
    public string? Exception { get; set; }
}

/// <summary>
/// Analytics statistics
/// </summary>
public class AnalyticsDto
{
    public int TotalConnections { get; set; }
    public double TotalDataTransferred { get; set; }
    public string AverageSessionDuration { get; set; } = string.Empty;
    public string PeakConnectionTime { get; set; } = string.Empty;
}

/// <summary>
/// Connection trend data point
/// </summary>
public class ConnectionTrendDto
{
    public DateTime Timestamp { get; set; }
    public int ClientCount { get; set; }
    public int SourceCount { get; set; }
}

/// <summary>
/// Data transfer statistics
/// </summary>
public class DataTransferStatsDto
{
    public DateTime Timestamp { get; set; }
    public double BytesSent { get; set; }
    public double BytesReceived { get; set; }
}

/// <summary>
/// Database statistics
/// </summary>
public class DatabaseStatsDto
{
    public string TotalSize { get; set; } = string.Empty;
    public int TableCount { get; set; }
    public int RecordCount { get; set; }
    public DateTime LastBackup { get; set; }
    public string DatabaseVersion { get; set; } = string.Empty;
}

/// <summary>
/// Table information
/// </summary>
public class TableInfoDto
{
    public string Name { get; set; } = string.Empty;
    public int Records { get; set; }
    public string Size { get; set; } = string.Empty;
}

/// <summary>
/// Database maintenance operation result
/// </summary>
public class MaintenanceResultDto
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
    public long ExecutionTimeMs { get; set; }
}
