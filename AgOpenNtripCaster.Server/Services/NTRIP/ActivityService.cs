using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.Entities;
using Microsoft.EntityFrameworkCore;

namespace AgOpenNtripCaster.Server.Services.NTRIP;

public interface IActivityService
{
    Task LogActivityAsync(ActivityType type, int mountPointId, string? userId, string description);
    Task<List<ActivityDto>> GetRecentActivitiesAsync(int limit = 50);
    Task<List<ActivityDto>> GetActivitiesByMountPointAsync(int mountPointId, int limit = 20);
    Task<List<ActivityDto>> GetActivitiesByUserAsync(string userId, int limit = 20);
}

public class ActivityService : IActivityService
{
    private readonly ApplicationDbContext _dbContext;

    public ActivityService(ApplicationDbContext dbContext)
    {
        _dbContext = dbContext;
    }

    public async Task LogActivityAsync(ActivityType type, int mountPointId, string? userId, string description)
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

    private static ActivityDto MapToActivityDto(Activity activity)
    {
        return new ActivityDto
        {
            Id = activity.Id,
            Type = activity.Type.ToString(),
            MountPointId = activity.MountPointId,
            MountPointName = activity.MountPoint?.Name ?? "Unknown",
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
    public int MountPointId { get; set; }
    public string MountPointName { get; set; } = string.Empty;
    public string? UserId { get; set; }
    public string? UserName { get; set; }
    public string Description { get; set; } = string.Empty;
    public DateTime CreatedAt { get; set; }
}
