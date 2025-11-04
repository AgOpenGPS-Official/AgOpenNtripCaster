using Microsoft.EntityFrameworkCore;
using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.DTOs;
using AgOpenNtripCaster.Server.Models.Entities;

namespace AgOpenNtripCaster.Server.Services.Auth;

/// <summary>
/// Service for group management operations
/// </summary>
public interface IGroupService
{
    Task<GroupListResponse> GetGroupsAsync(int page = 1, int pageSize = 10);
    Task<GroupDto?> GetGroupByIdAsync(int groupId);
    Task<CreateGroupResponse> CreateGroupAsync(CreateGroupRequest request);
    Task<UpdateGroupResponse> UpdateGroupAsync(int groupId, UpdateGroupRequest request);
    Task<DeleteGroupResponse> DeleteGroupAsync(int groupId);
    Task<GroupMembershipResponse> AddUserToGroupAsync(int groupId, string userId);
    Task<GroupMembershipResponse> RemoveUserFromGroupAsync(int groupId, string userId);
}

public class GroupService : IGroupService
{
    private readonly ApplicationDbContext _dbContext;
    private readonly ILogger<GroupService> _logger;

    public GroupService(ApplicationDbContext dbContext, ILogger<GroupService> logger)
    {
        _dbContext = dbContext;
        _logger = logger;
    }

    public async Task<GroupListResponse> GetGroupsAsync(int page = 1, int pageSize = 10)
    {
        var groups = await _dbContext.NtripGroups
            .Include(g => g.Users)
            .Include(g => g.MountPoints)
            .Skip((page - 1) * pageSize)
            .Take(pageSize)
            .ToListAsync();

        var total = await _dbContext.NtripGroups.CountAsync();

        return new GroupListResponse
        {
            Total = total,
            Page = page,
            PageSize = pageSize,
            Groups = groups.Select(MapToGroupDto).ToList()
        };
    }

    public async Task<GroupDto?> GetGroupByIdAsync(int groupId)
    {
        var group = await _dbContext.NtripGroups
            .Include(g => g.Users)
            .Include(g => g.MountPoints)
            .FirstOrDefaultAsync(g => g.Id == groupId);

        if (group == null)
        {
            return null;
        }

        return MapToGroupDto(group);
    }

    public async Task<CreateGroupResponse> CreateGroupAsync(CreateGroupRequest request)
    {
        // Validate input
        if (string.IsNullOrWhiteSpace(request.Name))
        {
            return new CreateGroupResponse
            {
                Success = false,
                Message = "Group name is required"
            };
        }

        // Check if group name already exists
        var existingGroup = await _dbContext.NtripGroups
            .FirstOrDefaultAsync(g => g.Name == request.Name);

        if (existingGroup != null)
        {
            return new CreateGroupResponse
            {
                Success = false,
                Message = "Group name already exists"
            };
        }

        // Create group
        var group = new NtripGroup
        {
            Name = request.Name,
            Description = request.Description,
            IsActive = request.IsActive,
            CreatedAt = DateTime.UtcNow
        };

        _dbContext.NtripGroups.Add(group);
        await _dbContext.SaveChangesAsync();

        _logger.LogInformation($"Group created: {group.Name}");

        return new CreateGroupResponse
        {
            Success = true,
            Message = "Group created successfully",
            Group = MapToGroupDto(group)
        };
    }

    public async Task<UpdateGroupResponse> UpdateGroupAsync(int groupId, UpdateGroupRequest request)
    {
        var group = await _dbContext.NtripGroups
            .Include(g => g.Users)
            .Include(g => g.MountPoints)
            .FirstOrDefaultAsync(g => g.Id == groupId);

        if (group == null)
        {
            return new UpdateGroupResponse
            {
                Success = false,
                Message = "Group not found"
            };
        }

        // Update fields
        if (!string.IsNullOrWhiteSpace(request.Name))
        {
            // Check if new name already exists (except for this group)
            var existingGroup = await _dbContext.NtripGroups
                .FirstOrDefaultAsync(g => g.Name == request.Name && g.Id != groupId);

            if (existingGroup != null)
            {
                return new UpdateGroupResponse
                {
                    Success = false,
                    Message = "Group name already exists"
                };
            }

            group.Name = request.Name;
        }

        if (request.Description != null)
        {
            group.Description = request.Description;
        }

        if (request.IsActive.HasValue)
        {
            group.IsActive = request.IsActive.Value;
        }

        _dbContext.NtripGroups.Update(group);
        await _dbContext.SaveChangesAsync();

        _logger.LogInformation($"Group updated: {group.Name}");

        return new UpdateGroupResponse
        {
            Success = true,
            Message = "Group updated successfully",
            Group = MapToGroupDto(group)
        };
    }

