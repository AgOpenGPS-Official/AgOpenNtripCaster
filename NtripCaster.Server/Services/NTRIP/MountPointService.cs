using Microsoft.EntityFrameworkCore;
using NtripCaster.Server.Data;
using NtripCaster.Server.Models.DTOs;
using NtripCaster.Server.Models.Entities;

namespace NtripCaster.Server.Services.NTRIP;

/// <summary>
/// Service for mount point management
/// </summary>
public interface IMountPointService
{
    Task<MountPointListResponse> GetMountPointsAsync(int page = 1, int pageSize = 10);
    Task<MountPointListResponse> GetUserMountPointsAsync(string userId, int page = 1, int pageSize = 10);  // Get only user's own sources
    Task<MountPointDto?> GetMountPointByIdAsync(int mountPointId);
    Task<MountPointDto?> GetMountPointByNameAsync(string name);
    Task<CreateMountPointResponse> CreateMountPointAsync(CreateMountPointRequest request, string userId);
    Task<UpdateMountPointResponse> UpdateMountPointAsync(int mountPointId, UpdateMountPointRequest request);
    Task<DeleteMountPointResponse> DeleteMountPointAsync(int mountPointId);
    Task<MountPointPermissionResponse> AllowGroupAsync(int mountPointId, int groupId);
    Task<MountPointPermissionResponse> DenyGroupAsync(int mountPointId, int groupId);
}

public class MountPointService : IMountPointService
{
    private readonly ApplicationDbContext _dbContext;
    private readonly ConnectionPool _connectionPool;
    private readonly ILogger<MountPointService> _logger;

    public MountPointService(
        ApplicationDbContext dbContext,
        ConnectionPool connectionPool,
        ILogger<MountPointService> logger)
    {
        _dbContext = dbContext;
        _connectionPool = connectionPool;
        _logger = logger;
    }

    public async Task<MountPointListResponse> GetMountPointsAsync(int page = 1, int pageSize = 10)
    {
        var mountPoints = await _dbContext.MountPoints
            .Include(m => m.AllowedGroups)
            .Skip((page - 1) * pageSize)
            .Take(pageSize)
            .ToListAsync();

        var total = await _dbContext.MountPoints.CountAsync();

        return new MountPointListResponse
        {
            Total = total,
            Page = page,
            PageSize = pageSize,
            MountPoints = mountPoints.Select(m => MapToMountPointDto(m)).ToList()
        };
    }

    public async Task<MountPointListResponse> GetUserMountPointsAsync(string userId, int page = 1, int pageSize = 10)
    {
        var mountPoints = await _dbContext.MountPoints
            .Where(m => m.UserId == userId)
            .Include(m => m.AllowedGroups)
            .Skip((page - 1) * pageSize)
            .Take(pageSize)
            .ToListAsync();

        var total = await _dbContext.MountPoints
            .Where(m => m.UserId == userId)
            .CountAsync();

        return new MountPointListResponse
        {
            Total = total,
            Page = page,
            PageSize = pageSize,
            MountPoints = mountPoints.Select(m => MapToMountPointDto(m)).ToList()
        };
    }

    public async Task<MountPointDto?> GetMountPointByIdAsync(int mountPointId)
    {
        var mountPoint = await _dbContext.MountPoints
            .Include(m => m.AllowedGroups)
            .FirstOrDefaultAsync(m => m.Id == mountPointId);

        if (mountPoint == null)
        {
            return null;
        }

        return MapToMountPointDto(mountPoint);
    }

    public async Task<MountPointDto?> GetMountPointByNameAsync(string name)
    {
        var mountPoint = await _dbContext.MountPoints
            .Include(m => m.AllowedGroups)
            .FirstOrDefaultAsync(m => m.Name == name);

        if (mountPoint == null)
        {
            return null;
        }

        return MapToMountPointDto(mountPoint);
    }

    public async Task<CreateMountPointResponse> CreateMountPointAsync(CreateMountPointRequest request, string userId)
    {
        // Validate input
        if (string.IsNullOrWhiteSpace(request.Name))
        {
            return new CreateMountPointResponse
            {
                Success = false,
                Message = "Mount point name is required"
            };
        }

        if (string.IsNullOrWhiteSpace(request.SourcePassword))
        {
            return new CreateMountPointResponse
            {
                Success = false,
                Message = "Source password is required"
            };
        }

        // Check if mount point name already exists
        var existing = await _dbContext.MountPoints
            .FirstOrDefaultAsync(m => m.Name == request.Name);

        if (existing != null)
        {
            return new CreateMountPointResponse
            {
                Success = false,
                Message = "Mount point name already exists"
            };
        }

        // Create mount point
        var mountPoint = new MountPoint
        {
            Name = request.Name,
            Description = request.Description,
            SourcePassword = request.SourcePassword,
            UserId = userId,  // Set the owner of this source
            RequireClientAuthentication = request.RequireClientAuthentication,
            IsActive = request.IsActive,
            CreatedAt = DateTime.UtcNow
        };

        // Add allowed groups
        if (request.AllowedGroupIds.Any())
        {
            var groups = await _dbContext.NtripGroups
                .Where(g => request.AllowedGroupIds.Contains(g.Id))
                .ToListAsync();

            mountPoint.AllowedGroups = groups;
        }

        _dbContext.MountPoints.Add(mountPoint);
        await _dbContext.SaveChangesAsync();

        _logger.LogInformation($"Mount point created: {mountPoint.Name}");

        return new CreateMountPointResponse
        {
            Success = true,
            Message = "Mount point created successfully",
            MountPoint = MapToMountPointDto(mountPoint)
        };
    }

