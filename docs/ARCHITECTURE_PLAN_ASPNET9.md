# AgOpen Ntripcaster - ASP.NET 9 Core + React Frontend Architecture Plan

**Decision Date:** 2025-10-28
**Scope:** Complete rewrite from Node.js/C++ to ASP.NET 9 Core + React
**Target:** 10 concurrent NTRIP stations, production-ready, Docker deployment
**Timeline:** 3-4 weeks estimated

---

## 📋 Executive Summary

We're rebuilding the NTRIP Caster management system from scratch using **ASP.NET 9 Core** backend with **React** frontend. This provides:

- ✅ **Built-in user management** (ASP.NET Identity)
- ✅ **Enterprise-grade architecture** (DI, middleware, EF Core)
- ✅ **Superior performance** for NTRIP streaming (async/await, thread pool)
- ✅ **Production-ready deployment** (Docker + Nginx + Certbot)
- ✅ **Reference from AgShare** (proven deployment pattern)
- ✅ **Lessons from original C++ code** (ring buffers, authentication hierarchy)

---

## 🏗️ Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                      NTRIP CASTER SYSTEM                    │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌──────────────────┐         ┌──────────────────┐          │
│  │  React Frontend  │         │ ASP.NET 9 Backend│          │
│  │  (Port 3000)     │◄───────►│ (Port 5000)      │          │
│  │                  │         │                  │          │
│  │ • Dashboard      │         │ • NTRIP Server   │          │
│  │ • User Mgmt      │         │ • Auth System    │          │
│  │ • Config UI      │         │ • REST APIs      │          │
│  │ • Statistics     │         │ • WebSocket      │          │
│  └──────────────────┘         └──────────────────┘          │
│                                       │                      │
│                                       │                      │
│                              ┌────────▼────────┐             │
│                              │   PostgreSQL    │             │
│                              │   (Port 5432)   │             │
│                              │                 │             │
│                              │ • Users         │             │
│                              │ • Groups        │             │
│                              │ • Mount Points  │             │
│                              │ • Statistics    │             │
│                              └─────────────────┘             │
│                                                               │
│                    NTRIP CLIENTS ────────────┐               │
│               (TCP/RTSP/HTTP connections)    │               │
│                                              ▼               │
│                                    NTRIP Server Stream       │
│                                    (Async TCP Handlers)      │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

---

## 📁 Project Structure

```
ntripcaster/
├── AgOpenNtripCaster.Server/           # ASP.NET 9 Backend
│   ├── Controllers/              # API Endpoints
│   │   ├── AuthController.cs
│   │   ├── SourcesController.cs
│   │   ├── ClientsController.cs
│   │   ├── StatisticsController.cs
│   │   └── AdminController.cs
│   ├── Models/                   # Data Models
│   │   ├── Entities/             # EF Core entities
│   │   ├── DTOs/                 # Request/Response DTOs
│   │   └── Constants.cs
│   ├── Services/                 # Business Logic
│   │   ├── NtripServer/          # Core NTRIP implementation
│   │   │   ├── NtripServerService.cs
│   │   │   ├── NtripClientHandler.cs
│   │   │   ├── SourceHandler.cs
│   │   │   ├── RingBuffer.cs
│   │   │   └── ConnectionPool.cs
│   │   ├── Auth/
│   │   │   ├── UserService.cs
│   │   │   ├── AuthenticationService.cs
│   │   │   └── PermissionService.cs
│   │   ├── Statistics/
│   │   │   └── StatisticsService.cs
│   │   └── ConfigurationService.cs
│   ├── Middleware/               # Custom middleware
│   ├── Data/                     # EF Core DbContext
│   │   ├── ApplicationDbContext.cs
│   │   └── Migrations/
│   ├── Program.cs               # Dependency injection, middleware setup
│   ├── appsettings.json
│   ├── appsettings.Development.json
│   ├── Dockerfile
│   └── AgOpenNtripCaster.Server.csproj
│
├── AgOpenNtripCaster.Client/           # React Frontend
│   ├── src/
│   │   ├── components/
│   │   │   ├── Dashboard.tsx
│   │   │   ├── SourcesManagement.tsx
│   │   │   ├── ClientsMonitor.tsx
│   │   │   ├── StatisticsView.tsx
│   │   │   └── UserManagement.tsx
│   │   ├── pages/
│   │   ├── services/
│   │   │   ├── api.ts            # Axios instance
│   │   │   └── ntrip.ts          # NTRIP API calls
│   │   ├── hooks/
│   │   ├── types/
│   │   ├── App.tsx
│   │   └── main.tsx
│   ├── Dockerfile
│   ├── package.json
│   └── vite.config.ts
│
├── docker-compose.yml            # Production stack
├── nginx/
│   └── default.conf             # Reverse proxy config
├── certbot/                      # SSL certificate setup
│   └── init.sh
├── ARCHITECTURE_PLAN_ASPNET9.md  # This file
├── NTRIP_PROTOCOL.md            # Protocol documentation
└── DEPLOYMENT.md                # Production deployment guide
```

