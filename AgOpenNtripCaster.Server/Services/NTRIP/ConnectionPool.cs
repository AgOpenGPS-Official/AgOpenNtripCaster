using System.Collections.Concurrent;
using System.Net.Sockets;

namespace AgOpenNtripCaster.Server.Services.NTRIP;

/// <summary>
/// Manages concurrent client and source connections
/// Tracks active connections and enforces limits
/// </summary>
public class ConnectionPool
{
    private readonly ConcurrentDictionary<string, ClientConnectionInfo> _clientConnections;
    public readonly ConcurrentDictionary<string, SourceConnectionInfo> _sourceConnections;
    private readonly int _maxClientsPerMount;
    private readonly int _maxSources;
    private readonly ILogger<ConnectionPool> _logger;

    public ConnectionPool(ILogger<ConnectionPool> logger, int maxClientsPerMount = 50, int maxSources = 10)
    {
        _logger = logger;
        _maxClientsPerMount = maxClientsPerMount;
        _maxSources = maxSources;
        _clientConnections = new ConcurrentDictionary<string, ClientConnectionInfo>();
        _sourceConnections = new ConcurrentDictionary<string, SourceConnectionInfo>();
    }

    /// <summary>
    /// Register a new client connection
    /// </summary>
    public bool RegisterClient(string clientId, string mountPointName, string username, TcpClient tcpClient)
    {
        // Check if max clients for this mount point is exceeded
        var clientsForMount = _clientConnections
            .Values
            .Count(c => c.MountPointName == mountPointName && !c.IsDisconnected);

        if (clientsForMount >= _maxClientsPerMount)
        {
            _logger.LogWarning($"Max clients ({_maxClientsPerMount}) exceeded for mount point {mountPointName}");
            return false;
        }

        var info = new ClientConnectionInfo
        {
            Id = clientId,
            MountPointName = mountPointName,
            Username = username,
            TcpClient = tcpClient,
            ConnectedAt = DateTime.UtcNow
        };

        var added = _clientConnections.TryAdd(clientId, info);
        if (added)
        {
            _logger.LogInformation($"Client registered: {clientId} ({username}@{mountPointName})");
        }

        return added;
    }

    /// <summary>
    /// Unregister a client connection
    /// </summary>
    public bool UnregisterClient(string clientId)
    {
        var removed = _clientConnections.TryRemove(clientId, out var info);
        if (removed && info != null)
        {
            _logger.LogInformation($"Client unregistered: {clientId} ({info.Username})");
        }
        return removed;
    }

    /// <summary>
    /// Register a source connection
    /// </summary>
    public bool RegisterSource(string sourceId, string mountPointName, TcpClient tcpClient)
    {
        // Check if max sources exceeded
        if (_sourceConnections.Count >= _maxSources)
        {
            _logger.LogWarning($"Max sources ({_maxSources}) exceeded");
            return false;
        }

        var info = new SourceConnectionInfo
        {
            Id = sourceId,
            MountPointName = mountPointName,
            TcpClient = tcpClient,
            ConnectedAt = DateTime.UtcNow
        };

        var added = _sourceConnections.TryAdd(sourceId, info);
        if (added)
        {
            _logger.LogInformation($"Source registered: {sourceId} ({mountPointName})");
        }

        return added;
    }

    /// <summary>
    /// Unregister a source connection
    /// </summary>
    public bool UnregisterSource(string sourceId)
    {
        var removed = _sourceConnections.TryRemove(sourceId, out var info);
        if (removed && info != null)
        {
            _logger.LogInformation($"Source unregistered: {sourceId} ({info.MountPointName})");
        }
        return removed;
    }

    /// <summary>
    /// Get all clients connected to a specific mount point
    /// </summary>
    public List<ClientConnectionInfo> GetClientsForMountPoint(string mountPointName)
    {
        return _clientConnections
            .Values
            .Where(c => c.MountPointName == mountPointName && !c.IsDisconnected)
            .ToList();
    }

    /// <summary>
    /// Get source for a specific mount point
    /// </summary>
    public SourceConnectionInfo? GetSourceForMountPoint(string mountPointName)
    {
        return _sourceConnections
            .Values
            .FirstOrDefault(s => s.MountPointName == mountPointName && !s.IsDisconnected);
    }

    /// <summary>
    /// Get active connection count
    /// </summary>
    public int GetActiveClientCount() => _clientConnections.Values.Count(c => !c.IsDisconnected);
    public int GetActiveSourceCount() => _sourceConnections.Values.Count(s => !s.IsDisconnected);

    /// <summary>
    /// Get connection info
    /// </summary>
    public ClientConnectionInfo? GetClient(string clientId)
    {
        _clientConnections.TryGetValue(clientId, out var info);
        return info;
    }

    /// <summary>
    /// Get all active client connections (for health checks)
    /// </summary>
    public List<ClientConnectionInfo> GetAllActiveClients()
    {
        return _clientConnections
            .Values
            .Where(c => !c.IsDisconnected)
            .ToList();
    }

    /// <summary>
    /// Get all active source connections (for health checks)
    /// </summary>
    public List<SourceConnectionInfo> GetAllActiveSources()
    {
        return _sourceConnections
            .Values
            .Where(s => !s.IsDisconnected)
            .ToList();
    }
}

public class ClientConnectionInfo
{
    public string Id { get; set; } = string.Empty;
    public string MountPointName { get; set; } = string.Empty;
    public string Username { get; set; } = string.Empty;
    public TcpClient? TcpClient { get; set; }
    public DateTime ConnectedAt { get; set; }
    public DateTime? DisconnectedAt { get; set; }
    public DateTime LastActivityAt { get; set; } = DateTime.UtcNow;
    public bool IsDisconnected => DisconnectedAt.HasValue;

    // Position tracking
    public double? LastLatitude { get; set; }
    public double? LastLongitude { get; set; }
    public double? LastAccuracy { get; set; }
    public DateTime? LastPositionAt { get; set; }

    // Stream control
    public bool IsStreaming { get; set; }
    public ClientReadPosition? ReadPosition { get; set; }

    // Statistics
    public long BytesSent { get; set; }
    public long BytesReceived { get; set; }
}

public class SourceConnectionInfo
{
    public string Id { get; set; } = string.Empty;
    public string MountPointName { get; set; } = string.Empty;
    public TcpClient? TcpClient { get; set; }
    public DateTime ConnectedAt { get; set; }
    public DateTime? DisconnectedAt { get; set; }
    public DateTime LastActivityAt { get; set; } = DateTime.UtcNow;
    public bool IsDisconnected => DisconnectedAt.HasValue;

    // Statistics
    public long BytesReceived { get; set; }
    public long BytesSent { get; set; }
}
