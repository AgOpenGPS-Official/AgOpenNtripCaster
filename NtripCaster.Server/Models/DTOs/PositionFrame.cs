namespace NtripCaster.Server.Models.DTOs;

public class PositionFrame
{
    public double Latitude { get; set; }
    public double Longitude { get; set; }
    public double? Altitude { get; set; }
    public double? Accuracy { get; set; }
    public DateTime Timestamp { get; set; } = DateTime.UtcNow;
}

public class ClientPositionUpdate
{
    public string ClientId { get; set; } = string.Empty;
    public string Username { get; set; } = string.Empty;
    public string MountPoint { get; set; } = string.Empty;
    public double Latitude { get; set; }
    public double Longitude { get; set; }
    public double? Accuracy { get; set; }
    public DateTime Timestamp { get; set; } = DateTime.UtcNow;
}

public class ClientStreamStatusUpdate
{
    public string ClientId { get; set; } = string.Empty;
    public string Username { get; set; } = string.Empty;
    public string Status { get; set; } = "connected";  // streaming/paused/connected
    public string Reason { get; set; } = string.Empty;
    public DateTime UpdatedAt { get; set; } = DateTime.UtcNow;
}