---

## 🔑 Core Components Breakdown

### 1. **NTRIP Server Core** (`NtripServerService`)

Handles the actual NTRIP protocol implementation:

```csharp
public class NtripServerService : IHostedService
{
    private TcpListener _tcpListener;
    private CancellationTokenSource _cancellationTokenSource;
    private ConnectionPool _connectionPool;
    private ConcurrentDictionary<string, RingBuffer> _mountPointBuffers;

    // Start listening for NTRIP clients
    public async Task StartAsync(CancellationToken cancellationToken)
    {
        _tcpListener = new TcpListener(IPAddress.Any, 2101);
        _tcpListener.Start();
        // Accept connections in background
        AcceptConnections(_cancellationTokenSource.Token);
    }

    // Handle each client connection
    private async Task HandleClientAsync(TcpClient client, CancellationToken ct)
    {
        var handler = new NtripClientHandler(client, _connectionPool);
        await handler.ProcessAsync(ct);
    }
}
```

### 2. **Ring Buffer System** (`RingBuffer<T>`)

Fast, lock-free circular buffer for streaming RTCM data:

```csharp
public class RingBuffer
{
    private readonly byte[][] _chunks;        // Circular array of chunks
    private int _writeIndex;                  // Current write position
    private object _writeLock = new();

    // Source writes RTCM data
    public void WriteData(byte[] data)
    {
        lock (_writeLock)
        {
            // Append to current chunk or create new chunk
            // When buffer fills, old data is automatically overwritten
        }
    }

    // Client reads from ring buffer
    public ClientReadPosition ReadData(ClientReadPosition pos, byte[] buffer)
    {
        // Client tracks: (chunkId, offset)
        // Reads efficiently without allocation
    }

    // If client falls behind, automatically kick from connection
    public bool IsClientTooFar(ClientReadPosition pos)
    {
        return pos.ChunkId < _writeIndex - BufferSize;
    }
}
```

**Key insight from C++ code:**
- 32 chunks × 100 bytes = 3.2 KB per source
- O(1) writes, O(1) reads
- Automatic backpressure (slow clients get disconnected)

### 3. **Connection Pool** (`ConnectionPool`)

Manages concurrent client connections efficiently:

```csharp
public class ConnectionPool
{
    private ConcurrentDictionary<string, NtripClientHandler> _clients;
    private readonly int _maxClients = 1000; // ASP.NET can handle this

    public void RegisterClient(string id, NtripClientHandler handler)
    {
        if (_clients.Count >= _maxClients)
            throw new InvalidOperationException("Max clients exceeded");
        _clients.TryAdd(id, handler);
    }

    public void UnregisterClient(string id)
    {
        _clients.TryRemove(id, out _);
    }
}
```

### 4. **Two-Tier Authentication System**

#### A) SOURCE Authentication (GNSS Stations → Push data)
Sources authenticate with mount point password:
```
GNSS Station → "SOURCE STATION_A:sourcePassword123 HTTP/1.1"
Server validates: Does STATION_A exist with this password?
→ Access granted, source streams RTCM data
```

#### B) CLIENT Authentication (Users → Pull data)
Clients authenticate with user credentials:
```
User → "GET /STATION_A HTTP/1.1"
        "Authorization: Basic username:password"
Server validates:
  1. User exists and password correct? ✓
  2. User in group with access to STATION_A? ✓
→ Access granted, client receives RTCM stream
```

