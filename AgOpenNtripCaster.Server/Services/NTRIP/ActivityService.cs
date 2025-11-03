using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.Entities;
using AgOpenNtripCaster.Server.Hubs;
using Microsoft.AspNetCore.SignalR;
using Microsoft.EntityFrameworkCore;

namespace AgOpenNtripCaster.Server.Services.NTRIP;

public interface IActivityService
{
    Task LogActivityAsync(ActivityType type, int? mountPointId, string? userId, string description);

    // Convenience methods for common user activity events
    Task LogUserLoginAsync(string userId, string userName);
    Task LogUserLogoutAsync(string userId, string userName);
    Task LogUserCreatedAsync(string userId, string createdByUserId, string userName);
    Task LogUserUpdatedAsync(string userId, string updatedByUserId, string userName);
    Task LogUserDeletedAsync(string userId, string deletedByUserId, string userName);
    Task LogMountPointCreatedAsync(int mountPointId, string mountPointName, string userId, string userName);
    Task LogMountPointUpdatedAsync(int mountPointId, string mountPointName, string userId, string userName);
    Task LogMountPointDeletedAsync(int mountPointId, string mountPointName, string userId, string userName);
    Task LogGroupCreatedAsync(int groupId, string groupName, string userId, string userName);
    Task LogGroupUpdatedAsync(int groupId, string groupName, string userId, string userName);
    Task LogGroupDeletedAsync(int groupId, string groupName, string userId, string userName);
    Task LogPermissionChangedAsync(int mountPointId, string mountPointName, int groupId, string groupName, string userId, string userName);

    Task<List<ActivityDto>> GetRecentActivitiesAsync(int limit = 50);
    Task<List<ActivityDto>> GetActivitiesByMountPointAsync(int mountPointId, int limit = 20);
    Task<List<ActivityDto>> GetActivitiesByUserAsync(string userId, int limit = 20);
}

public class ActivityService : IActivityService
{
    private readonly ApplicationDbContext _dbContext;
    private readonly IHubContext<NtripHub> _hubContext;
    private readonly ILogger<ActivityService> _logger;

    public ActivityService(ApplicationDbContext dbContext, IHubContext<NtripHub> hubContext, ILogger<ActivityService> logger)
    {
        _dbContext = dbContext;
        _hubContext = hubContext;
        _logger = logger;
    }

    public async Task LogActivityAsync(ActivityType type, int? mountPointId, string? userId, string description)
    {
        try
        {
            var activity = new Activity
            {
                Type = type,
                MountPointId = mountPointId,
                UserId = userId,
                Description = description,
                CreatedAt = DateTime.UtcNow
            };

            _dbContext.Activities.Add(activity);
            await _dbContext.SaveChangesAsync();

            // Broadcast new activity to all connected SignalR clients
            try
            {
                var activityDto = new ActivityDto
                {
                    Id = activity.Id,
                    Type = activity.Type.ToString(),
                    MountPointId = activity.MountPointId,
                    MountPointName = activity.MountPoint?.Name,
                    UserId = activity.UserId,
                    UserName = activity.User?.UserName ?? (activity.UserId != null ? "Unknown User" : null),
                    Description = activity.Description,
                    CreatedAt = activity.CreatedAt
                };

                await _hubContext.Clients.All.SendAsync("ActivityCreated", activityDto);
            }
            catch (Exception signalREx)
            {
                _logger.LogError(signalREx, "Error broadcasting activity via SignalR");
            }
        }
        catch (Exception ex)
        {
            System.Diagnostics.Debug.WriteLine($"Error logging activity: {ex.Message}");
            // Don't throw - activity logging shouldn't break the main flow
        }
    }

    public async Task<List<ActivityDto>> GetRecentActivitiesAsync(int limit = 50)
    {
        try
        {
            var activities = await _dbContext.Activities
                .Include(a => a.MountPoint)
                .Include(a => a.User)
                .OrderByDescending(a => a.CreatedAt)
                .Take(limit)
                .ToListAsync();

            return activities.Select(a => MapToActivityDto(a)).ToList();
        }
        catch (Exception ex)
        {
            System.Diagnostics.Debug.WriteLine($"Error retrieving activities: {ex.Message}");
            return new List<ActivityDto>();
        }
    }

    public async Task<List<ActivityDto>> GetActivitiesByMountPointAsync(int mountPointId, int limit = 20)
    {
        try
        {
            var activities = await _dbContext.Activities
                .Where(a => a.MountPointId == mountPointId)
                .Include(a => a.MountPoint)
                .Include(a => a.User)
                .OrderByDescending(a => a.CreatedAt)
                .Take(limit)
                .ToListAsync();

            return activities.Select(a => MapToActivityDto(a)).ToList();
        }
        catch (Exception ex)
        {
            System.Diagnostics.Debug.WriteLine($"Error retrieving mount point activities: {ex.Message}");
            return new List<ActivityDto>();
        }
    }

