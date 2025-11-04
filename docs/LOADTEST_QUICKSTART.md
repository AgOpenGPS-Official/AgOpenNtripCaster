# NtripCaster Load Testing - Quick Start Guide

## What is This?

A comprehensive load testing tool to verify your NtripCaster server can handle 20 sources + 200 clients simultaneously (the test scenario you asked for).

## Quick Start (2 minutes)

### 1. Build the Load Test Tool
```bash
cd NtripCaster.LoadTest
dotnet build -c Release
```

### 2. Start NtripCaster Server
In another terminal:
```bash
cd AgOpenNtripCaster.Server
dotnet run
# Wait for "Now listening on: http://localhost:5000"
```

### 3. Run the Load Test
```bash
cd NtripCaster.LoadTest
dotnet run
```

**That's it!** The test will:
- Connect 20 GNSS sources streaming RTCM data
- Connect 200 RTK rovers requesting corrections
- Run for 2 minutes (120 seconds)
- Display live metrics every 5 seconds
- Print a final report with performance analysis

## Understanding the Results

### Success Looks Like:
```
✓ All 20 sources connected
✓ All 200 clients connected
✓ Frame loss < 5%
✓ Stable throughput throughout test
```

### What the Metrics Mean

**During Test:**
- `SRC FRAMES`: Total RTCM frames sent by all sources
- `CLT FRAMES`: Total RTCM frames received by all clients
- `SRC BYTES(MB)`: Total megabytes sent
- `CLT BYTES(MB)`: Total megabytes received

**Final Report:**
- **Connection Success Rate**: % of connections that stayed connected
- **Frame Loss Rate**: % of frames that didn't reach clients
- **Throughput**: MB/s or KB/s for sustained data rate

## Custom Test Scenarios

### Light Test (Quick Validation)
```bash
dotnet run -- --sources 5 --clients 50 --duration 60
```

### Your Target Load (20 sources, 200 clients)
```bash
dotnet run -- --sources 20 --clients 200 --duration 120
```

### Heavy Test (Enterprise Scale)
```bash
dotnet run -- --sources 100 --clients 1000 --duration 300
```

### Production Server Test
```bash
dotnet run -- --host rtk.example.com --port 2101 --sources 20 --clients 200 --duration 120
```

## Interpreting Results

| Result | Interpretation | Recommendation |
|--------|---|---|
| **All pass, 0% loss** | Server easily handles load | 2-4 core VPS sufficient |
| **All pass, <5% loss** | Server handles load well | 4GB RAM, 2-4 cores recommended |
| **Some disconnect** | Server struggling | Upgrade to 8GB RAM, 4+ cores |
| **Many failures** | Server overloaded | Enterprise setup needed |

## VPS Sizing Guide

Based on your test results:

**For 20 sources + 200 clients:**
- ✅ **Minimum**: 2 cores, 4GB RAM (~$20/month)
- ✅ **Recommended**: 4 cores, 8GB RAM (~$40/month)
- ⚠️ **If issues**: 8 cores, 16GB RAM + load balancer

**For 100 sources + 1000 clients:**
- 🚀 Need multiple instances with load balancer
- Database optimization essential
- Consider Redis for caching

## Troubleshooting

### "Cannot reach server at localhost:2101"
- Backend server not running
- Port 2101 blocked by firewall
- Check server is listening: `netstat -an | grep 2101`

### Many clients fail to connect
- Server overloaded or crashing
- Check backend logs: `dotnet run` output in server terminal
- Try fewer clients: `--clients 100`

### Frame loss high (>10%)
- Network congestion
- Server can't keep up with load
- Try fewer sources: `--sources 10`

### All clients disconnect mid-test
- Server crashed
- Check for errors in server logs
- Reduce test duration or load

## Next Steps

1. **Baseline Test**: Run standard test to establish baseline
2. **Monitor**: Watch the live metrics
3. **Analyze**: Review the final report
4. **Scale**: Test with your actual expected load
5. **Optimize**: Adjust server config based on results
6. **Validate**: Run test again to confirm improvements

## Project Files

```
NtripCaster.LoadTest/
├── Program.cs                  # CLI entry point
├── SourceClient.cs            # GNSS source simulator
├── RoverClient.cs             # RTK rover simulator
├── LoadTestOrchestrator.cs    # Test orchestration & metrics
├── README.md                  # Full documentation
└── NtripCaster.LoadTest.csproj
```

## For More Details

See `NtripCaster.LoadTest/README.md` for:
- Detailed metrics explanation
- Performance tuning tips
- Advanced scenarios
- How the NTRIP protocol is simulated

---

**Questions?** Check the README.md in NtripCaster.LoadTest folder
