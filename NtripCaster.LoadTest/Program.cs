using NtripCaster.LoadTest;

var serverHost = "localhost";
var serverPort = 2101;
var durationSeconds = 120; // 2 minutes default
var numSources = 20;
var numClients = 200;

// Parse command line arguments
var cmdArgs = args.ToList();
for (int i = 0; i < cmdArgs.Count; i++)
{
    switch (cmdArgs[i].ToLower())
    {
        case "--host":
            if (i + 1 < cmdArgs.Count)
                serverHost = cmdArgs[++i];
            break;
        case "--port":
            if (i + 1 < cmdArgs.Count && int.TryParse(cmdArgs[++i], out var port))
                serverPort = port;
            break;
        case "--duration":
            if (i + 1 < cmdArgs.Count && int.TryParse(cmdArgs[++i], out var duration))
                durationSeconds = duration;
            break;
        case "--sources":
            if (i + 1 < cmdArgs.Count && int.TryParse(cmdArgs[++i], out var sources))
                numSources = sources;
            break;
        case "--clients":
            if (i + 1 < cmdArgs.Count && int.TryParse(cmdArgs[++i], out var clients))
                numClients = clients;
            break;
        case "--help":
        case "-h":
            PrintUsage();
            return;
    }
}

try
{
    // Check if server is reachable
    Console.WriteLine($"Checking server connectivity: {serverHost}:{serverPort}...");
    using (var client = new System.Net.Sockets.TcpClient())
    {
        var connectTask = client.ConnectAsync(serverHost, serverPort);
        if (!connectTask.Wait(TimeSpan.FromSeconds(5)))
        {
            Console.WriteLine($"❌ Cannot reach server at {serverHost}:{serverPort}. Is the NTRIP server running?");
            return;
        }
    }
    Console.WriteLine("✓ Server is reachable\n");

    // Run load test
    var orchestrator = new LoadTestOrchestrator(serverHost, serverPort);

    // Add Ctrl+C handler
    var cts = new CancellationTokenSource();
    Console.CancelKeyPress += (sender, e) =>
    {
        e.Cancel = true;
        cts.Cancel();
    };

    await orchestrator.RunAsync(numSources, numClients, durationSeconds);
    orchestrator.PrintFinalReport();
}
catch (Exception ex)
{
    Console.WriteLine($"\n❌ Error: {ex.Message}");
    Environment.Exit(1);
}

void PrintUsage()
{
    Console.WriteLine(@"
╔════════════════════════════════════════════════════════════╗
║  NTRIP Load Test Tool                                      ║
║  Simulates multiple GNSS sources and RTK clients           ║
╚════════════════════════════════════════════════════════════╝

Usage:
  dotnet run -- [options]

Options:
  --host <hostname>        NTRIP server hostname (default: localhost)
  --port <port>           NTRIP server port (default: 2101)
  --sources <count>       Number of source clients (default: 20)
  --clients <count>       Number of rover clients (default: 200)
  --duration <seconds>    Test duration in seconds (default: 120)
  --help                  Show this help message

Examples:
  # Default test (20 sources, 200 clients, 2 minutes)
  dotnet run

  # Production server, 10 sources, 500 clients, 5 minutes
  dotnet run -- --host rtk.example.com --port 2101 --sources 10 --clients 500 --duration 300

  # Light test for quick validation
  dotnet run -- --sources 5 --clients 50 --duration 60
");
}
