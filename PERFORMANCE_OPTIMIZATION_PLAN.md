# AgOpenNtripCaster Performance Optimization Plan
**Based on Millipede-Caster Analysis**

## Executive Summary
This document outlines a comprehensive plan to implement high-performance optimizations inspired by the Millipede NTRIP caster, which successfully handles 10,000+ simultaneous connections. Each optimization is broken down into complete implementation steps covering UI, Backend, Caster logic, and Database changes.

---

## Phase 1: Zero-Copy Data Distribution 🔥 CRITICAL

**Expected Impact:** 90% memory reduction, 3-5x throughput increase
**Complexity:** Medium
**Timeline:** 2-3 weeks

### Overview
Currently, RTCM data is copied for each client connection. With 1000 clients, this means 1000 memory copies per packet. Zero-copy uses shared memory with reference counting.

### Step 1.1: Backend - Implement Shared Buffer Pool
**Files:** `AgOpenNtripCaster.Server/Services/NtripServerService.cs`

```csharp
public class SharedRtcmBuffer : IDisposable
{
    private readonly IMemoryOwner<byte> _memoryOwner;
    private int _refCount;
    private readonly object _lock = new();

    public ReadOnlyMemory<byte> Data { get; }
    public int Length { get; }

    public SharedRtcmBuffer(byte[] data)
    {
        _memoryOwner = MemoryPool<byte>.Shared.Rent(data.Length);
        data.CopyTo(_memoryOwner.Memory.Span);
        Data = _memoryOwner.Memory.Slice(0, data.Length);
        Length = data.Length;
        _refCount = 1;
    }

    public void AddRef()
    {
        lock (_lock)
        {
            _refCount++;
        }
    }

    public void Release()
    {
        lock (_lock)
        {
            _refCount--;
            if (_refCount == 0)
            {
                _memoryOwner.Dispose();
            }
        }
    }

    public void Dispose() => Release();
}
```

**Changes needed:**
- Add `SharedRtcmBuffer` class to Server project
- Modify `BroadcastToClients()` to use shared buffers
- Update `ClientSession` to track buffer references

### Step 1.2: Caster Logic - Modify Broadcast Method
**Files:** `AgOpenNtripCaster.Server/Services/NtripServerService.cs`

```csharp
private async Task BroadcastToClientsAsync(string mountPoint, byte[] rtcmData)
{
    var clients = GetClientsForMountPoint(mountPoint);
    if (!clients.Any()) return;

    // Create shared buffer with reference count = number of clients
    using var sharedBuffer = new SharedRtcmBuffer(rtcmData);

    // Set initial reference count
    for (int i = 1; i < clients.Count; i++)
    {
        sharedBuffer.AddRef();
    }

    // Send to all clients concurrently without copying
    var sendTasks = clients.Select(client =>
        client.SendSharedAsync(sharedBuffer));

    await Task.WhenAll(sendTasks);
}
```

**Changes needed:**
- Replace current broadcast loop with parallel sends
- Add `SendSharedAsync()` method to `ClientSession`
- Track buffer references per client

### Step 1.3: Database - Add Metrics Tracking
**Files:** `AgOpenNtripCaster.Server/Data/ApplicationDbContext.cs`

```csharp
public class PerformanceMetric
{
    public int Id { get; set; }
    public DateTime Timestamp { get; set; }
    public long TotalBytesSent { get; set; }
    public long MemorySavedBytes { get; set; }
    public int ActiveZeroCopyBuffers { get; set; }
    public double AverageBroadcastTimeMs { get; set; }
}
```

**Migration:**
```bash
dotnet ef migrations add AddPerformanceMetrics
dotnet ef database update
```

### Step 1.4: Admin UI - Performance Dashboard
**Files:** `AgOpenNtripCaster.Client/src/pages/admin/PerformancePage.tsx`

**New page showing:**
- Real-time memory usage (with/without zero-copy)
- Active shared buffers count
- Memory saved per second
- Broadcast latency graph

