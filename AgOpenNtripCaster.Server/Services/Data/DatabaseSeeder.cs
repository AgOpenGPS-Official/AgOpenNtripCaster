using Microsoft.AspNetCore.Identity;
using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.Entities;

namespace AgOpenNtripCaster.Server.Services.Data;

public interface IDatabaseSeeder
{
    Task SeedAsync();
}

public class DatabaseSeeder : IDatabaseSeeder
{
    private readonly ApplicationDbContext _context;
    private readonly UserManager<NtripUser> _userManager;
    private readonly RoleManager<IdentityRole> _roleManager;
    private readonly ILogger<DatabaseSeeder> _logger;
    private readonly IConfiguration _configuration;

    public DatabaseSeeder(
        ApplicationDbContext context,
        UserManager<NtripUser> userManager,
        RoleManager<IdentityRole> roleManager,
        ILogger<DatabaseSeeder> logger,
        IConfiguration configuration)
    {
        _context = context;
        _userManager = userManager;
        _roleManager = roleManager;
        _logger = logger;
        _configuration = configuration;
    }

    public async Task SeedAsync()
    {
        try
        {
            _logger.LogInformation("Starting database seeding...");

            // Always ensure roles exist (even if users already exist)
            await CreateRolesAsync();

            // Only create admin user if no users exist
            if (!_context.Users.Any())
            {
                _logger.LogInformation("No users found, creating admin user...");
                await CreateAdminUserAsync();
            }
            else
            {
                _logger.LogInformation("Users already exist, skipping admin user creation");
            }

            _logger.LogInformation("Database seeding completed successfully");
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error occurred while seeding the database");
            throw;
        }
    }

    private async Task CreateRolesAsync()
    {
        var roles = new[] { "Admin", "User", "ReadOnly" };

        foreach (var role in roles)
        {
            if (!await _roleManager.RoleExistsAsync(role))
            {
                var result = await _roleManager.CreateAsync(new IdentityRole(role));
                if (result.Succeeded)
                {
                    _logger.LogInformation("Created role: {Role}", role);
                }
                else
                {
                    _logger.LogError("Failed to create role: {Role}", role);
                }
            }
        }
    }

    private async Task CreateAdminUserAsync()
    {
        // Read admin credentials from environment variables or appsettings
        var adminEmail = _configuration["Admin:Email"] ?? "admin@ntripcaster.local";
        var adminPassword = _configuration["Admin:Password"];

        // Require admin password to be set via environment variable for security
        if (string.IsNullOrWhiteSpace(adminPassword))
        {
            _logger.LogWarning(
                "Admin password not configured. Set Admin:Password environment variable. Using default for development.");
            adminPassword = "ChangeMe@12345";
        }

        var existingUser = await _userManager.FindByEmailAsync(adminEmail);
        if (existingUser != null)
        {
            _logger.LogInformation("Admin user already exists: {Email}", adminEmail);
            return;
        }

        var adminUser = new NtripUser
        {
            UserName = adminEmail,
            Email = adminEmail,
            EmailConfirmed = true,
            FullName = "System Administrator",
            CreatedAt = DateTime.UtcNow,
            MaxConnections = 100,
            IsActive = true
        };

        var result = await _userManager.CreateAsync(adminUser, adminPassword);
        if (result.Succeeded)
        {
            _logger.LogInformation("Created admin user: {Email}", adminEmail);

            // Assign admin role
            var roleResult = await _userManager.AddToRoleAsync(adminUser, "Admin");
            if (roleResult.Succeeded)
            {
                _logger.LogInformation("Assigned Admin role to user: {Email}", adminEmail);
            }
            else
            {
                _logger.LogError("Failed to assign Admin role to user: {Email}", adminEmail);
            }
        }
        else
        {
            _logger.LogError("Failed to create admin user. Errors: {Errors}",
                string.Join(", ", result.Errors.Select(e => e.Description)));
        }
    }
}
