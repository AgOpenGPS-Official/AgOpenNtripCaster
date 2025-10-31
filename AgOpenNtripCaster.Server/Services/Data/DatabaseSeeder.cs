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

    public DatabaseSeeder(
        ApplicationDbContext context,
        UserManager<NtripUser> userManager,
        RoleManager<IdentityRole> roleManager,
        ILogger<DatabaseSeeder> logger)
    {
        _context = context;
        _userManager = userManager;
        _roleManager = roleManager;
        _logger = logger;
    }

    public async Task SeedAsync()
    {
        try
        {
            // Check if database is already seeded
            if (_context.Users.Any())
            {
                _logger.LogInformation("Database already seeded, skipping seeding");
                return;
            }

            _logger.LogInformation("Starting database seeding...");

            // Create roles
            await CreateRolesAsync();

            // Create admin user
            await CreateAdminUserAsync();

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
        var roles = new[] { "Admin", "User" };

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
        const string adminEmail = "admin@ntripcaster.local";
        const string adminPassword = "ChangeMe@12345";

        var existingUser = await _userManager.FindByEmailAsync(adminEmail);
        if (existingUser != null)
        {
            _logger.LogInformation("Admin user already exists");
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