**Models:**
```csharp
// Mount Point (NTRIP station)
public class MountPoint
{
    public int Id { get; set; }
    public string Name { get; set; }                    // "STATION_A"
    public string SourcePassword { get; set; }         // Source auth password
    public string Description { get; set; }
    public int Latitude { get; set; }                  // For sourcetable
    public int Longitude { get; set; }
    public string Format { get; set; }                 // "RTCM3"
    public bool RequireClientAuthentication { get; set; } = true;
    public bool IsActive { get; set; } = true;
    public ICollection<NtripGroup> AllowedGroups { get; set; }
}

// User (Frontend login + NTRIP client credentials)
public class NtripUser : IdentityUser
{
    public string FullName { get; set; }
    public ICollection<NtripGroup> Groups { get; set; }
}

// Group (Permission hierarchy)
public class NtripGroup
{
    public int Id { get; set; }
    public string Name { get; set; }
    public ICollection<NtripUser> Users { get; set; }
    public ICollection<MountPoint> MountPoints { get; set; }
}
```

**Authentication Flows:**
1. Source: Mount point name + source password
2. Client: Username + password (same as frontend login) + group membership

### 5. **Client Handler** (`NtripClientHandler`)

Handles individual NTRIP client connections with bidirectional position tracking:

```csharp
public class NtripClientHandler : IAsyncDisposable
{
    private readonly TcpClient _tcpClient;
    private readonly NetworkStream _stream;
    private readonly RingBuffer _sourceBuffer;
    private readonly IHubContext<NtripHub> _hubContext;  // WebSocket broadcast
    private ClientReadPosition _readPos;

    // Position tracking
    private PositionFrame _lastPosition;
    private DateTime _lastPositionTime = DateTime.MinValue;
    private const int MAX_POSITION_AGE_SEC = 15;
    private bool _isStreaming = false;

    public async Task ProcessAsync(CancellationToken ct)
    {
        try
        {
            // 1. Read NTRIP request (HTTP-like format)
            var request = await ReadNtripRequestAsync(ct);

            // 2. Authenticate (check User/Group/MountPoint)
            var permitted = await _authService.AuthorizeAsync(request, ct);
            if (!permitted)
                await SendResponseAsync("401 Unauthorized", ct);

            // 3. Send response header
            await SendResponseAsync("200 OK\r\n\r\n", ct);

            // 4. Start bidirectional streaming:
            //    - Task 1: Listen for position updates from client
            //    - Task 2: Stream RTCM data, controlling based on position freshness
            var positionReader = ReadPositionDataAsync(ct);
            var dataStreamer = StreamRtcmDataAsync(request.Username, request.MountPoint, ct);

            await Task.WhenAll(positionReader, dataStreamer);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Client handler error");
        }
        finally
        {
            await DisposeAsync();
        }
    }

    // Task 1: Listen for position updates from client
    // Client sends: "POS|52.3|5.1|2.5\n" every 10 seconds
    private async Task ReadPositionDataAsync(CancellationToken ct)
    {
        var reader = new StreamReader(_stream);
        while (!ct.IsCancellationRequested)
        {
            try
            {
                string posLine = await reader.ReadLineAsync(ct);

                if (posLine?.StartsWith("POS|") == true)
                {
                    var parts = posLine.Split('|');
                    _lastPosition = new PositionFrame
                    {
                        Latitude = double.Parse(parts[1]),
                        Longitude = double.Parse(parts[2]),
                        Accuracy = double.Parse(parts[3]),
                        Timestamp = DateTime.UtcNow
                    };
                    _lastPositionTime = DateTime.UtcNow;

                    // Broadcast to frontend (WebSocket)
                    await _hubContext.Clients.All.SendAsync(
                        "ClientPositionUpdate",
                        new
                        {
                            ClientId = _clientId,
                            Username = _username,
                            Lat = _lastPosition.Latitude,
                            Lng = _lastPosition.Longitude,
                            Accuracy = _lastPosition.Accuracy,
                            Timestamp = _lastPosition.Timestamp
                        },
                        cancellationToken: ct
                    );

                    _logger.LogInformation(
                        $"Client {_username} position: {_lastPosition.Latitude}, {_lastPosition.Longitude}");
                }
                else
                {
                    await Task.Delay(100, ct);
                }
            }
            catch (IOException)
            {
                // Client disconnected
                break;
            }
        }
    }

    // Task 2: Stream RTCM data to client
    // KEY LOGIC: Only stream if position is fresh (<15 sec)
    private async Task StreamRtcmDataAsync(string username, string mountPoint, CancellationToken ct)
    {
        var buffer = new byte[4096];
        var writer = new StreamWriter(_stream);

        while (!ct.IsCancellationRequested)
        {
            // CHECK: Has position been received recently?
            var timeSinceLastPos = DateTime.UtcNow - _lastPositionTime;
            bool shouldStream = timeSinceLastPos.TotalSeconds <= MAX_POSITION_AGE_SEC;

            if (!shouldStream)
            {
                // HOLD data stream - position too old
                if (_isStreaming)
                {
                    _isStreaming = false;
                    _logger.LogWarning(
                        $"Client {username} position too old ({timeSinceLastPos.TotalSeconds:F1}s). Pausing stream.");

                    // Notify frontend
                    await _hubContext.Clients.All.SendAsync(
                        "ClientStreamStatus",
                        new
                        {
                            ClientId = _clientId,
                            Status = "paused",
                            Reason = "No position data"
                        },
                        cancellationToken: ct
                    );
                }

                await Task.Delay(1000, ct); // Check again in 1 sec
                continue;
            }

            // RESUME: Position is fresh, start/continue streaming
            if (!_isStreaming)
            {
                _isStreaming = true;
                _logger.LogInformation($"Client {username} stream RESUMED");

                // Notify frontend
                await _hubContext.Clients.All.SendAsync(
                    "ClientStreamStatus",
                    new
                    {
                        ClientId = _clientId,
                        Status = "streaming",
                        Reason = "Position received"
                    },
                    cancellationToken: ct
                );
            }

            // Read RTCM from ring buffer
            int bytesRead = _sourceBuffer.ReadData(_readPos, buffer);

            if (bytesRead > 0)
            {
                // Send RTCM data
                await _stream.WriteAsync(buffer, 0, bytesRead, ct);
                _readPos.Advance(bytesRead);
            }
            else
            {
                await Task.Delay(10, ct); // Back off if no data
            }
        }
    }
}
```