**API Endpoint:**
```csharp
[HttpGet("api/admin/performance/metrics")]
public async Task<ActionResult<PerformanceMetricsDto>> GetPerformanceMetrics()
{
    var metrics = new PerformanceMetricsDto
    {
        ZeroCopyEnabled = true,
        ActiveSharedBuffers = _bufferPool.ActiveCount,
        MemorySavedMB = _bufferPool.MemorySavedBytes / (1024.0 * 1024.0),
        AverageBroadcastLatencyMs = _metrics.AverageBroadcastTime
    };
    return Ok(metrics);
}
```

### Step 1.5: Testing & Verification
**Test scenarios:**
1. Load test with 100 clients on same mount point
2. Verify memory usage is ~1MB instead of ~100MB
3. Monitor buffer leak detection (all buffers should be released)
4. Check broadcast latency < 1ms per client

**Success criteria:**
- ✅ Memory usage reduced by >80%
- ✅ No buffer leaks after 24h continuous operation
- ✅ Throughput increased by >3x
- ✅ Performance dashboard shows correct metrics

---

## Phase 2: Automatic Slow Client Detection & Disconnection 🔥 CRITICAL

**Expected Impact:** Prevents OOM crashes, protects healthy clients
**Complexity:** Low
**Timeline:** 1 week

### Overview
One slow client can exhaust server memory and affect all clients. Implement automatic detection and disconnection of clients that can't keep up.

### Step 2.1: Backend - Add Backlog Monitoring
**Files:** `AgOpenNtripCaster.Server/Services/ClientSession.cs`

```csharp
public class ClientSession
{
    private const int MaxBacklogBytes = 16384; // 16KB like Millipede
    private const int BacklogCheckIntervalMs = 1000;

    public long PendingOutputBytes { get; private set; }
    public DateTime? BacklogExceededAt { get; private set; }

    private async Task MonitorBacklogAsync(CancellationToken ct)
    {
        while (!ct.IsCancellationRequested)
        {
            await Task.Delay(BacklogCheckIntervalMs, ct);

            // Check if output buffer is too large
            if (PendingOutputBytes > MaxBacklogBytes)
            {
                if (BacklogExceededAt == null)
                {
                    BacklogExceededAt = DateTime.UtcNow;
                    _logger.LogWarning("Client {Id} backlog exceeded: {Bytes} bytes",
                        Id, PendingOutputBytes);
                }

                // Disconnect if backlogged for >5 seconds
                var backlogDuration = DateTime.UtcNow - BacklogExceededAt.Value;
                if (backlogDuration.TotalSeconds > 5)
                {
                    _logger.LogWarning("Disconnecting slow client {Id} (backlogged for {Duration}s)",
                        Id, backlogDuration.TotalSeconds);
                    await DisconnectAsync("Output buffer backlog exceeded");
                    break;
                }
            }
            else
            {
                BacklogExceededAt = null; // Reset if recovered
            }
        }
    }
}
```

**Changes needed:**
- Add backlog monitoring task to `ClientSession`
- Track pending bytes in output buffer
- Implement graceful disconnect with reason

### Step 2.2: Database - Track Disconnection Reasons
**Files:** `AgOpenNtripCaster.Server/Models/Entities/ClientSession.cs`

```csharp
public enum DisconnectReason
{
    ClientClosed,
    ServerClosed,
    BacklogExceeded,
    Timeout,
    Error
}

public class ClientSession
{
    // ... existing properties
    public DisconnectReason? DisconnectReason { get; set; }
    public string? DisconnectMessage { get; set; }
}
```

**Migration:**
```bash
dotnet ef migrations add AddDisconnectReason
dotnet ef database update
```

### Step 2.3: Admin UI - Client Health Dashboard
**Files:** `AgOpenNtripCaster.Client/src/pages/admin/ClientHealthPage.tsx`

**New page showing:**
- List of currently backlogged clients (warning state)
- Historical disconnections by reason (pie chart)
- Average output buffer size per client
- Alert when multiple clients are backlogged

**Features:**
- Real-time client list with buffer size column
- Color coding: Green (<4KB), Yellow (4-12KB), Red (>12KB)
- Manual disconnect button for admins

