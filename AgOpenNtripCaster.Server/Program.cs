using System.Text;
using DotNetEnv;
using Microsoft.AspNetCore.Authentication.JwtBearer;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.SignalR;
using Microsoft.EntityFrameworkCore;
using Microsoft.IdentityModel.Tokens;
using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.Entities;
using AgOpenNtripCaster.Server.Services.Auth;
using AgOpenNtripCaster.Server.Services.Data;
using AgOpenNtripCaster.Server.Services.Email;
using AgOpenNtripCaster.Server.Services.NTRIP;
using AgOpenNtripCaster.Server.Services.Notifications;
using Serilog;
using Serilog.Events;

// Load environment variables from .env
Env.Load();

var builder = WebApplication.CreateBuilder(args);

// Configure Serilog
Log.Logger = new LoggerConfiguration()
    .MinimumLevel.Information()
    // Filter out noisy namespaces - only show warnings and errors from them
    .MinimumLevel.Override("Microsoft.EntityFrameworkCore", LogEventLevel.Warning)
    .MinimumLevel.Override("Microsoft.AspNetCore", LogEventLevel.Warning)
    .MinimumLevel.Override("Microsoft.AspNetCore.DataProtection", LogEventLevel.Error)
    .WriteTo.Console()
    .WriteTo.File("logs/ntripcaster-.txt", rollingInterval: RollingInterval.Day)
    .CreateLogger();

builder.Host.UseSerilog();

// Configure PostgreSQL connection
// Support both CONNECTION_STRING (for backwards compatibility) and individual DB variables (Docker Compose)
var connectionString = Environment.GetEnvironmentVariable("CONNECTION_STRING");

if (string.IsNullOrEmpty(connectionString))
{
    // Build from individual environment variables (used by docker-compose)
    var dbHost = Environment.GetEnvironmentVariable("DB_HOST") ?? "localhost";
    var dbPort = Environment.GetEnvironmentVariable("DB_PORT") ?? "5432";
    var dbName = Environment.GetEnvironmentVariable("DB_NAME") ?? "ntripcaster";
    var dbUser = Environment.GetEnvironmentVariable("DB_USER") ?? "ntripuser";
    var dbPassword = Environment.GetEnvironmentVariable("DB_PASSWORD") ?? "ntrippass";

    connectionString = $"Host={dbHost};Port={dbPort};Database={dbName};Username={dbUser};Password={dbPassword}";
}

builder.Services.AddDbContext<ApplicationDbContext>(options =>
{
    options.UseNpgsql(connectionString);
});

// Configure Identity
builder.Services.AddIdentity<NtripUser, IdentityRole>()
    .AddEntityFrameworkStores<ApplicationDbContext>()
    .AddDefaultTokenProviders();

builder.Services.Configure<IdentityOptions>(options =>
{
    options.Password.RequireDigit = true;
    options.Password.RequiredLength = 8;
    options.Password.RequireNonAlphanumeric = false;
    options.Password.RequireUppercase = false;
    options.Password.RequireLowercase = false;
    options.User.RequireUniqueEmail = true;
});

// Configure JWT Authentication
var jwtSecret = Environment.GetEnvironmentVariable("JWT_SECRET") ?? "your-secret-key-here-min-32-chars";
var jwtKey = Encoding.ASCII.GetBytes(jwtSecret);

builder.Services.AddAuthentication(options =>
{
    options.DefaultAuthenticateScheme = JwtBearerDefaults.AuthenticationScheme;
    options.DefaultChallengeScheme = JwtBearerDefaults.AuthenticationScheme;
})
.AddJwtBearer(options =>
{
    options.TokenValidationParameters = new TokenValidationParameters
    {
        ValidateIssuerSigningKey = true,
        IssuerSigningKey = new SymmetricSecurityKey(jwtKey),
        ValidateIssuer = false,
        ValidateAudience = false,
        ValidateLifetime = true,
        ClockSkew = TimeSpan.Zero
    };
});

builder.Services.AddAuthorization();

// Configure Email Service
builder.Configuration.AddJsonFile("appsettings.json", optional: true, reloadOnChange: true);
builder.Services.AddScoped<IEmailService, EmailService>();
builder.Services.AddScoped<IEmailTriggerSettingsService, EmailTriggerSettingsService>();
builder.Services.AddScoped<IEmailSmtpSettingsService, EmailSmtpSettingsService>();

// Configure Auth Service
builder.Services.AddScoped<IAuthService, AuthService>();

// Configure User Service
builder.Services.AddScoped<IUserService, UserService>();