**Key Features:**
- ✅ Bidirectional communication (client sends position, server sends RTCM)
- ✅ Position parsing from "POS|lat|lon|accuracy" format
- ✅ Automatic stream pause if position >15 sec old
- ✅ Automatic stream resume when fresh position arrives
- ✅ Real-time WebSocket broadcast to frontend
- ✅ Stream status tracking (streaming/paused)

### 6. **REST API Controllers**

Expose NTRIP management through REST:

```csharp
[ApiController]
[Route("api/[controller]")]
[Authorize]
public class SourcesController : ControllerBase
{
    [HttpGet]
    public async Task<ActionResult<IEnumerable<SourceDto>>> GetSources()
    {
        var sources = await _db.Sources.ToListAsync();
        return Ok(_mapper.Map<IEnumerable<SourceDto>>(sources));
    }

    [HttpPost]
    [Authorize(Roles = "Admin")]
    public async Task<ActionResult> CreateSource(CreateSourceDto dto)
    {
        var source = new Source { Name = dto.Name, ... };
        _db.Sources.Add(source);
        await _db.SaveChangesAsync();
        return CreatedAtAction(nameof(GetSource), new { id = source.Id }, source);
    }
}
```

---

## 🗄️ Database Schema (EF Core)

```csharp
// Users and authentication
public class NtripUser : IdentityUser
{
    public string FullName { get; set; }
    public DateTime CreatedAt { get; set; }
    public int MaxConnections { get; set; }
    public ICollection<NtripGroup> Groups { get; set; }
}

// User groups (3-tier auth: Mount → Group → User)
public class NtripGroup
{
    public int Id { get; set; }
    public string Name { get; set; }
    public string Description { get; set; }
    public ICollection<NtripUser> Users { get; set; }
    public ICollection<MountPoint> MountPoints { get; set; }
}

// NTRIP mount points
public class MountPoint
{
    public int Id { get; set; }
    public string Name { get; set; }
    public string Identifier { get; set; }      // e.g., "STATION_A"
    public string Source { get; set; }          // Source IP/hostname
    public int Port { get; set; }               // Source port
    public string Format { get; set; }          // RTCM3, etc.
    public bool RequireAuthentication { get; set; }
    public int MaxClients { get; set; }
    public int CurrentClients { get; set; }
    public bool IsActive { get; set; }
    public ICollection<NtripGroup> AllowedGroups { get; set; }
}

// Active client session with position tracking
public class ClientSession
{
    public string Id { get; set; }
    public string UserId { get; set; }
    public string MountPointName { get; set; }
    public DateTime ConnectedAt { get; set; }
    public DateTime? DisconnectedAt { get; set; }

    // Position tracking (location-based corrections)
    public double? LastLatitude { get; set; }
    public double? LastLongitude { get; set; }
    public double? LastAccuracy { get; set; }
    public DateTime? LastPositionAt { get; set; }

    // Stream status
    public ClientStreamStatus Status { get; set; }  // Streaming/Paused/Disconnected
    public DateTime? LastStreamPauseAt { get; set; }

    // Statistics
    public long BytesReceived { get; set; }
    public long BytesSent { get; set; }
}

public enum ClientStreamStatus
{
    Connected,      // Just connected, awaiting position
    Streaming,      // Position fresh, actively streaming RTCM
    Paused,         // Position too old (>15 sec), stream on hold
    Disconnected    // Connection closed
}

// Source (GNSS station) connections
public class SourceConnection
{
    public int Id { get; set; }
    public int MountPointId { get; set; }
    public DateTime ConnectedAt { get; set; }
    public DateTime? DisconnectedAt { get; set; }
    public long BytesReceived { get; set; }
    public long BytesSent { get; set; }
    public SourceConnectionStatus Status { get; set; }
}

public enum SourceConnectionStatus
{
    Connected,
    Streaming,      // Actively receiving RTCM from source
    Disconnected
}
```