### Step 2.4: Configuration - Admin Settings
**Files:** `AgOpenNtripCaster.Client/src/pages/admin/SystemSettingsPage.tsx`

**Add settings:**
```typescript
interface BacklogSettings {
  maxBacklogBytes: number;        // Default: 16384
  backlogTimeoutSeconds: number;  // Default: 5
  enableAutoDisconnect: boolean;  // Default: true
}
```

**Backend API:**
```csharp
[HttpGet("api/admin/settings/backlog")]
[HttpPut("api/admin/settings/backlog")]
```

### Step 2.5: Testing & Verification
**Test scenarios:**
1. Simulate slow client (throttle network to 1KB/s)
2. Verify client is detected as backlogged within 1 second
3. Verify client is disconnected after 5 seconds
4. Check logs for proper disconnect reason
5. Verify healthy clients are not affected

**Success criteria:**
- ✅ Slow clients detected within 1 second
- ✅ Automatic disconnect after configured timeout
- ✅ No false positives on healthy clients
- ✅ Admin UI shows real-time backlog status

---

## Phase 3: Fine-Grained Locking per Mount Point ⭐

**Expected Impact:** 30% throughput increase, better concurrency
**Complexity:** Medium
**Timeline:** 2 weeks

### Overview
Current implementation uses global locks. Implement per-mount-point RW locks to allow concurrent access to different mount points.

### Step 3.1: Backend - Implement RW Lock Manager
**Files:** `AgOpenNtripCaster.Server/Services/MountPointLockManager.cs`

```csharp
public class MountPointLockManager : IDisposable
{
    private readonly ConcurrentDictionary<string, ReaderWriterLockSlim> _locks = new();
    private readonly ILogger<MountPointLockManager> _logger;

    public IDisposable AcquireReadLock(string mountPoint)
    {
        var rwLock = _locks.GetOrAdd(mountPoint, _ => new ReaderWriterLockSlim());
        rwLock.EnterReadLock();
        return new ReadLockReleaser(rwLock);
    }

    public IDisposable AcquireWriteLock(string mountPoint)
    {
        var rwLock = _locks.GetOrAdd(mountPoint, _ => new ReaderWriterLockSlim());
        rwLock.EnterWriteLock();
        return new WriteLockReleaser(rwLock);
    }

    private class ReadLockReleaser : IDisposable
    {
        private readonly ReaderWriterLockSlim _lock;
        public ReadLockReleaser(ReaderWriterLockSlim rwLock) => _lock = rwLock;
        public void Dispose() => _lock.ExitReadLock();
    }

    private class WriteLockReleaser : IDisposable
    {
        private readonly ReaderWriterLockSlim _lock;
        public WriteLockReleaser(ReaderWriterLockSlim rwLock) => _lock = rwLock;
        public void Dispose() => _lock.ExitWriteLock();
    }
}
```

### Step 3.2: Caster Logic - Apply RW Locks
**Files:** `AgOpenNtripCaster.Server/Services/NtripServerService.cs`

```csharp
// Reading client list (multiple readers OK)
public async Task<List<ClientSession>> GetClientsAsync(string mountPoint)
{
    using (_lockManager.AcquireReadLock(mountPoint))
    {
        return _clientsByMountPoint.GetValueOrDefault(mountPoint)?.ToList() ?? new();
    }
}

// Adding/removing clients (exclusive write)
public async Task AddClientAsync(string mountPoint, ClientSession client)
{
    using (_lockManager.AcquireWriteLock(mountPoint))
    {
        if (!_clientsByMountPoint.ContainsKey(mountPoint))
            _clientsByMountPoint[mountPoint] = new List<ClientSession>();

        _clientsByMountPoint[mountPoint].Add(client);
    }
}
```

### Step 3.3: Database - Lock Contention Metrics
**Files:** `AgOpenNtripCaster.Server/Models/Entities/LockMetric.cs`

