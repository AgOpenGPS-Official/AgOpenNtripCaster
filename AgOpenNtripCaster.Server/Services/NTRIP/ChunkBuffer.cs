using Microsoft.Extensions.Logging;

namespace AgOpenNtripCaster.Server.Services.NTRIP;

/// <summary>
/// Chunk-based broadcast buffer for RTCM streaming
/// Based on BKG NtripCaster architecture for synchronized timing
///
/// Key design:
/// - Source writes to atomic chunks
/// - All clients read from SAME chunk data
/// - Source waits for all clients to consume chunk before advancing
/// - Preserves exact timing and grouping of RTCM messages
/// </summary>
public class ChunkBuffer
{
    private const int ChunkSize = 8192;  // Larger chunks to handle bursts
    private const int NumChunks = 32;    // Ring of 32 chunks

    private readonly Chunk[] _chunks;
    private int _sourceChunkId = 0;
    private readonly object _lock = new();
    private readonly ILogger<ChunkBuffer>? _logger;

    public ChunkBuffer(ILogger<ChunkBuffer>? logger = null)
    {
        _logger = logger;
        _chunks = new Chunk[NumChunks];
        for (int i = 0; i < NumChunks; i++)
        {
            _chunks[i] = new Chunk(ChunkSize);
        }
    }

    private class Chunk
    {
        public byte[] Data { get; }
        public int Length { get; set; }
        public int ClientsLeft { get; set; }

        public Chunk(int size)
        {
            Data = new byte[size];
            Length = 0;
            ClientsLeft = 0;
        }
    }

    /// <summary>
    /// Writes data from source to current chunk
    /// Blocks if previous chunk still has clients reading
    /// </summary>
    public async Task WriteDataAsync(byte[] data, int length, int numClients, CancellationToken cancellationToken)
    {
        if (data == null || length == 0 || numClients == 0)
            return;

        // Wait if current chunk still has clients reading
        while (true)
        {
            lock (_lock)
            {
                var currentChunk = _chunks[_sourceChunkId];
                if (currentChunk.ClientsLeft == 0)
                {
                    break; // Current chunk consumed, can write new data
                }
            }

            // Wait a bit before checking again
            await Task.Delay(1, cancellationToken);
        }

        lock (_lock)
        {
            var chunk = _chunks[_sourceChunkId];

            // Write data to chunk (may need multiple writes if data > ChunkSize)
            int offset = 0;
            while (offset < length)
            {
                int bytesToCopy = Math.Min(length - offset, ChunkSize);
                Array.Copy(data, offset, chunk.Data, 0, bytesToCopy);
                chunk.Length = bytesToCopy;
                chunk.ClientsLeft = numClients;
                _logger?.LogWarning("📝 CHUNK WRITE: ChunkId={ChunkId}, Length={Length}, ClientsLeft={ClientsLeft}",
                    _sourceChunkId, bytesToCopy, numClients);

                // Advance to next chunk if we have more data
                if (offset + bytesToCopy < length)
                {
                    _sourceChunkId = (_sourceChunkId + 1) % NumChunks;
                    chunk = _chunks[_sourceChunkId];
                }

                offset += bytesToCopy;
            }

            // Advance to next chunk for next write
            _sourceChunkId = (_sourceChunkId + 1) % NumChunks;
        }
    }

    /// <summary>
    /// Gets starting position for new client
    /// </summary>
    public ClientChunkPosition GetCurrentPosition()
    {
        lock (_lock)
        {
            return new ClientChunkPosition
            {
                ChunkId = _sourceChunkId,
                Offset = 0
            };
        }
    }

    /// <summary>
    /// Reads data from client's current chunk position
    /// Returns bytes read, or 0 if caught up
    /// </summary>
    public int ReadData(ClientChunkPosition pos, byte[] buffer)
    {
        if (buffer == null || buffer.Length == 0)
            return 0;

        lock (_lock)
        {
            // Check if client is caught up with source
            if (pos.ChunkId == _sourceChunkId)
            {
                return 0; // No new data
            }

            var chunk = _chunks[pos.ChunkId];

            // Check if chunk has any data
            if (chunk.Length == 0)
            {
                return 0;
            }

            // Calculate how much to read from this chunk
            int remainingInChunk = chunk.Length - pos.Offset;
            if (remainingInChunk <= 0)
            {
                return 0; // This chunk is fully consumed
            }

            int bytesToRead = Math.Min(remainingInChunk, buffer.Length);

            if (bytesToRead > 0)
            {
                Array.Copy(chunk.Data, pos.Offset, buffer, 0, bytesToRead);
            }

            return bytesToRead;
        }
    }

    /// <summary>
    /// Client advances to next chunk, decrements clients_left counter
    /// Call this when client has fully consumed current chunk
    /// </summary>
    public void AdvanceChunk(ClientChunkPosition pos)
    {
        lock (_lock)
        {
            var chunk = _chunks[pos.ChunkId];
            var oldClientsLeft = chunk.ClientsLeft;

            // Decrement clients left for this chunk
            if (chunk.ClientsLeft > 0)
            {
                chunk.ClientsLeft--;
            }

            _logger?.LogWarning("⏭️ ADVANCE CHUNK: ChunkId={ChunkId}, ClientsLeft: {Old} → {New}",
                pos.ChunkId, oldClientsLeft, chunk.ClientsLeft);

            // Move to next chunk
            pos.ChunkId = (pos.ChunkId + 1) % NumChunks;
            pos.Offset = 0;
        }
    }

    /// <summary>
    /// Removes a client from the buffer (decrements all chunks they haven't consumed)
    /// Call this when client disconnects
    /// </summary>
    public void RemoveClient(ClientChunkPosition pos)
    {
        lock (_lock)
        {
            // Decrement clients_left for current chunk if client was reading it
            var chunk = _chunks[pos.ChunkId];
            var oldClientsLeft = chunk.ClientsLeft;
            if (chunk.ClientsLeft > 0)
            {
                chunk.ClientsLeft--;
            }
            _logger?.LogWarning("❌ REMOVE CLIENT: ChunkId={ChunkId}, ClientsLeft: {Old} → {New}",
                pos.ChunkId, oldClientsLeft, chunk.ClientsLeft);
        }
    }

    /// <summary>
    /// Registers a new client for all future chunks
    /// Call this when client connects to a mountpoint with existing source
    /// </summary>
    public void AddClient()
    {
        // New clients start at current position, no need to update old chunks
        // They'll naturally start reading from next chunk that gets written
    }
}

/// <summary>
/// Tracks client's position in chunk buffer
/// </summary>
public class ClientChunkPosition
{
    public int ChunkId { get; set; }
    public int Offset { get; set; }

    /// <summary>
    /// Advances offset within current chunk
    /// </summary>
    public void AdvanceOffset(int bytes)
    {
        Offset += bytes;
    }
}