---

## 🔐 Authentication & Connection Flow

### 1. Frontend Login
```
User → POST /api/auth/login { username, password }
Server checks: User exists + password correct?
→ Returns JWT token + RefreshToken
→ Frontend stores tokens (localStorage)
```

### 2. Source Connection (GNSS Station)
```
GNSS Station → TCP :2101
             → "SOURCE STATION_A:sourcePassword123 HTTP/1.1"
Server checks: Mount point exists with this password?
→ Access granted, source begins streaming RTCM data
→ Data stored in RingBuffer[STATION_A]
```

### 3. Client Connection (End User)
```
NTRIP Client → TCP :2101
            → "GET /STATION_A HTTP/1.1"
            → "Authorization: Basic username:password"
Server checks:
  1. User exists + password correct? ✓
  2. User in group with access to STATION_A? ✓
  3. Mount point is active? ✓
→ Access granted, client receives RTCM stream
```

### 4. Bidirectional Position & Stream Control
```
NTRIP Client → (every 10 sec) "POS|52.3|5.1|2.5"
                              ↓
                        Server parses position
                              ↓
Server checks: Position fresh (<15 sec)?
  → YES: Stream RTCM data
  → NO: Pause stream (wait for new position)
                              ↓
Frontend (via WebSocket) ← Position update + Stream status
                              ↓
React Map updates in real-time
```

---

## 📍 Real-time Position Tracking & Broadcasting

The system uses **SignalR (WebSocket)** to broadcast client positions to the frontend in real-time:

### Server Hub (NtripHub.cs)
```csharp
public class NtripHub : Hub
{
    // Called when position update is received from client
    public async Task OnClientPositionUpdate(ClientPositionUpdate update)
    {
        // Broadcast to all connected clients (admin dashboard viewers)
        await Clients.All.SendAsync("ClientPositionUpdated", update);
    }

    // Called when stream status changes
    public async Task OnClientStreamStatus(ClientStreamStatusUpdate update)
    {
        // Notify frontend: "Client X now streaming" or "Client X paused"
        await Clients.All.SendAsync("ClientStreamStatusChanged", update);
    }

    // Called when client connects
    public override async Task OnConnectedAsync()
    {
        await Clients.All.SendAsync("ClientConnected", new { connectionId = Context.ConnectionId });
        await base.OnConnectedAsync();
    }

    // Called when client disconnects
    public override async Task OnDisconnectedAsync(Exception exception)
    {
        await Clients.All.SendAsync("ClientDisconnected", new { connectionId = Context.ConnectionId });
        await base.OnDisconnectedAsync(exception);
    }
}
```

### DTOs
```csharp
public class PositionFrame
{
    public double Latitude { get; set; }
    public double Longitude { get; set; }
    public double? Altitude { get; set; }
    public double? Accuracy { get; set; }
    public DateTime Timestamp { get; set; }
}

public class ClientPositionUpdate
{
    public string ClientId { get; set; }
    public string Username { get; set; }
    public string MountPoint { get; set; }
    public double Latitude { get; set; }
    public double Longitude { get; set; }
    public double? Accuracy { get; set; }
    public DateTime Timestamp { get; set; }
}

public class ClientStreamStatusUpdate
{
    public string ClientId { get; set; }
    public string Username { get; set; }
    public ClientStreamStatus Status { get; set; }    // Streaming/Paused/Connected
    public string Reason { get; set; }                // "Position fresh" or "Position too old"
    public DateTime UpdatedAt { get; set; }
}
```