```csharp
public class LockMetric
{
    public int Id { get; set; }
    public DateTime Timestamp { get; set; }
    public string MountPoint { get; set; }
    public int ReadLockWaitTimeMs { get; set; }
    public int WriteLockWaitTimeMs { get; set; }
    public int ConcurrentReaders { get; set; }
}
```

### Step 3.4: Admin UI - Lock Performance Dashboard
**Files:** `AgOpenNtripCaster.Client/src/pages/admin/LockPerformancePage.tsx`

**Shows:**
- Lock wait times per mount point
- Concurrent readers/writers graph
- Lock contention hotspots
- Read/write lock ratio

### Step 3.5: Testing & Verification
**Test scenarios:**
1. Concurrent requests to different mount points (should not block)
2. Multiple read operations on same mount point (should run parallel)
3. Write operation blocks all operations on that mount point
4. Measure lock wait times under load

**Success criteria:**
- ✅ Different mount points can be accessed concurrently
- ✅ Multiple readers on same mount point run in parallel
- ✅ Lock wait time < 1ms under normal load
- ✅ No deadlocks after 24h stress test

---

## Phase 4: On-Demand Source Fetching ⭐

**Expected Impact:** 50% reduction in upstream connections, bandwidth savings
**Complexity:** Medium
**Timeline:** 2 weeks

### Overview
Only fetch sources from upstream when clients actually need them. Auto-close idle sources after timeout.

### Step 4.1: Backend - Implement Source Cache with TTL
**Files:** `AgOpenNtripCaster.Server/Services/SourceCacheService.cs`

```csharp
public class SourceCacheService
{
    private readonly MemoryCache _cache;
    private readonly ILogger<SourceCacheService> _logger;

    public async Task<SourceConnection> GetOrFetchSourceAsync(
        string mountPoint,
        Func<Task<SourceConnection>> fetchFunc)
    {
        var cacheKey = $"source:{mountPoint}";

        return await _cache.GetOrCreateAsync(cacheKey, async entry =>
        {
            // Sliding expiration: close after 5 minutes of no client activity
            entry.SlidingExpiration = TimeSpan.FromMinutes(5);
            entry.RegisterPostEvictionCallback(OnSourceEvicted);

            _logger.LogInformation("Fetching source on-demand: {MountPoint}", mountPoint);
            var source = await fetchFunc();
            source.IncrementSubscriberCount();
            return source;
        });
    }

    private void OnSourceEvicted(object key, object value, EvictionReason reason, object state)
    {
        if (value is SourceConnection source && source.SubscriberCount == 0)
        {
            _logger.LogInformation("Closing idle source: {MountPoint}", source.MountPoint);
            _ = source.DisconnectAsync();
        }
    }
}
```

### Step 4.2: Caster Logic - Lazy Source Connection
**Files:** `AgOpenNtripCaster.Server/Services/NtripServerService.cs`

```csharp
private async Task<SourceConnection> ConnectToSourceAsync(string mountPoint)
{
    return await _sourceCacheService.GetOrFetchSourceAsync(mountPoint, async () =>
    {
        var sourceConfig = await _mountPointService.GetSourceConfigAsync(mountPoint);
        if (sourceConfig == null)
        {
            throw new InvalidOperationException($"Source config not found: {mountPoint}");
        }

        return await EstablishSourceConnectionAsync(sourceConfig);
    });
}
```

### Step 4.3: Database - Source Activity Tracking
**Files:** `AgOpenNtripCaster.Server/Models/Entities/SourceActivityLog.cs`

```csharp
public class SourceActivityLog
{
    public int Id { get; set; }
    public string MountPoint { get; set; }
    public DateTime StartedAt { get; set; }
    public DateTime? ClosedAt { get; set; }
    public int TotalSubscribers { get; set; }
    public long BytesReceived { get; set; }
    public SourceCloseReason? CloseReason { get; set; }
}

public enum SourceCloseReason
{
    IdleTimeout,
    Error,
    AdminRequested,
    ConfigurationChange
}
```

### Step 4.4: Admin UI - Source Activity Dashboard
**Files:** `AgOpenNtripCaster.Client/src/pages/admin/SourceActivityPage.tsx`

