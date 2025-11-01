# NtripCaster Load Test Tool

A comprehensive load testing tool for the NtripCaster NTRIP server. Simulates multiple GNSS sources and RTK clients to measure performance under load.

## Features

- **Source Simulation**: Simulates GNSS base stations streaming RTCM correction data
- **Client Simulation**: Simulates RTK rovers receiving corrections and sending position updates
- **Real NTRIP Protocol**: Full implementation of NTRIP authentication and streaming
- **Comprehensive Metrics**: Collects throughput, frame counts, connection rates, and performance data
- **Configurable Scenarios**: Test with any number of sources and clients
- **Live Monitoring**: Real-time metrics during test execution
- **Detailed Reports**: Summary statistics and performance analysis after test

## Building

```bash
cd NtripCaster.LoadTest
dotnet build -c Release
```

## Running Tests

### Basic Usage

Run with default parameters (20 sources, 200 clients, 120 seconds):

```bash
dotnet run
```

### Custom Configuration

```bash
# Test with 10 sources and 500 clients for 5 minutes
dotnet run -- --sources 10 --clients 500 --duration 300

# Test against remote server
dotnet run -- --host rtk.example.com --port 2101 --sources 20 --clients 200

# Light test for quick validation
dotnet run -- --sources 5 --clients 50 --duration 60
```

### Command Line Options

```
--host <hostname>        NTRIP server hostname (default: localhost)
--port <port>            NTRIP server port (default: 2101)
--sources <count>        Number of source clients (default: 20)
--clients <count>        Number of rover clients (default: 200)
--duration <seconds>     Test duration in seconds (default: 120)
--help                   Show help message
```

## Test Scenarios

### Quick Validation
```bash
dotnet run -- --sources 5 --clients 50 --duration 60
```
- Quick sanity check
- Tests basic connectivity and streaming
- ~30 seconds to run

### Standard Load Test
```bash
dotnet run -- --sources 20 --clients 200 --duration 120
```
- Default recommended test
- Simulates realistic deployment
- ~2 minutes to run
- **Use this to validate your VPS can handle target load**

### Enterprise Scenario
```bash
dotnet run -- --sources 100 --clients 1000 --duration 300
```
- Heavy load test
- Enterprise-scale deployment
- 5 minutes to run
- **Use this to identify bottlenecks**

### Stress Test
```bash
dotnet run -- --sources 50 --clients 2000 --duration 180
```
- Maximum stress testing
- Tests breaking point of server
- ~3 minutes to run
- **Use to find system limits**

## Understanding the Output

### During Test

```
TIME    | SOURCES | CLIENTS | SRC FRAMES | SRC BYTES(MB) | CLT FRAMES | CLT BYTES(MB) | AVG LATENCY(ms) |
--------|---------|---------|------------|---------------|------------|---------------|-----------------|
003s    |      20 |     200 |       3000 |           1.45 |       2950 |           1.43 |            0.64 |
008s    |      20 |     200 |       8000 |           3.87 |       7890 |           3.82 |            0.65 |
```

**Metrics explained:**
- **TIME**: Elapsed time since test start
- **SOURCES**: Number of active source connections
- **CLIENTS**: Number of active client connections
- **SRC FRAMES**: Total RTCM frames sent by all sources
- **SRC BYTES**: Total data sent by all sources
- **CLT FRAMES**: Total RTCM frames received by all clients
- **CLT BYTES**: Total data received by all clients
- **AVG LATENCY**: Estimated latency based on frame synchronization

### Final Report

```
📊 SOURCE STATISTICS
─────────────────────────────────────────────────────────────
Total Sources Connected: 20
Currently Connected:    20
Total Frames Sent:      600,000
Total Data Sent:        290.65 MB
Average Frame Size:     484 bytes
Average Duration:       120.00 seconds
Average Throughput:     2,421.92 KB/s
Avg Frames/sec:         5,000.0 fps

📊 CLIENT STATISTICS
─────────────────────────────────────────────────────────────
Total Clients Connected: 200
Currently Connected:    200
Total Frames Received:  590,000
Total Data Received:    285.58 MB
Average Frame Size:     484 bytes
Average Duration:       120.00 seconds
Average Throughput:     2,379.83 KB/s
Avg Frames/sec:         4,916.7 fps
Total Positions Sent:   2,000

📈 PERFORMANCE ANALYSIS
─────────────────────────────────────────────────────────────
Source Connection Success Rate:  100.0%
Client Connection Success Rate:  100.0%
Source Total Throughput:         2,421.92 KB/s
Client Total Throughput:         2,379.83 KB/s
Estimated Frame Loss Rate:        1.67%
```

## Performance Interpretation

### Success Indicators
- ✅ Connection success rate > 95%
- ✅ Frame loss rate < 5%
- ✅ All sources and clients sustain connection for test duration
- ✅ Throughput is consistent (no sudden drops)

### Warning Signs
- ⚠️ Connection success rate < 80%
- ⚠️ Frame loss rate > 10%
- ⚠️ Clients disconnecting mid-test
- ⚠️ High frame loss in early stages suggests server overload

### Recommendations by Result

**20 sources / 200 clients succeeds:**
- ✓ 2-4 core VPS with 4GB RAM is sufficient
- Consider adding monitoring/alerts
- Ready for production

**20 sources / 200 clients has issues:**
- ⚠️ Upgrade to 4-8 core VPS with 8GB RAM
- Add load balancer for multiple instances
- Review server configuration (connection limits, buffer sizes)

**100+ sources / 1000+ clients required:**
- 🚀 Enterprise setup needed:
  - 8+ core server
  - 16GB+ RAM
  - Database optimization (indexes, connection pooling)
  - Redis caching for positions
  - Multiple backend instances
  - Database replication

## Troubleshooting

### Cannot reach server
```
❌ Cannot reach server at localhost:2101. Is the NTRIP server running?
```
- Ensure NtripCaster backend is running
- Check firewall allows port 2101
- Verify correct hostname/port

### Low frame counts
- Server may be under high load
- Network congestion
- Check server logs for errors
- Reduce number of concurrent connections

### High frame loss
- Connection instability
- Server struggling to keep up
- Network issues
- Reduce test load or upgrade server

### All clients disconnect
- Server crashed or restarted
- Firewall timeout/rules changed
- Check server logs for errors

## Performance Tuning Tips

### For the Load Test
- Run test during off-peak hours
- Use a wired connection, not WiFi
- Close other network-heavy applications
- Run multiple iterations to get average results

### For the Server
- Monitor CPU, memory, disk I/O during test
- Check database query times
- Review connection pool settings
- Consider enabling compression if high throughput
- Profile with tools like dotTrace or PerfView

## Next Steps

1. **Baseline**: Run standard test (20 sources, 200 clients) to establish baseline
2. **Monitor**: Watch the metrics during test
3. **Analyze**: Review final report
4. **Scale**: Test with your target load
5. **Optimize**: If needed, optimize server configuration
6. **Validate**: Run test again to confirm improvements

## Files

- `Program.cs` - Entry point and CLI argument parsing
- `SourceClient.cs` - GNSS source client simulator
- `RoverClient.cs` - RTK rover client simulator
- `LoadTestOrchestrator.cs` - Test orchestration and metrics collection
- `README.md` - This file

## License

Same as NtripCaster - MIT License