### Frontend React Hook (useNtripClients.ts)
```typescript
import { useEffect, useState } from 'react';
import { HubConnectionBuilder } from '@microsoft/signalr';

interface ClientLocation {
  clientId: string;
  username: string;
  mountPoint: string;
  lat: number;
  lng: number;
  accuracy?: number;
  status: 'streaming' | 'paused' | 'connected' | 'disconnected';
  lastUpdate: Date;
  lastPositionAt: Date;
}

export function useNtripClients() {
  const [clients, setClients] = useState<Map<string, ClientLocation>>(new Map());

  useEffect(() => {
    const connection = new HubConnectionBuilder()
      .withUrl('/api/ntrip-hub')
      .withAutomaticReconnect()
      .build();

    connection.on('ClientPositionUpdated', (update: ClientPositionUpdate) => {
      setClients(prev => {
        const updated = new Map(prev);
        const existing = updated.get(update.clientId) || {};
        updated.set(update.clientId, {
          ...existing,
          clientId: update.clientId,
          username: update.username,
          mountPoint: update.mountPoint,
          lat: update.latitude,
          lng: update.longitude,
          accuracy: update.accuracy,
          lastUpdate: new Date(),
          lastPositionAt: new Date(update.timestamp),
        });
        return updated;
      });
    });

    connection.on('ClientStreamStatusChanged', (update: ClientStreamStatusUpdate) => {
      setClients(prev => {
        const updated = new Map(prev);
        const client = updated.get(update.clientId);
        if (client) {
          client.status = update.status;
          client.lastUpdate = new Date(update.updatedAt);
        }
        return updated;
      });
    });

    connection.on('ClientConnected', () => {
      // Client connected to WebSocket
      console.log('Client connected to WebSocket');
    });

    connection.on('ClientDisconnected', ({ connectionId }: { connectionId: string }) => {
      // Remove client from map
      setClients(prev => {
        const updated = new Map(prev);
        Array.from(updated.entries()).forEach(([key, value]) => {
          if (value.clientId === connectionId) {
            updated.delete(key);
          }
        });
        return updated;
      });
    });

    connection.start().catch(err => console.error(err));

    return () => {
      connection.stop();
    };
  }, []);

  return { clients };
}
```

### Frontend Map Component (NtripClientsMap.tsx)
```typescript
import { MapContainer, TileLayer, Marker, CircleMarker, Popup } from 'react-leaflet';
import L from 'leaflet';
import { useNtripClients } from '../hooks/useNtripClients';

export function NtripClientsMap() {
  const { clients } = useNtripClients();

  return (
    <MapContainer center={[52.1, 5.2]} zoom={8} style={{ height: '600px' }}>
      <TileLayer
        url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
        attribution="&copy; OpenStreetMap"
      />

      {Array.from(clients.values()).map(client => {
        const statusColor =
          client.status === 'streaming' ? 'green' :
          client.status === 'paused' ? 'orange' :
          'gray';

        return (
          <div key={client.clientId}>
            {/* Marker for client location */}
            <Marker
              position={[client.lat, client.lng]}
              icon={L.icon({
                iconUrl: `data:image/svg+xml,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="${statusColor}"><circle cx="12" cy="12" r="8"/></svg>`,
              })}
            >
              <Popup>
                <div>
                  <strong>{client.username}</strong>
                  <p>Mount: {client.mountPoint}</p>
                  <p>Status: <span style={{ color: statusColor }}>
                    {client.status === 'streaming' ? '🟢 Streaming' :
                     client.status === 'paused' ? '🟡 Paused' : '⚫ Idle'}
                  </span></p>
                  <p>Accuracy: {client.accuracy?.toFixed(2)}m</p>
                  <p>Last update: {client.lastUpdate.toLocaleTimeString()}</p>
                </div>
              </Popup>
            </Marker>

            {/* Accuracy circle (if available) */}
            {client.accuracy && (
              <CircleMarker
                center={[client.lat, client.lng]}
                radius={Math.min(client.accuracy / 1000, 100)} // Scale appropriately
                fillOpacity={0.1}
                color={statusColor}
              />
            )}
          </div>
        );
      })}
    </MapContainer>
  );
}
```