    public async Task<List<ActivityDto>> GetActivitiesByUserAsync(string userId, int limit = 20)
    {
        try
        {
            var activities = await _dbContext.Activities
                .Where(a => a.UserId == userId)
                .Include(a => a.MountPoint)
                .Include(a => a.User)
                .OrderByDescending(a => a.CreatedAt)
                .Take(limit)
                .ToListAsync();

            return activities.Select(a => MapToActivityDto(a)).ToList();
        }
        catch (Exception ex)
        {
            System.Diagnostics.Debug.WriteLine($"Error retrieving user activities: {ex.Message}");
            return new List<ActivityDto>();
        }
    }

    // Convenience methods for user activity events
    public async Task LogUserLoginAsync(string userId, string userName)
    {
        await LogActivityAsync(ActivityType.UserLogin, null, userId, $"User {userName} logged in");
    }

    public async Task LogUserLogoutAsync(string userId, string userName)
    {
        await LogActivityAsync(ActivityType.UserLogout, null, userId, $"User {userName} logged out");
    }

    public async Task LogUserCreatedAsync(string userId, string createdByUserId, string userName)
    {
        await LogActivityAsync(ActivityType.UserCreated, null, createdByUserId, $"User {userName} was created");
    }

    public async Task LogUserUpdatedAsync(string userId, string updatedByUserId, string userName)
    {
        await LogActivityAsync(ActivityType.UserUpdated, null, updatedByUserId, $"User {userName} was updated");
    }

    public async Task LogUserDeletedAsync(string userId, string deletedByUserId, string userName)
    {
        await LogActivityAsync(ActivityType.UserDeleted, null, deletedByUserId, $"User {userName} was deleted");
    }

    public async Task LogMountPointCreatedAsync(int mountPointId, string mountPointName, string userId, string userName)
    {
        await LogActivityAsync(ActivityType.MountPointCreated, mountPointId, userId, $"Mount point '{mountPointName}' was created by {userName}");
    }

    public async Task LogMountPointUpdatedAsync(int mountPointId, string mountPointName, string userId, string userName)
    {
        await LogActivityAsync(ActivityType.MountPointUpdated, mountPointId, userId, $"Mount point '{mountPointName}' was updated by {userName}");
    }

    public async Task LogMountPointDeletedAsync(int mountPointId, string mountPointName, string userId, string userName)
    {
        await LogActivityAsync(ActivityType.MountPointDeleted, null, userId, $"Mount point '{mountPointName}' was deleted by {userName}");
    }

    public async Task LogGroupCreatedAsync(int groupId, string groupName, string userId, string userName)
    {
        await LogActivityAsync(ActivityType.GroupCreated, null, userId, $"Group '{groupName}' was created by {userName}");
    }

    public async Task LogGroupUpdatedAsync(int groupId, string groupName, string userId, string userName)
    {
        await LogActivityAsync(ActivityType.GroupUpdated, null, userId, $"Group '{groupName}' was updated by {userName}");
    }

    public async Task LogGroupDeletedAsync(int groupId, string groupName, string userId, string userName)
    {
        await LogActivityAsync(ActivityType.GroupDeleted, null, userId, $"Group '{groupName}' was deleted by {userName}");
    }

    public async Task LogPermissionChangedAsync(int mountPointId, string mountPointName, int groupId, string groupName, string userId, string userName)
    {
        await LogActivityAsync(ActivityType.PermissionsChanged, mountPointId, userId, $"Permissions for group '{groupName}' on mount point '{mountPointName}' were changed by {userName}");
    }

    private static ActivityDto MapToActivityDto(Activity activity)
    {
        return new ActivityDto
        {
            Id = activity.Id,
            Type = activity.Type.ToString(),
            MountPointId = activity.MountPointId,
            MountPointName = activity.MountPoint?.Name,
            UserId = activity.UserId,
            UserName = activity.User?.UserName ?? (activity.UserId != null ? "Unknown User" : null),
            Description = activity.Description,
            CreatedAt = activity.CreatedAt
        };
    }
}

public class ActivityDto
{
    public int Id { get; set; }
    public string Type { get; set; } = string.Empty;
    public int? MountPointId { get; set; }
    public string? MountPointName { get; set; }
    public string? UserId { get; set; }
    public string? UserName { get; set; }
    public string Description { get; set; } = string.Empty;
    public DateTime CreatedAt { get; set; }
}
