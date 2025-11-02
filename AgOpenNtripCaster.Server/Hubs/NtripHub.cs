using Microsoft.AspNetCore.SignalR;
using AgOpenNtripCaster.Server.Models.DTOs;

namespace AgOpenNtripCaster.Server.Hubs;

public class NtripHub : Hub
{
    private readonly ILogger<NtripHub> _logger;

    public NtripHub(ILogger<NtripHub> logger)
    {
        _logger = logger;
    }

    public override async Task OnConnectedAsync()
    {
        _logger.LogInformation($"SignalR client connected: {Context.ConnectionId}");
        await base.OnConnectedAsync();
    }

    public override async Task OnDisconnectedAsync(Exception? exception)
    {
        _logger.LogInformation($"SignalR client disconnected: {Context.ConnectionId}");
        // Note: NTRIP ClientDisconnected events are sent by NtripServerService.MarkClientSessionDisconnectedAsync
        // to include clientId and username for proper dashboard updates
        await base.OnDisconnectedAsync(exception);
    }

    // Called by NtripClientHandler when position update is received
    public async Task OnClientPositionUpdate(ClientPositionUpdate update)
    {
        _logger.LogDebug($"Position update from client {update.ClientId}: {update.Latitude}, {update.Longitude}");
        await Clients.All.SendAsync("ClientPositionUpdated", update);
    }

    // Called by NtripClientHandler when stream status changes
    public async Task OnClientStreamStatus(ClientStreamStatusUpdate update)
    {
        _logger.LogInformation($"Stream status change for client {update.ClientId}: {update.Status}");
        await Clients.All.SendAsync("ClientStreamStatusChanged", update);
    }

    // Called by NtripServerService when dashboard stats need updating (real-time)
    public async Task OnDashboardStatsUpdate(object stats)
    {
        _logger.LogDebug("Broadcasting dashboard stats update");
        await Clients.All.SendAsync("DashboardStatsUpdated", stats);
    }

    // Called by NtripServerService when new activity is created (real-time)
    public async Task OnActivityCreated(object activity)
    {
        _logger.LogDebug("Broadcasting new activity");
        await Clients.All.SendAsync("ActivityCreated", activity);
    }

    // Called by NtripServerService when mount point status changes
    public async Task OnMountPointStatusChanged(object statusUpdate)
    {
        _logger.LogDebug("Broadcasting mount point status change");
        await Clients.All.SendAsync("MountPointStatusChanged", statusUpdate);
    }

    // Called by NtripServerService for system alerts
    public async Task OnSystemAlert(object alert)
    {
        _logger.LogWarning("Broadcasting system alert");
        await Clients.All.SendAsync("SystemAlert", alert);
    }
}