// Configure Group Service
builder.Services.AddScoped<IGroupService, GroupService>();

// Configure Mount Point Service
builder.Services.AddScoped<IMountPointService, MountPointService>();

// Configure Activity Service
builder.Services.AddScoped<IActivityService, ActivityService>();

// Configure Alert Service
builder.Services.AddScoped<IAlertService, AlertService>();

// Configure Connection Stats Service
builder.Services.AddSingleton<IConnectionStatsService, ConnectionStatsService>();

// Configure Telegram Notification Service
builder.Services.AddSingleton<ITelegramNotificationService, TelegramNotificationService>();

// Configure CORS
builder.Services.AddCors(options =>
{
    if (builder.Environment.IsDevelopment())
    {
        // Development: Allow all origins with credentials for local development
        options.AddPolicy("FrontendPolicy", policy =>
        {
            policy
                .SetIsOriginAllowed(_ => true)  // Allow any origin in development
                .AllowAnyHeader()
                .AllowAnyMethod()
                .AllowCredentials();  // Allow credentials for JWT and SignalR
        });
    }
    else
    {
        // Production: Restrict to specific origins
        var corsOrigins = Environment.GetEnvironmentVariable("CORS_ORIGINS") ?? "http://localhost:5173";
        options.AddPolicy("FrontendPolicy", policy =>
        {
            policy
                .WithOrigins(corsOrigins.Split(',').Select(o => o.Trim()).ToArray())
                .AllowAnyHeader()
                .AllowAnyMethod()
                .AllowCredentials();  // Allow credentials for JWT authentication and SignalR
        });
    }
});

// Add services
builder.Services.AddControllers();
builder.Services.AddOpenApi();
builder.Services.AddSignalR();

// Add NTRIP services
builder.Services.AddSingleton<AgOpenNtripCaster.Server.Services.NTRIP.ConnectionPool>();
builder.Services.AddScoped<AgOpenNtripCaster.Server.Services.Auth.NtripAuthenticationService>();
builder.Services.AddHostedService<AgOpenNtripCaster.Server.Services.NTRIP.NtripServerService>();

// Add Configuration services (CAS/NET)
builder.Services.AddScoped<AgOpenNtripCaster.Server.Services.Configuration.ICasterInfoService, AgOpenNtripCaster.Server.Services.Configuration.CasterInfoService>();
builder.Services.AddScoped<AgOpenNtripCaster.Server.Services.Configuration.INetworkInfoService, AgOpenNtripCaster.Server.Services.Configuration.NetworkInfoService>();

// Add User services
builder.Services.AddScoped<AgOpenNtripCaster.Server.Services.User.ISourcePasswordService, AgOpenNtripCaster.Server.Services.User.SourcePasswordService>();

// Add Database Seeder
builder.Services.AddScoped<IDatabaseSeeder, DatabaseSeeder>();

// Add Database Management Service
builder.Services.AddScoped<IDatabaseManagementService, DatabaseManagementService>();

var app = builder.Build();

// Configure middleware
if (app.Environment.IsDevelopment())
{
    app.MapOpenApi();
}

app.UseSerilogRequestLogging();
app.UseHttpsRedirection();
app.UseCors("FrontendPolicy");
app.UseAuthentication();
app.UseAuthorization();

// Map controllers and SignalR hub
app.MapControllers();
app.MapHub<AgOpenNtripCaster.Server.Hubs.NtripHub>("/api/ntrip-hub");

// Health check endpoint (for Docker health checks)
app.MapGet("/api/health", () => new
{
    status = "healthy",
    timestamp = DateTime.UtcNow,
    version = "1.0.0"
}).WithName("Health").WithOpenApi();

// Database initialization & seed default admin user
using (var scope = app.Services.CreateScope())
{
    var dbContext = scope.ServiceProvider.GetRequiredService<ApplicationDbContext>();
    var seeder = scope.ServiceProvider.GetRequiredService<IDatabaseSeeder>();

    try
    {
        // Run migrations
        await dbContext.Database.MigrateAsync();
        Log.Information("Database migrated successfully");

        // Seed database with default roles and admin user
        await seeder.SeedAsync();
    }
    catch (Exception ex)
    {
        Log.Error(ex, "Database initialization failed");
    }
}

try
{
    Log.Information("Starting NtripCaster server...");
    app.Run();
}
catch (Exception ex)
{
    Log.Fatal(ex, "Application terminated unexpectedly");
}
finally
{
    Log.CloseAndFlush();
}