    public async Task<UpdateMountPointResponse> UpdateMountPointAsync(int mountPointId, UpdateMountPointRequest request)
    {
        var mountPoint = await _dbContext.MountPoints
            .Include(m => m.AllowedGroups)
            .FirstOrDefaultAsync(m => m.Id == mountPointId);

        if (mountPoint == null)
        {
            return new UpdateMountPointResponse
            {
                Success = false,
                Message = "Mount point not found"
            };
        }

        // Update fields
        if (request.Description != null)
        {
            mountPoint.Description = request.Description;
        }

        if (!string.IsNullOrWhiteSpace(request.SourcePassword))
        {
            mountPoint.SourcePassword = request.SourcePassword;
        }

        if (request.RequireClientAuthentication.HasValue)
        {
            mountPoint.RequireClientAuthentication = request.RequireClientAuthentication.Value;
        }

        if (request.IsActive.HasValue)
        {
            mountPoint.IsActive = request.IsActive.Value;
        }

        // Update allowed groups if provided
        if (request.AllowedGroupIds != null)
        {
            var groups = await _dbContext.NtripGroups
                .Where(g => request.AllowedGroupIds.Contains(g.Id))
                .ToListAsync();

            mountPoint.AllowedGroups = groups;
        }

        _dbContext.MountPoints.Update(mountPoint);
        await _dbContext.SaveChangesAsync();

        _logger.LogInformation($"Mount point updated: {mountPoint.Name}");

        return new UpdateMountPointResponse
        {
            Success = true,
            Message = "Mount point updated successfully",
            MountPoint = MapToMountPointDto(mountPoint)
        };
    }

    public async Task<DeleteMountPointResponse> DeleteMountPointAsync(int mountPointId)
    {
        var mountPoint = await _dbContext.MountPoints
            .FirstOrDefaultAsync(m => m.Id == mountPointId);

        if (mountPoint == null)
        {
            return new DeleteMountPointResponse
            {
                Success = false,
                Message = "Mount point not found"
            };
        }

        _dbContext.MountPoints.Remove(mountPoint);
        await _dbContext.SaveChangesAsync();

        _logger.LogInformation($"Mount point deleted: {mountPoint.Name}");

        return new DeleteMountPointResponse
        {
            Success = true,
            Message = "Mount point deleted successfully"
        };
    }

    public async Task<MountPointPermissionResponse> AllowGroupAsync(int mountPointId, int groupId)
    {
        var mountPoint = await _dbContext.MountPoints
            .Include(m => m.AllowedGroups)
            .FirstOrDefaultAsync(m => m.Id == mountPointId);

        if (mountPoint == null)
        {
            return new MountPointPermissionResponse
            {
                Success = false,
                Message = "Mount point not found"
            };
        }

        var group = await _dbContext.NtripGroups.FirstOrDefaultAsync(g => g.Id == groupId);
        if (group == null)
        {
            return new MountPointPermissionResponse
            {
                Success = false,
                Message = "Group not found"
            };
        }

        // Check if group already has access
        if (mountPoint.AllowedGroups.Any(g => g.Id == groupId))
        {
            return new MountPointPermissionResponse
            {
                Success = false,
                Message = "Group already has access to this mount point"
            };
        }

        mountPoint.AllowedGroups.Add(group);
        _dbContext.MountPoints.Update(mountPoint);
        await _dbContext.SaveChangesAsync();

        _logger.LogInformation($"Group {group.Name} allowed access to mount point {mountPoint.Name}");

        return new MountPointPermissionResponse
        {
            Success = true,
            Message = "Group access granted successfully",
            MountPoint = MapToMountPointDto(mountPoint)
        };
    }

    public async Task<MountPointPermissionResponse> DenyGroupAsync(int mountPointId, int groupId)
    {
        var mountPoint = await _dbContext.MountPoints
            .Include(m => m.AllowedGroups)
            .FirstOrDefaultAsync(m => m.Id == mountPointId);

        if (mountPoint == null)
        {
            return new MountPointPermissionResponse
            {
                Success = false,
                Message = "Mount point not found"
            };
        }

        var group = mountPoint.AllowedGroups.FirstOrDefault(g => g.Id == groupId);
        if (group == null)
        {
            return new MountPointPermissionResponse
            {
                Success = false,
                Message = "Group does not have access to this mount point"
            };
        }

        mountPoint.AllowedGroups.Remove(group);
        _dbContext.MountPoints.Update(mountPoint);
        await _dbContext.SaveChangesAsync();

        _logger.LogInformation($"Group {group.Name} denied access to mount point {mountPoint.Name}");

        return new MountPointPermissionResponse
        {
            Success = true,
            Message = "Group access revoked successfully",
            MountPoint = MapToMountPointDto(mountPoint)
        };
    }

    private MountPointDto MapToMountPointDto(MountPoint mountPoint)
    {
        var clients = _connectionPool.GetClientsForMountPoint(mountPoint.Name);
        var source = _connectionPool.GetSourceForMountPoint(mountPoint.Name);

        return new MountPointDto
        {
            Id = mountPoint.Id,
            Name = mountPoint.Name,
            Description = mountPoint.Description,
            RequireClientAuthentication = mountPoint.RequireClientAuthentication,
            IsActive = mountPoint.IsActive,
            CreatedAt = mountPoint.CreatedAt,
            ActiveSourceCount = source != null && !source.IsDisconnected ? 1 : 0,
            ActiveClientCount = clients.Count,
            AllowedGroupNames = mountPoint.AllowedGroups?.Select(g => g.Name).ToList() ?? new()
        };
    }
}
