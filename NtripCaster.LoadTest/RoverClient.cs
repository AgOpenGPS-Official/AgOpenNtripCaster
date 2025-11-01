using System.Net.Sockets;
using System.Text;

namespace NtripCaster.LoadTest;

/// <summary>
/// Simulates an RTK rover connecting to an NTRIP mount point,
/// receiving RTCM corrections and sending position updates
/// </summary>
public class RoverClient
{
    private readonly string _serverHost;
    private readonly int _serverPort;
    private readonly string _mountPoint;
    private readonly string _username;
    private readonly string _password;
    private readonly int _positionIntervalMs; // Interval between position updates

    private TcpClient? _tcpClient;
    private NetworkStream? _stream;
    private CancellationTokenSource? _cancellationTokenSource;
    private Task? _receiveTask;
    private Task? _positionTask;

    public string ClientId { get; }
    public bool IsConnected { get; private set; }
    public DateTime ConnectedAt { get; private set; }
    public long FramesReceived { get; private set; }
    public long BytesReceived { get; private set; }
    public long PositionsSent { get; private set; }
    public double LastLatitude { get; private set; }
    public double LastLongitude { get; private set; }
    public double LastAccuracy { get; private set; }

    public RoverClient(string serverHost, int serverPort, string mountPoint, string username, string password, int positionIntervalMs = 10000)
    {
        _serverHost = serverHost;
        _serverPort = serverPort;
        _mountPoint = mountPoint;
        _username = username;
        _password = password;
        _positionIntervalMs = positionIntervalMs;
        ClientId = $"{mountPoint}_{Guid.NewGuid().ToString().Substring(0, 8)}";
    }

    public async Task ConnectAsync(CancellationToken cancellationToken = default)
    {
        try
        {
            _tcpClient = new TcpClient();
            await _tcpClient.ConnectAsync(_serverHost, _serverPort, cancellationToken);

            _stream = _tcpClient.GetStream();

            // Create HTTP GET request with Basic Auth
            var credentials = Convert.ToBase64String(Encoding.ASCII.GetBytes($"{_username}:{_password}"));
            var getRequest = $"GET /{_mountPoint} HTTP/1.1\r\n" +
                           $"Host: {_serverHost}:{_serverPort}\r\n" +
                           $"Authorization: Basic {credentials}\r\n" +
                           $"User-Agent: NTRIP NtripCaster.LoadTest/1.0\r\n" +
                           $"Connection: keep-alive\r\n" +
                           $"\r\n";

            var requestBytes = Encoding.ASCII.GetBytes(getRequest);
            await _stream.WriteAsync(requestBytes, cancellationToken);
            await _stream.FlushAsync(cancellationToken);

            // Read HTTP response
            var buffer = new byte[1024];
            var bytesRead = await _stream.ReadAsync(buffer, cancellationToken);
            var response = Encoding.ASCII.GetString(buffer, 0, bytesRead);

            if (!response.Contains("200 OK"))
            {
                throw new InvalidOperationException($"Failed to connect to mount point {_mountPoint}: {response.Split('\n')[0]}");
            }

            IsConnected = true;
            ConnectedAt = DateTime.UtcNow;

            // Start receiving RTCM and sending positions
            _cancellationTokenSource = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken);
            _receiveTask = ReceiveRtcmDataAsync(_cancellationTokenSource.Token);
            _positionTask = SendPositionUpdatesAsync(_cancellationTokenSource.Token);
        }
        catch (Exception ex)
        {
            IsConnected = false;
            throw new InvalidOperationException($"Failed to connect to mount point {_mountPoint}", ex);
        }
    }

    private async Task ReceiveRtcmDataAsync(CancellationToken cancellationToken)
    {
        if (_stream == null) return;

        try
        {
            var buffer = new byte[4096];

            while (!cancellationToken.IsCancellationRequested && IsConnected)
            {
                try
                {
                    var bytesRead = await _stream.ReadAsync(buffer, cancellationToken);

                    if (bytesRead == 0)
                    {
                        // Connection closed
                        IsConnected = false;
                        break;
                    }

                    // Count RTCM frames (look for sync byte 0xD3)
                    for (int i = 0; i < bytesRead - 1; i++)
                    {
                        if (buffer[i] == 0xD3)
                        {
                            FramesReceived++;
                        }
                    }

                    BytesReceived += bytesRead;
                }
                catch (IOException)
                {
                    IsConnected = false;
                    break;
                }
            }
        }
        catch (OperationCanceledException) { }
        catch (Exception ex)
        {
            Console.WriteLine($"[Client {ClientId}] Error receiving RTCM: {ex.Message}");
        }
    }

    private async Task SendPositionUpdatesAsync(CancellationToken cancellationToken)
    {
        if (_stream == null) return;

        try
        {
            var random = new Random();

            while (!cancellationToken.IsCancellationRequested && IsConnected)
            {
                try
                {
                    // Generate random position in a region (simulating movement)
                    LastLatitude = 52.0 + random.NextDouble() * 0.01; // ~1km range
                    LastLongitude = 5.0 + random.NextDouble() * 0.01;
                    LastAccuracy = random.NextDouble() * 2.0 + 0.5; // 0.5-2.5 meters

                    var positionMessage = $"POS|{LastLatitude:F6}|{LastLongitude:F6}|{LastAccuracy:F2}\r\n";
                    var posBytes = Encoding.ASCII.GetBytes(positionMessage);

                    await _stream.WriteAsync(posBytes, cancellationToken);
                    await _stream.FlushAsync(cancellationToken);

                    PositionsSent++;

                    await Task.Delay(_positionIntervalMs, cancellationToken);
                }
                catch (OperationCanceledException)
                {
                    break;
                }
                catch (IOException)
                {
                    IsConnected = false;
                    break;
                }
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"[Client {ClientId}] Error sending positions: {ex.Message}");
        }
    }

    public async Task DisconnectAsync()
    {
        IsConnected = false;

        if (_cancellationTokenSource != null)
        {
            _cancellationTokenSource.Cancel();
            if (_receiveTask != null)
            {
                try
                {
                    await _receiveTask;
                }
                catch (OperationCanceledException) { }
            }
            if (_positionTask != null)
            {
                try
                {
                    await _positionTask;
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

    public double GetDataThroughputMbps()
    {
        var duration = GetConnectionDuration();
        if (duration.TotalSeconds == 0) return 0;

        return (BytesReceived * 8) / (1_000_000.0 * duration.TotalSeconds);
    }
}
