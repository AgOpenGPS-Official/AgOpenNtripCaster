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
using AgOpenNtripCaster.Server.Services.Email;
using AgOpenNtripCaster.Server.Services.NTRIP;
using Serilog;

// Load environment variables from .env
Env.Load();

var builder = WebApplication.CreateBuilder(args);

// Configure Serilog
Log.Logger = new LoggerConfiguration()
    .MinimumLevel.Debug()
    .WriteTo.Console()
    .WriteTo.File("logs/ntripcaster-.txt", rollingInterval: RollingInterval.Day)
    .CreateLogger();

builder.Host.UseSerilog();

// Configure PostgreSQL connection
var connectionString = Environment.GetEnvironmentVariable("CONNECTION_STRING")
    ?? "Host=localhost;Port=5432;Database=ntripcaster;Username=ntripuser;Password=ntrippass";

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

// Configure Auth Service
builder.Services.AddScoped<IAuthService, AuthService>();

// Configure User Service
builder.Services.AddScoped<IUserService, UserService>();

// Configure Group Service
builder.Services.AddScoped<IGroupService, GroupService>();

// Configure Mount Point Service
builder.Services.AddScoped<IMountPointService, MountPointService>();

// Configure CORS
builder.Services.AddCors(options =>
{
    // Get allowed origins (development defaults to localhost:5173, production from env var)
    var corsOrigins = Environment.GetEnvironmentVariable("CORS_ORIGIN") ?? "http://localhost:5173";

    options.AddPolicy("FrontendPolicy", policy =>
    {
        policy
            .WithOrigins(corsOrigins.Split(',').Select(o => o.Trim()).ToArray())
            .AllowAnyHeader()
            .AllowAnyMethod()
            .AllowCredentials(); // Allow credentials for JWT authentication and SignalR
    });
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

// Health check endpoint
app.MapGet("/health", () => new
{
    status = "healthy",
    timestamp = DateTime.UtcNow,
    version = "1.0.0"
}).WithName("Health").WithOpenApi();

// Database initialization & seed default admin user
using (var scope = app.Services.CreateScope())
{
    var dbContext = scope.ServiceProvider.GetRequiredService<ApplicationDbContext>();
    var userManager = scope.ServiceProvider.GetRequiredService<UserManager<NtripUser>>();
    var roleManager = scope.ServiceProvider.GetRequiredService<RoleManager<IdentityRole>>();

    try
    {
        // Run migrations
        await dbContext.Database.MigrateAsync();
        Log.Information("Database migrated successfully");

        // Create default roles
        var adminRoleExists = await roleManager.RoleExistsAsync("Admin");
        if (!adminRoleExists)
        {
            await roleManager.CreateAsync(new IdentityRole("Admin"));
            Log.Information("Admin role created");
        }

        var userRoleExists = await roleManager.RoleExistsAsync("User");
        if (!userRoleExists)
        {
            await roleManager.CreateAsync(new IdentityRole("User"));
            Log.Information("User role created");
        }

        // Create default admin user
        var adminEmail = "admin@ntripcaster.local";
        var existingAdmin = await userManager.FindByEmailAsync(adminEmail);

        if (existingAdmin == null)
        {
            var adminUser = new NtripUser
            {
                UserName = adminEmail,
                Email = adminEmail,
                FullName = "System Administrator",
                EmailConfirmed = true,
                IsActive = true,
                MaxConnections = 100,
                CreatedAt = DateTime.UtcNow
            };

            var createResult = await userManager.CreateAsync(adminUser, "ChangeMe@12345");
            if (createResult.Succeeded)
            {
                await userManager.AddToRoleAsync(adminUser, "Admin");
                Log.Information("Default admin user created: admin@ntripcaster.local");
            }
            else
            {
                var errors = string.Join(", ", createResult.Errors.Select(e => e.Description));
                Log.Warning($"Failed to create default admin user: {errors}");
            }
        }
        else
        {
            // Ensure existing admin user has Admin role
            var isInAdminRole = await userManager.IsInRoleAsync(existingAdmin, "Admin");
            if (!isInAdminRole)
            {
                await userManager.AddToRoleAsync(existingAdmin, "Admin");
                Log.Information("Added Admin role to existing admin user");
            }
            Log.Information("Default admin user already exists");
        }
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