### Program.cs Configuration
```csharp
// In Program.cs:
builder.Services.AddSignalR();
app.MapHub<NtripHub>("/api/ntrip-hub");
```

---

## 🚀 Development Phases

### Phase 0: Setup (3-4 days)
- [ ] Create ASP.NET 9 Core project (using template)
- [ ] Setup EF Core with PostgreSQL
- [ ] Configure dependency injection
- [ ] Setup logging and error handling
- [ ] Create React project structure (Vite + TypeScript)

### Phase 1: Core NTRIP Server (4-5 days)
- [ ] Implement `RingBuffer<byte>` class
- [ ] Implement `ConnectionPool` class
- [ ] Implement `TcpListener` in `NtripServerService`
- [ ] Implement `NtripClientHandler` for basic NTRIP protocol
- [ ] Test with NTRIP client simulator (ntripclient)
- [ ] Handle graceful disconnection

### Phase 2: Authentication System (2-3 days)
- [ ] Setup ASP.NET Identity with JWT
- [ ] Implement 3-tier permission hierarchy
- [ ] Create `PermissionService` for authorization checks
- [ ] Test user/group/mountpoint permissions

### Phase 3: NTRIP Source Management (2 days)
- [ ] Implement `SourceHandler` (connects to upstream GNSS source)
- [ ] Implement data relay from source → ring buffer
- [ ] Handle source reconnection logic
- [ ] Source health monitoring

### Phase 4: REST APIs (2-3 days)
- [ ] Users management endpoints
- [ ] Sources/MountPoints CRUD
- [ ] Statistics endpoints
- [ ] Configuration endpoints

### Phase 5: React Frontend (4-5 days)
- [ ] Login/Register pages
- [ ] Dashboard (real-time stats)
- [ ] Source management
- [ ] User/Group management
- [ ] Client connections monitor
- [ ] WebSocket integration for real-time updates

### Phase 6: Docker Deployment (3-4 days)
- [ ] ASP.NET 9 Dockerfile (multi-stage)
- [ ] React frontend Dockerfile (Node + Nginx)
- [ ] PostgreSQL setup
- [ ] Nginx reverse proxy config
- [ ] Certbot SSL setup
- [ ] docker-compose.yml
- [ ] Environment configuration

### Phase 7: Testing & Hardening (2-3 days)
- [ ] Load testing (concurrent connections)
- [ ] Authentication security tests
- [ ] Performance profiling
- [ ] Documentation

**Total: 3-4 weeks**

---

## 🔄 Key Design Decisions

### 1. Why ASP.NET 9 Core?
- ✅ Superior performance for I/O-bound operations (async/await)
- ✅ Built-in dependency injection
- ✅ Entity Framework Core (mature ORM)
- ✅ Built-in user management (ASP.NET Identity)
- ✅ Proven in production (AgShare uses it)

### 2. Why Ring Buffer?
- ✅ Constant O(1) writes from GNSS source
- ✅ Constant O(1) reads from multiple clients
- ✅ No allocation/GC pressure
- ✅ Automatic backpressure (slow clients kicked)
- ✅ Proven in original C++ code

### 3. Why 3-Tier Authentication?
- ✅ Flexible permission model
- ✅ Support multiple users per group
- ✅ Support multiple groups per mount point
- ✅ Proven in original C++ code

### 4. Why PostgreSQL?
- ✅ Mature, stable, open source
- ✅ Excellent .NET support via EF Core
- ✅ Docker-friendly
- ✅ AgShare uses it (proven pattern)

### 5. Bidirectional Position Tracking?
- ✅ Ensures accurate location-based corrections
- ✅ Automatic stream control (pause if no position)
- ✅ Real-time map visualization on frontend
- ✅ Security: No corrections without valid position
- ✅ Prevents abuse: User must send position to receive data

---

## 🔄 Complete Data Flow Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                    NTRIP CASTER SYSTEM                          │
│                         (Port 2101)                             │
└─────────────────────────────────────────────────────────────────┘