**Shows:**
- Currently active sources (green indicator)
- Idle sources pending closure (yellow, countdown timer)
- Source fetch/close events timeline
- Bandwidth saved by on-demand fetching

**Features:**
- Manual "Fetch Now" button
- "Keep Alive" button to prevent idle timeout
- Configuration for idle timeout duration

### Step 4.5: Configuration - Source Fetching Settings
**Files:** `AgOpenNtripCaster.Client/src/pages/admin/SystemSettingsPage.tsx`

```typescript
interface SourceFetchingSettings {
  enableOnDemandFetching: boolean;  // Default: true
  idleTimeoutMinutes: number;        // Default: 5
  maxConcurrentSources: number;      // Default: 100
  prefetchPopularSources: boolean;   // Default: false
}
```

### Step 4.6: Testing & Verification
**Test scenarios:**
1. Client connects to mount point → source fetched automatically
2. Last client disconnects → source closes after 5 minutes
3. New client connects before timeout → source stays alive (timer reset)
4. Verify no memory leaks from cache eviction

**Success criteria:**
- ✅ Sources only fetched when needed
- ✅ Idle sources close after configured timeout
- ✅ Source refetched correctly when needed again
- ✅ Bandwidth usage reduced by >40% with mixed workload

---

## Phase 5: Virtual "Near" Base Selection 💡 NEW FEATURE

**Expected Impact:** Huge value-add for mobile users, automatic optimal base selection
**Complexity:** High
**Timeline:** 3-4 weeks

### Overview
Implement automatic nearest base station selection based on client GPS position. When a rover moves, automatically switch to the closest base station.

### Step 5.1: Database - Position and Base Station Schema
**Files:** `AgOpenNtripCaster.Server/Models/Entities/BaseStationPosition.cs`

```csharp
public class BaseStationPosition
{
    public int Id { get; set; }
    public int MountPointId { get; set; }
    public MountPoint MountPoint { get; set; }

    public double Latitude { get; set; }
    public double Longitude { get; set; }
    public double? Altitude { get; set; }

    public DateTime LastUpdated { get; set; }
    public PositionSource Source { get; set; } // RTCM, Manual, NMEA
}

public class ClientPosition
{
    public int Id { get; set; }
    public string ClientSessionId { get; set; }
    public ClientSession ClientSession { get; set; }

    public double Latitude { get; set; }
    public double Longitude { get; set; }
    public double? Altitude { get; set; }

    public DateTime ReceivedAt { get; set; }
    public string? NmeaMessage { get; set; } // Original $GPGGA message
}

public enum PositionSource
{
    RtcmMessage,    // Extracted from RTCM 1005/1006
    NmeaGga,        // From NMEA $GPGGA
    ManualEntry     // Admin configured
}
```

**Migration:**
```bash
dotnet ef migrations add AddPositionTracking
dotnet ef database update
```

### Step 5.2: Backend - NMEA Parser & Position Extraction
**Files:** `AgOpenNtripCaster.Server/Services/PositionService.cs`

