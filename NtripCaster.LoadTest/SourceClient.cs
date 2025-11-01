using System.Net.Sockets;
using System.Text;

namespace NtripCaster.LoadTest;

/// <summary>
/// Simulates a GNSS base station connecting to the NTRIP server as a source
/// and streaming RTCM correction data
/// </summary>
public class SourceClient
{
    private readonly string _serverHost;
    private readonly int _serverPort;
    private readonly string _sourceId;
    private readonly string _sourcePassword;
    private readonly int _rtcmIntervalMs; // Interval between RTCM frame sends

    private TcpClient? _tcpClient;
    private NetworkStream? _stream;
    private CancellationTokenSource? _cancellationTokenSource;
    private Task? _sendTask;

    public string SourceId { get; }
    public bool IsConnected { get; private set; }
    public DateTime ConnectedAt { get; private set; }
    public long FramesSent { get; private set; }
    public long BytesSent { get; private set; }

    public SourceClient(string serverHost, int serverPort, string sourceId, string sourcePassword, int rtcmIntervalMs = 100)
    {
        _serverHost = serverHost;
        _serverPort = serverPort;
        _sourceId = sourceId;
        _sourcePassword = sourcePassword;
        _rtcmIntervalMs = rtcmIntervalMs;
        SourceId = sourceId;
    }

    public async Task ConnectAsync(CancellationToken cancellationToken = default)
    {
        try
        {
            _tcpClient = new TcpClient();
            await _tcpClient.ConnectAsync(_serverHost, _serverPort, cancellationToken);

            _stream = _tcpClient.GetStream();

            // Send SOURCE request: SOURCE <password> <mountpoint>
            var sourceRequest = $"SOURCE {_sourcePassword} {_sourceId}\r\n";
            var requestBytes = Encoding.ASCII.GetBytes(sourceRequest);
            await _stream.WriteAsync(requestBytes, cancellationToken);
            await _stream.FlushAsync(cancellationToken);

            // Read response
            var buffer = new byte[256];
            var bytesRead = await _stream.ReadAsync(buffer, cancellationToken);
            var response = Encoding.ASCII.GetString(buffer, 0, bytesRead);

            if (!response.Contains("200 OK"))
            {
                throw new InvalidOperationException($"Failed to connect as source: {response}");
            }

            IsConnected = true;
            ConnectedAt = DateTime.UtcNow;

            // Start sending RTCM data
            _cancellationTokenSource = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken);
            _sendTask = SendRtcmDataAsync(_cancellationTokenSource.Token);
        }
        catch (Exception ex)
        {
            IsConnected = false;
            throw new InvalidOperationException($"Failed to connect source {_sourceId}", ex);
        }
    }

    private async Task SendRtcmDataAsync(CancellationToken cancellationToken)
    {
        if (_stream == null) return;

        try
        {
            var random = new Random();

            while (!cancellationToken.IsCancellationRequested)
            {
                try
                {
                    // Generate fake RTCM frame (realistic RTCM1005 + RTCM1077 messages)
                    var rtcmFrame = GenerateRtcmFrame(random);

                    await _stream.WriteAsync(rtcmFrame, cancellationToken);
                    await _stream.FlushAsync(cancellationToken);

                    FramesSent++;
                    BytesSent += rtcmFrame.Length;

                    await Task.Delay(_rtcmIntervalMs, cancellationToken);
                }
                catch (OperationCanceledException)
                {
                    break;
                }
                catch (IOException)
                {
                    // Connection lost
                    IsConnected = false;
                    break;
                }
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"[Source {_sourceId}] Error sending RTCM: {ex.Message}");
        }
    }

    private byte[] GenerateRtcmFrame(Random random)
    {
        // RTCM 3 frame format:
        // [0xD3] [Reserved 2 bits] [Frame Length 10 bits] [Message Type 12 bits] [Payload] [CRC 24 bits]

        // Create realistic RTCM data (100-200 bytes typical)
        var payloadSize = random.Next(80, 200);
        var payload = new byte[payloadSize];
        random.NextBytes(payload);

        // Build frame
        var frame = new List<byte>();
        frame.Add(0xD3); // RTCM sync byte

        // Frame length (10 bits, includes CRC)
        var frameLength = (ushort)(payloadSize + 3); // payload + 3 bytes for CRC
        var lengthBytes = new byte[2];
        lengthBytes[0] = (byte)((frameLength >> 8) & 0x3F); // 6 MSBs + 2 reserved bits
        lengthBytes[1] = (byte)(frameLength & 0xFF);
        frame.Add(lengthBytes[0]);
        frame.Add(lengthBytes[1]);

        frame.AddRange(payload);

        // Add dummy CRC (24 bits)
        var crc = (uint)random.Next();
        frame.Add((byte)((crc >> 16) & 0xFF));
        frame.Add((byte)((crc >> 8) & 0xFF));
        frame.Add((byte)(crc & 0xFF));

        return frame.ToArray();
    }

    public async Task DisconnectAsync()
    {
        IsConnected = false;

        if (_cancellationTokenSource != null)
        {
            _cancellationTokenSource.Cancel();
            if (_sendTask != null)
            {
                try
                {
                    await _sendTask;
                }
                catch (OperationCanceledException) { }
            }
            _cancellationTokenSource.Dispose();
        }

        _stream?.Dispose();
        _tcpClient?.Dispose();
    }

    public TimeSpan GetConnectionDuration()
    {
        return IsConnected ? DateTime.UtcNow - ConnectedAt : TimeSpan.Zero;
    }
}