LAYER 1: SOURCES (Pushing RTCM Data)
┌──────────────────┐
│ GNSS Station #1  │
│  (Base Station)  │
└────────┬─────────┘
         │
         │ AUTH: SOURCE STATION_A:password
         │
         ▼
    ┌─────────────────────────┐
    │ NtripServerService      │
    │ :2101 TCP Listener      │
    │                         │
    │ Authenticates source    │
    └────────┬────────────────┘
             │
             ▼
    ┌─────────────────────────┐
    │ RingBuffer[STATION_A]   │
    │ (32 chunks × 100 bytes) │
    │ = 3.2 KB buffer         │
    └────────┬────────────────┘
             │
             │ RTCM data stored here
             │


LAYER 2: CLIENTS (Pulling RTCM + Sending Position)
┌──────────────────────────────┐
│ NTRIP Client Application     │
│ (Mobile/Desktop)             │
│                              │
│ 1. Connect to :2101          │
│ 2. AUTH: GET /STATION_A      │
│    Authorization: Basic ...  │
│ 3. Every 10 sec:             │
│    Send: POS|52.3|5.1|2.5    │
│ 4. Receive: RTCM stream      │
└──────────┬───────────────────┘
           │
           │ TCP Connection
           │
           ▼
    ┌──────────────────────────────┐
    │ NtripClientHandler           │
    │ (Per-client async handler)   │
    │                              │
    │ Task 1: ReadPositionData     │
    │   Parse: "POS|lat|lon|acc"   │
    │   Update: _lastPosition      │
    │   Broadcast: to WebSocket    │
    │                              │
    │ Task 2: StreamRtcmData       │
    │   Check: Position fresh?     │
    │   IF <15 sec:                │
    │     Read from RingBuffer     │
    │     Send to client           │
    │   ELSE:                       │
    │     PAUSE stream             │
    │     Wait for new position    │
    └────────┬─────────────────────┘
             │
             │ Position updates
             │
             ▼
    ┌──────────────────────────────┐
    │ SignalR Hub (NtripHub)       │
    │ WebSocket broadcasts:        │
    │ - ClientPositionUpdate       │
    │ - ClientStreamStatusUpdate   │
    └────────┬─────────────────────┘
             │
             │ WebSocket
             │
             ▼
┌─────────────────────────────────┐
│ React Frontend Dashboard        │
│                                 │
│ useNtripClients() Hook         │
│   Listens to WebSocket events   │
│                                 │
│ NtripClientsMap Component       │
│   Displays:                     │
│   • Client markers (colored)    │
│   • Current status (streaming/  │
│     paused)                     │
│   • Accuracy circles            │
│   • Real-time updates           │
└─────────────────────────────────┘


LAYER 3: DATA STORAGE
┌──────────────────────────────┐
│ PostgreSQL Database          │
│                              │
│ Tables:                      │
│ • NtripUser (login creds)    │
│ • NtripGroup (permissions)   │
│ • MountPoint (STATION_A)     │
│ • ClientSession              │
│   - LastLatitude/Longitude   │
│   - LastPositionAt           │
│   - Status (Streaming/Paused)│
│ • SourceConnection           │
│ • Statistics                 │
└──────────────────────────────┘
```

---

## 📊 Performance Targets

| Metric | Target | ASP.NET 9 Capability |
|--------|--------|---------------------|
| Concurrent Clients | 1000+ | ✅ Easily |
| Concurrent Stations | 10 | ✅ Trivial |
| Latency per client | <50ms | ✅ Achievable |
| Data throughput | 100 Mbps | ✅ No problem |
| Memory per client | <10KB | ✅ Ring buffer design |
| Max response time | <10s | ✅ Typical |

---

## 🔗 Integration Points

### With Original C++ Code
- Study `src/ntrip.h` for protocol handling ✅
- Study `src/authenticate/` for permission model ✅
- Study ring buffer concepts → implement in C# ✅

### With AgShare
- Docker setup pattern ✅
- ASP.NET 9 project structure ✅
- Nginx + Certbot configuration ✅
- Environment variable management ✅

---

## 📝 Next Steps

1. ✅ **Review this architecture** - Does it make sense?
2. ✅ **Approve or modify** - Any changes needed?
3. → **Phase 0: Create project** - Start building
4. → **Phase 1: NTRIP Server** - Core functionality
5. → **Phase 2+: Iterate** - Build out remaining features

---

## 📚 References

- Original NTRIP code: `ntripcaster_org/src/`
- AgShare reference: `../AgShare/` (deployment pattern)
- NTRIP Analysis: `NTRIP_CASTER_ARCHITECTURE.md`