```csharp
public class PositionService
{
    // Parse NMEA $GPGGA message
    public ClientPosition? ParseNmeaGga(string nmeaMessage)
    {
        // $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
        if (!nmeaMessage.StartsWith("$GPGGA") && !nmeaMessage.StartsWith("$GNGGA"))
            return null;

        var parts = nmeaMessage.Split(',');
        if (parts.Length < 10) return null;

        var lat = ParseCoordinate(parts[2], parts[3]);
        var lon = ParseCoordinate(parts[4], parts[5]);

        if (lat == null || lon == null) return null;

        return new ClientPosition
        {
            Latitude = lat.Value,
            Longitude = lon.Value,
            Altitude = ParseAltitude(parts[9]),
            ReceivedAt = DateTime.UtcNow,
            NmeaMessage = nmeaMessage
        };
    }

    // Calculate distance between two points (Haversine formula)
    public double CalculateDistanceMeters(double lat1, double lon1, double lat2, double lon2)
    {
        const double R = 6371000; // Earth radius in meters

        var dLat = ToRadians(lat2 - lat1);
        var dLon = ToRadians(lon2 - lon1);

        var a = Math.Sin(dLat / 2) * Math.Sin(dLat / 2) +
                Math.Cos(ToRadians(lat1)) * Math.Cos(ToRadians(lat2)) *
                Math.Sin(dLon / 2) * Math.Sin(dLon / 2);

        var c = 2 * Math.Atan2(Math.Sqrt(a), Math.Sqrt(1 - a));

        return R * c;
    }

    // Find nearest base station
    public async Task<MountPoint?> FindNearestBaseAsync(double lat, double lon)
    {
        var bases = await _dbContext.BaseStationPositions
            .Include(b => b.MountPoint)
            .Where(b => b.MountPoint.IsActive)
            .ToListAsync();

        return bases
            .Select(b => new {
                MountPoint = b.MountPoint,
                Distance = CalculateDistanceMeters(lat, lon, b.Latitude, b.Longitude)
            })
            .OrderBy(x => x.Distance)
            .FirstOrDefault()?.MountPoint;
    }
}
```

### Step 5.3: Backend - Virtual Mount Point Handler
**Files:** `AgOpenNtripCaster.Server/Services/VirtualMountPointService.cs`

```csharp
public class VirtualMountPointService
{
    private const double HysteresisMeters = 500.0; // Don't switch unless >500m closer
    private readonly PositionService _positionService;
    private readonly ILogger<VirtualMountPointService> _logger;

    public async Task<MountPointSwitchResult> CheckAndSwitchIfNeededAsync(
        ClientSession client,
        ClientPosition newPosition)
    {
        // Find nearest base
        var nearestBase = await _positionService.FindNearestBaseAsync(
            newPosition.Latitude,
            newPosition.Longitude);

        if (nearestBase == null)
            return MountPointSwitchResult.NoBaseAvailable();

        // If already on nearest base, no change needed
        if (client.MountPoint == nearestBase.Name)
            return MountPointSwitchResult.AlreadyOptimal();

        // Calculate distances
        var currentDistance = client.LastPosition != null
            ? _positionService.CalculateDistanceMeters(
                newPosition.Latitude, newPosition.Longitude,
                client.LastPosition.Latitude, client.LastPosition.Longitude)
            : double.MaxValue;

        var nearestDistance = _positionService.CalculateDistanceMeters(
            newPosition.Latitude, newPosition.Longitude,
            nearestBase.Position.Latitude, nearestBase.Position.Longitude);

        // Only switch if significantly closer (hysteresis)
        if (nearestDistance < currentDistance - HysteresisMeters)
        {
            _logger.LogInformation(
                "Switching client {ClientId} from {OldBase} to {NewBase} ({Distance}m closer)",
                client.Id, client.MountPoint, nearestBase.Name,
                currentDistance - nearestDistance);

            await SwitchClientMountPointAsync(client, nearestBase);
            return MountPointSwitchResult.Switched(nearestBase.Name);
        }

        return MountPointSwitchResult.NoSwitchNeeded();
    }
}
```

### Step 5.4: Caster Logic - Integrate Position Tracking
**Files:** `AgOpenNtripCaster.Server/Services/NtripServerService.cs`

```csharp
// In HandleClientDataAsync method
private async Task HandleClientDataAsync(ClientSession client, byte[] data)
{
    // Check if data contains NMEA message
    var dataStr = Encoding.ASCII.GetString(data);
    if (dataStr.Contains("$GPGGA") || dataStr.Contains("$GNGGA"))
    {
        var position = _positionService.ParseNmeaGga(dataStr);
        if (position != null)
        {
            position.ClientSessionId = client.Id;
            await _dbContext.ClientPositions.AddAsync(position);
            await _dbContext.SaveChangesAsync();

            // Check if we should switch to a closer base
            if (client.IsVirtualMountPoint)
            {
                var switchResult = await _virtualMountPointService
                    .CheckAndSwitchIfNeededAsync(client, position);

                if (switchResult.Switched)
                {
                    await NotifyClientOfSwitchAsync(client, switchResult.NewMountPoint);
                }
            }
        }
    }
}
```