    public async Task<DeleteGroupResponse> DeleteGroupAsync(int groupId)
    {
        var group = await _dbContext.NtripGroups
            .Include(g => g.Users)
            .FirstOrDefaultAsync(g => g.Id == groupId);

        if (group == null)
        {
            return new DeleteGroupResponse
            {
                Success = false,
                Message = "Group not found"
            };
        }

        // Remove group from all users
        foreach (var user in group.Users.ToList())
        {
            user.Groups.Remove(group);
        }

        _dbContext.NtripGroups.Remove(group);
        await _dbContext.SaveChangesAsync();

        _logger.LogInformation($"Group deleted: {group.Name}");

        return new DeleteGroupResponse
        {
            Success = true,
            Message = "Group deleted successfully"
        };
    }

    public async Task<GroupMembershipResponse> AddUserToGroupAsync(int groupId, string userId)
    {
        var group = await _dbContext.NtripGroups
            .Include(g => g.Users)
            .Include(g => g.MountPoints)
            .FirstOrDefaultAsync(g => g.Id == groupId);

        if (group == null)
        {
            return new GroupMembershipResponse
            {
                Success = false,
                Message = "Group not found"
            };
        }

        var user = await _dbContext.Users.FirstOrDefaultAsync(u => u.Id == userId);
        if (user == null)
        {
            return new GroupMembershipResponse
            {
                Success = false,
                Message = "User not found"
            };
        }

        // Check if user is already in group
        if (group.Users.Any(u => u.Id == userId))
        {
            return new GroupMembershipResponse
            {
                Success = false,
                Message = "User is already a member of this group"
            };
        }

        group.Users.Add(user);
        _dbContext.NtripGroups.Update(group);
        await _dbContext.SaveChangesAsync();

        _logger.LogInformation($"User {user.Email} added to group {group.Name}");

        return new GroupMembershipResponse
        {
            Success = true,
            Message = "User added to group successfully",
            Group = MapToGroupDto(group)
        };
    }

    public async Task<GroupMembershipResponse> RemoveUserFromGroupAsync(int groupId, string userId)
    {
        var group = await _dbContext.NtripGroups
            .Include(g => g.Users)
            .Include(g => g.MountPoints)
            .FirstOrDefaultAsync(g => g.Id == groupId);

        if (group == null)
        {
            return new GroupMembershipResponse
            {
                Success = false,
                Message = "Group not found"
            };
        }

        var user = group.Users.FirstOrDefault(u => u.Id == userId);
        if (user == null)
        {
            return new GroupMembershipResponse
            {
                Success = false,
                Message = "User is not a member of this group"
            };
        }

        group.Users.Remove(user);
        _dbContext.NtripGroups.Update(group);
        await _dbContext.SaveChangesAsync();

        _logger.LogInformation($"User {user.Email} removed from group {group.Name}");

        return new GroupMembershipResponse
        {
            Success = true,
            Message = "User removed from group successfully",
            Group = MapToGroupDto(group)
        };
    }

    private GroupDto MapToGroupDto(NtripGroup group)
    {
        return new GroupDto
        {
            Id = group.Id,
            Name = group.Name,
            Description = group.Description,
            IsActive = group.IsActive,
            CreatedAt = group.CreatedAt,
            UserCount = group.Users?.Count ?? 0,
            UserEmails = group.Users?.Select(u => u.Email ?? string.Empty).ToList() ?? new(),
            MountPointNames = group.MountPoints?.Select(m => m.Name).ToList() ?? new()
        };
    }
}
