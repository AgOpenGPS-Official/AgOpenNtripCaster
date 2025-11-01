using NtripCaster.LoadTest;

var serverHost = "localhost";
var serverPort = 2101;
var apiUrl = "http://localhost:5000";
var durationSeconds = 120; // 2 minutes default
var numSources = 20;
var numClients = 200;
var adminEmail = "admin@ntripcaster.local";
var adminPassword = "AdminPassword123!";
var createMountPoints = false;

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
        case "--api":
            if (i + 1 < cmdArgs.Count)
                apiUrl = cmdArgs[++i];
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
        case "--admin-email":
            if (i + 1 < cmdArgs.Count)
                adminEmail = cmdArgs[++i];
            break;
        case "--admin-password":
            if (i + 1 < cmdArgs.Count)
                adminPassword = cmdArgs[++i];
            break;
        case "--setup":
            createMountPoints = true;
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
    Console.WriteLine("✓ Server is reachable");

    // Check if API is reachable
    Console.WriteLine($"Checking API connectivity: {apiUrl}...");
    using (var httpClient = new System.Net.Http.HttpClient())
    {
        try
        {
            var apiTask = httpClient.GetAsync($"{apiUrl}/api/health");
            if (!apiTask.Wait(TimeSpan.FromSeconds(5)))
            {
                Console.WriteLine($"⚠️  API health check timeout. Continuing anyway...");
            }
            else
            {
                Console.WriteLine("✓ API is reachable\n");
            }
        }
        catch
        {
            Console.WriteLine("⚠️  Could not reach API health endpoint. Continuing anyway...\n");
        }
    }

    // Run load test
    var orchestrator = new LoadTestOrchestrator(serverHost, serverPort, apiUrl);

    // Add Ctrl+C handler
    var cts = new CancellationTokenSource();
    Console.CancelKeyPress += (sender, e) =>
    {
        e.Cancel = true;
        cts.Cancel();
    };

    // Run test with optional mount point setup
    var adminCreds = createMountPoints ? (adminEmail, adminPassword) : (null, null);
    await orchestrator.RunAsync(numSources, numClients, durationSeconds, adminCreds.Item1, adminCreds.Item2);
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
║  With automatic mount point setup via API                  ║
╚════════════════════════════════════════════════════════════╝

Usage:
  dotnet run -- [options]

NTRIP Server Options:
  --host <hostname>           NTRIP server hostname (default: localhost)
  --port <port>              NTRIP server port (default: 2101)

API & Setup Options:
  --api <url>                API base URL (default: http://localhost:5000)
  --setup                    Create mount points before test (requires auth)
  --admin-email <email>      Admin email for auth (default: admin@ntripcaster.local)
  --admin-password <pass>    Admin password for auth (default: AdminPassword123!)

Load Test Options:
  --sources <count>          Number of source clients (default: 20)
  --clients <count>          Number of rover clients (default: 200)
  --duration <seconds>       Test duration in seconds (default: 120)

Other:
  --help                     Show this help message

Examples:
  # Default test without setup (assumes mount points exist)
  dotnet run

  # Setup mount points and run test (creates SOURCE_000 to SOURCE_019)
  dotnet run -- --setup

  # Setup with custom admin credentials
  dotnet run -- --setup --admin-email user@example.com --admin-password MyPassword123!

  # Light test with setup
  dotnet run -- --setup --sources 5 --clients 50 --duration 60

  # Production server with setup
  dotnet run -- --host rtk.example.com --api https://rtk.example.com/api --setup --sources 20 --clients 200

  # Only run test without setup (faster, reuses existing mount points)
  dotnet run -- --host localhost --port 2101 --sources 20 --clients 200 --duration 120
");
}