### Step 5.5: Admin UI - Virtual Base Configuration
**Files:** `AgOpenNtripCaster.Client/src/pages/admin/VirtualBasePage.tsx`

**Features:**
- Enable/disable virtual base feature globally
- Configure hysteresis distance (meters)
- Map view showing:
  - Base station locations (blue markers)
  - Active client positions (green markers with trails)
  - Lines connecting clients to their current base
  - Real-time position updates via SignalR

**Configuration panel:**
```typescript
interface VirtualBaseSettings {
  enabled: boolean;
  hysteresisMeters: number;           // Default: 500
  maxSwitchFrequencySeconds: number;  // Min time between switches
  requiredGpsAccuracy: number;        // Ignore positions with worse accuracy
  logAllSwitches: boolean;
}
```

### Step 5.6: Admin UI - Live Map Component
**Files:** `AgOpenNtripCaster.Client/src/components/VirtualBase/LiveMap.tsx`

```typescript
import { MapContainer, TileLayer, Marker, Popup, Polyline } from 'react-leaflet';

export const LiveMap: React.FC = () => {
  const { bases } = useBaseStations();
  const { clients } = useActiveClients();

  return (
    <MapContainer center={[51.5, 0]} zoom={6}>
      <TileLayer url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png" />

      {/* Base stations */}
      {bases.map(base => (
        <Marker key={base.id} position={[base.latitude, base.longitude]} icon={baseIcon}>
          <Popup>
            <strong>{base.name}</strong><br/>
            Active clients: {base.clientCount}
          </Popup>
        </Marker>
      ))}

      {/* Clients */}
      {clients.map(client => (
        <>
          <Marker key={client.id} position={[client.latitude, client.longitude]} icon={clientIcon}>
            <Popup>
              Client: {client.id}<br/>
              Base: {client.mountPoint}<br/>
              Distance: {client.distanceToBase}m
            </Popup>
          </Marker>

          {/* Line to base */}
          <Polyline
            positions={[
              [client.latitude, client.longitude],
              [client.baseLatitude, client.baseLongitude]
            ]}
            color="blue"
            weight={1}
            opacity={0.5}
          />
        </>
      ))}
    </MapContainer>
  );
};
```

### Step 5.7: Mount Point Configuration - Mark as Virtual
**Files:** `AgOpenNtripCaster.Client/src/pages/admin/MountPointsManagement.tsx`

**Add checkbox:**
- "Enable as Virtual Base" - allows clients to connect to "VIRTUAL" mount point
- When enabled, clients automatically get routed to nearest base

### Step 5.8: Testing & Verification
**Test scenarios:**
1. Client connects to VIRTUAL mount point → assigned to nearest base
2. Client sends NMEA positions while moving → switches to closer base when >500m closer
3. Verify hysteresis prevents rapid switching (ping-pong)
4. Multiple clients at different locations → each gets nearest base
5. Base goes offline → clients switch to next nearest base

**Success criteria:**
- ✅ Position parsing works for all NMEA formats
- ✅ Distance calculations accurate (compare with online calculator)
- ✅ Switching happens smoothly without data loss
- ✅ Hysteresis prevents unnecessary switches
- ✅ Map shows real-time client movements and base assignments
- ✅ Clients automatically failover when base goes offline

---

## Phase 6: Socket Buffer Tuning 💡

**Expected Impact:** 10-20% latency improvement
**Complexity:** Low
**Timeline:** 3 days

### Step 6.1: Backend - Socket Configuration
**Files:** `AgOpenNtripCaster.Server/Services/NtripServerService.cs`

