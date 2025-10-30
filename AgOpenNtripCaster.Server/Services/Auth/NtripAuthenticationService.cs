using Microsoft.AspNetCore.Identity;
using Microsoft.EntityFrameworkCore;
using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.Entities;
using AgOpenNtripCaster.Server.Services.User;

namespace AgOpenNtripCaster.Server.Services.Auth;

/// <summary>
/// Handles NTRIP-specific authentication
/// Two flows:
/// 1. SOURCE: Mount point name + source password
/// 2. CLIENT: Username + password + group membership
/// </summary>
public class NtripAuthenticationService
{
    private readonly ApplicationDbContext _dbContext;
    private readonly UserManager<NtripUser> _userManager;
    private readonly ISourcePasswordService _sourcePasswordService;
    private readonly ILogger<NtripAuthenticationService> _logger;

    public NtripAuthenticationService(
        ApplicationDbContext dbContext,
        UserManager<NtripUser> userManager,
        ISourcePasswordService sourcePasswordService,
        ILogger<NtripAuthenticationService> logger)
    {
        _dbContext = dbContext;
        _userManager = userManager;
        _sourcePasswordService = sourcePasswordService;
        _logger = logger;
    }

    /// <summary>
    /// Authenticate a GNSS station (source)
    /// Flow:
    /// 1. Username = MountPoint name (e.g., "BaseStationA")
    /// 2. Password = User's unique source password
    /// 3. Find mount point by name, get owner, verify source password
    /// </summary>
    public async Task<MountPoint?> AuthenticateSourceAsync(string mountPointName, string providedPassword)
    {
        try
        {
            // 1. Find mount point
            var mountPoint = await _dbContext.MountPoints
                .FirstOrDefaultAsync(m => m.Name == mountPointName && m.IsActive);

            if (mountPoint == null)
            {
                _logger.LogWarning($"Source auth failed: Mount point '{mountPointName}' not found");
                return null;
            }

            // 2. Get mount point owner
            if (string.IsNullOrEmpty(mountPoint.UserId))
            {
                _logger.LogWarning($"Source auth failed: Mount point '{mountPointName}' has no owner");
                return null;
            }

            var owner = await _userManager.FindByIdAsync(mountPoint.UserId);
            if (owner == null || !owner.IsActive)
            {
                _logger.LogWarning($"Source auth failed: Mount point owner for '{mountPointName}' not found or inactive");
                return null;
            }

            // 3. Verify source password using SourcePasswordService (bcrypt)
            var passwordValid = await _sourcePasswordService.VerifySourcePasswordAsync(owner.Id, providedPassword);
            if (!passwordValid)
            {
                _logger.LogWarning($"Source auth failed: Invalid source password for mount point '{mountPointName}'");
                return null;
            }

            _logger.LogInformation($"Source authenticated: {mountPointName} (owner: {owner.Email})");
            return mountPoint;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, $"Source authentication error for {mountPointName}");
            return null;
        }
    }

    /// <summary>
    /// Authenticate a client
    /// GET /STATION_A
    /// Authorization: Basic username:password
    /// </summary>
    public async Task<ClientAuthResult> AuthenticateClientAsync(
        string username,
        string password,
        string mountPointName)
    {
        try
        {
            // 1. Find user
            var user = await _userManager.FindByNameAsync(username);
            if (user == null)
            {
                _logger.LogWarning($"Client auth failed: User '{username}' not found");
                return new ClientAuthResult { Success = false, Reason = "User not found" };
            }

            // 2. Check if user is active
            if (!user.IsActive)
            {
                _logger.LogWarning($"Client auth failed: User '{username}' is inactive");
                return new ClientAuthResult { Success = false, Reason = "User is inactive" };
            }

            // 3. Verify password
            var passwordOk = await _userManager.CheckPasswordAsync(user, password);
            if (!passwordOk)
            {
                _logger.LogWarning($"Client auth failed: Wrong password for user '{username}'");
                return new ClientAuthResult { Success = false, Reason = "Invalid password" };
            }

            // 4. Find mount point
            var mountPoint = await _dbContext.MountPoints
                .Include(m => m.AllowedGroups)
                .FirstOrDefaultAsync(m => m.Name == mountPointName && m.IsActive);

            if (mountPoint == null)
            {
                _logger.LogWarning($"Client auth failed: Mount point '{mountPointName}' not found");
                return new ClientAuthResult { Success = false, Reason = "Mount point not found" };
            }

            // 5. Check authentication requirement
            if (!mountPoint.RequireClientAuthentication)
            {
                // Public mount point - no further checks needed
                _logger.LogInformation($"Client authenticated (public): {username}@{mountPointName}");
                return new ClientAuthResult
                {
                    Success = true,
                    User = user,
                    MountPoint = mountPoint,
                    IsPublic = true
                };
            }

            // 6. Check group membership
            var userGroups = user.Groups.Select(g => g.Id).ToHashSet();
            var allowedGroupIds = mountPoint.AllowedGroups.Select(g => g.Id).ToHashSet();

            var hasAccess = userGroups.Intersect(allowedGroupIds).Any();
            if (!hasAccess)
            {
                _logger.LogWarning(
                    $"Client auth failed: User '{username}' not in allowed groups for '{mountPointName}'");
                return new ClientAuthResult
                {
                    Success = false,
                    Reason = "Not in allowed group"
                };
            }

            // 7. Check user connection limit
            var activeConnectionsForUser = await _dbContext.ClientSessions
                .CountAsync(c => c.UserId == user.Id && c.DisconnectedAt == null);

            if (activeConnectionsForUser >= user.MaxConnections)
            {
                _logger.LogWarning(
                    $"Client auth failed: User '{username}' reached max connections ({user.MaxConnections})");
                return new ClientAuthResult
                {
                    Success = false,
                    Reason = $"Max connections ({user.MaxConnections}) reached"
                };
            }

            _logger.LogInformation($"Client authenticated: {username}@{mountPointName}");
            return new ClientAuthResult
            {
                Success = true,
                User = user,
                MountPoint = mountPoint,
                IsPublic = false
            };
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, $"Client authentication error for {username}@{mountPointName}");
            return new ClientAuthResult { Success = false, Reason = "Authentication error" };
        }
    }
}

public class ClientAuthResult
{
    public bool Success { get; set; }
    public string Reason { get; set; } = string.Empty;
    public NtripUser? User { get; set; }
    public MountPoint? MountPoint { get; set; }
    public bool IsPublic { get; set; }
}
