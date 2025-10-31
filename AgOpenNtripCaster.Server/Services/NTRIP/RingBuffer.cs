namespace AgOpenNtripCaster.Server.Services.NTRIP;

/// <summary>
/// High-performance circular buffer for streaming RTCM data
/// Inspired by original C++ NTRIP Caster implementation
///
/// Key design:
/// - 32 chunks of 100 bytes each = 3.2 KB per mount point
/// - O(1) writes and reads
/// - Automatic backpressure for slow clients
/// </summary>
public class RingBuffer
{
    private const int ChunkSize = 100;
    private const int NumChunks = 32;
    private readonly byte[][] _chunks;
    private int _writeIndex = 0;
    // Use ReaderWriterLockSlim for better concurrency with many readers (clients)
    // Multiple clients can read simultaneously, but writer (source) gets exclusive access
    private readonly ReaderWriterLockSlim _lock = new();

    public RingBuffer()
    {
        _chunks = new byte[NumChunks][];
        for (int i = 0; i < NumChunks; i++)
        {
            _chunks[i] = new byte[ChunkSize];
        }
    }

    /// <summary>
    /// Writes data to the ring buffer
    /// Used by source connections to store RTCM data
    /// </summary>
    public void WriteData(byte[] data)
    {
        if (data == null || data.Length == 0)
            return;

        _lock.EnterWriteLock();
        try
        {
            int offset = 0;
            while (offset < data.Length)
            {
                int bytesToWrite = Math.Min(ChunkSize, data.Length - offset);
                Array.Copy(data, offset, _chunks[_writeIndex], 0, bytesToWrite);

                _writeIndex = (_writeIndex + 1) % NumChunks;
                offset += bytesToWrite;
            }
        }
        finally
        {
            _lock.ExitWriteLock();
        }
    }

    /// <summary>
    /// Gets current write position (for new clients to start reading)
    /// </summary>
    public ClientReadPosition GetCurrentPosition()
    {
        _lock.EnterReadLock();
        try
        {
            return new ClientReadPosition { ChunkId = _writeIndex, Offset = 0 };
        }
        finally
        {
            _lock.ExitReadLock();
        }
    }

    /// <summary>
    /// Reads data from the ring buffer at client's current position
    /// Returns 0 if client is caught up with writer (no new data yet)
    /// </summary>
    public int ReadData(ClientReadPosition pos, byte[] buffer)
    {
        if (buffer == null || buffer.Length == 0)
            return 0;

        _lock.EnterReadLock();
        try
        {
            // Check if client is too far behind
            if (IsClientTooFar(pos))
            {
                return -1; // Signal to disconnect client
            }

            // Don't read if client has caught up to writer (no new data)
            if (pos.ChunkId == _writeIndex)
            {
                return 0;  // No new data available
            }

            // Read from current position
            if (pos.ChunkId >= NumChunks)
                return 0;

            int bytesRead = Math.Min(ChunkSize - pos.Offset, buffer.Length);
            if (bytesRead > 0)
            {
                Array.Copy(_chunks[pos.ChunkId], pos.Offset, buffer, 0, bytesRead);
            }

            return bytesRead;
        }
        finally
        {
            _lock.ExitReadLock();
        }
    }

    /// <summary>
    /// Checks if client is too far behind (automatic backpressure)
    /// </summary>
    public bool IsClientTooFar(ClientReadPosition pos)
    {
        // If client is more than 30 chunks behind, disconnect
        return pos.ChunkId < _writeIndex - NumChunks + 2;
    }
}

/// <summary>
/// Tracks client's position in the ring buffer
/// </summary>
public class ClientReadPosition
{
    public int ChunkId { get; set; }
    public int Offset { get; set; }

    public void Advance(int bytes)
    {
        Offset += bytes;
        if (Offset >= 100)
        {
            ChunkId = (ChunkId + 1) % 32;
            Offset = Offset % 100;
        }
    }
}