```csharp
private void ConfigureSocket(Socket socket)
{
    // Set send buffer to 112KB (like Millipede)
    socket.SetSocketOption(SocketOptionLevel.Socket,
        SocketOptionName.SendBuffer,
        114688);

    // Set receive buffer
    socket.SetSocketOption(SocketOptionLevel.Socket,
        SocketOptionName.ReceiveBuffer,
        114688);

    // Enable TCP_NODELAY (disable Nagle's algorithm for low latency)
    socket.SetSocketOption(SocketOptionLevel.Tcp,
        SocketOptionName.NoDelay,
        true);

    // Set keep-alive
    socket.SetSocketOption(SocketOptionLevel.Socket,
        SocketOptionName.KeepAlive,
        true);
}
```

### Step 6.2: Configuration - Socket Settings
**Files:** `appsettings.json`

```json
{
  "SocketSettings": {
    "SendBufferSize": 114688,
    "ReceiveBufferSize": 114688,
    "NoDelay": true,
    "KeepAlive": true,
    "LingerTime": 0
  }
}
```

### Step 6.3: Admin UI - Socket Diagnostics
**Files:** `AgOpenNtripCaster.Client/src/pages/admin/SocketDiagnosticsPage.tsx`

**Shows:**
- Current socket buffer settings
- Socket send queue lengths per client
- TCP retransmission rates
- Recommendation engine for optimal buffer sizes

### Step 6.4: Testing & Verification
**Test scenarios:**
1. Measure latency before/after tuning
2. Monitor socket buffer utilization
3. Test with various network conditions (LAN, WAN, mobile)

**Success criteria:**
- ✅ Latency reduced by >10%
- ✅ No socket buffer overflows
- ✅ Settings applied correctly to all sockets

---

## Implementation Priority & Timeline

| Phase | Feature | Priority | Duration | Dependencies |
|-------|---------|----------|----------|--------------|
| 1 | Zero-Copy Distribution | 🔥 Critical | 2-3 weeks | None |
| 2 | Slow Client Detection | 🔥 Critical | 1 week | None |
| 3 | Fine-Grained Locking | ⭐ High | 2 weeks | None |
| 4 | On-Demand Fetching | ⭐ High | 2 weeks | None |
| 5 | Virtual Near Base | 💡 Feature | 3-4 weeks | Phase 4 |
| 6 | Socket Tuning | 💡 Nice-to-have | 3 days | None |

**Total Estimated Timeline:** 10-12 weeks for all phases

---

## Success Metrics

### Performance Targets
- **Memory usage:** 80% reduction with 1000 clients
- **Throughput:** 3-5x increase (from ~200 clients/core to ~1000 clients/core)
- **Latency:** <5ms average client broadcast time
- **Stability:** No crashes under 24h load test with 5000 clients

### Monitoring Dashboards
1. **Performance Dashboard** - zero-copy metrics, broadcast latency
2. **Client Health Dashboard** - backlog detection, disconnection reasons
3. **Lock Performance** - contention analysis, wait times
4. **Source Activity** - on-demand fetching efficiency
5. **Virtual Base Map** - real-time client positions and base assignments

---

## Rollback Strategy

Each phase should be:
1. **Feature-flagged** - can be disabled via configuration
2. **Backward compatible** - old behavior available as fallback
3. **Monitored** - metrics to detect issues quickly
4. **Tested** - comprehensive unit + integration tests

Example feature flags:
```json
{
  "Features": {
    "ZeroCopyEnabled": true,
    "AutoDisconnectSlowClients": true,
    "FineGrainedLocking": true,
    "OnDemandSourceFetching": true,
    "VirtualNearBase": false,
    "SocketBufferTuning": true
  }
}
```

---

## Documentation Requirements

For each phase, document:
1. **API changes** - new endpoints, DTOs
2. **Database migrations** - schema changes
3. **Configuration options** - appsettings.json, admin UI
4. **Monitoring/alerting** - what to watch for
5. **Troubleshooting guide** - common issues and fixes

---

## Questions Before Starting?

Before implementation, clarify:
1. Which phase to start with? (Recommend Phase 1 or 2 first)
2. Database: PostgreSQL or SQL Server specifics?
3. Deployment: Docker/Kubernetes or bare metal?
4. Monitoring: Existing metrics infrastructure (Prometheus, Grafana)?
5. Testing: Performance testing environment available?
