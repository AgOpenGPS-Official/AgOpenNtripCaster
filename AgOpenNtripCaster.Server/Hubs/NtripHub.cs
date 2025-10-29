using Microsoft.AspNetCore.SignalR;
using NtripCaster.Server.Models.DTOs;

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
        _logger.LogInformation($"Client connected: {Context.ConnectionId}");
        await Clients.All.SendAsync("ClientConnected", new { connectionId = Context.ConnectionId });
        await base.OnConnectedAsync();
    }

    public override async Task OnDisconnectedAsync(Exception? exception)
    {
        _logger.LogInformation($"Client disconnected: {Context.ConnectionId}");
        await Clients.All.SendAsync("ClientDisconnected", new { connectionId = Context.ConnectionId });
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
}
