using Microsoft.EntityFrameworkCore;
using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.DTOs;
using AgOpenNtripCaster.Server.Models.Entities;

namespace AgOpenNtripCaster.Server.Services.NTRIP;

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
    Task<UpdateMountPointResponse> UpdateMountPointAsync(int mountPointId, UpdateMountPointRequest request, string? userId = null, bool isAdmin = false);
    Task<DeleteMountPointResponse> DeleteMountPointAsync(int mountPointId, string? userId = null, bool isAdmin = false);
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
            .Include(m => m.Owner)
            .Skip((page - 1) * pageSize)
            .Take(pageSize)
            .ToListAsync();

        var total = await _dbContext.MountPoints.CountAsync();

        // Batch load all source/client counts to avoid concurrent DbContext access
        var mountPointIds = mountPoints.Select(m => m.Id).ToList();

        var sourceCounts = await _dbContext.SourceConnections
            .Where(sc => mountPointIds.Contains(sc.MountPointId) && sc.DisconnectedAt == null)
            .GroupBy(sc => sc.MountPointId)
            .Select(g => new { MountPointId = g.Key, Count = g.Count() })
            .ToDictionaryAsync(x => x.MountPointId, x => x.Count);

        var clientCounts = await _dbContext.ClientSessions
            .Where(cs => mountPointIds.Contains(cs.MountPointId) && cs.DisconnectedAt == null)
            .GroupBy(cs => cs.MountPointId)
            .Select(g => new { MountPointId = g.Key, Count = g.Count() })
            .ToDictionaryAsync(x => x.MountPointId, x => x.Count);

        // Map synchronously with pre-loaded counts
        var mountPointDtos = mountPoints.Select(m => MapToMountPointDto(m, sourceCounts, clientCounts)).ToList();

        return new MountPointListResponse
        {
            Total = total,
            Page = page,
            PageSize = pageSize,
            MountPoints = mountPointDtos
        };
    }

    public async Task<MountPointListResponse> GetUserMountPointsAsync(string userId, int page = 1, int pageSize = 10)
    {
        var mountPoints = await _dbContext.MountPoints
            .Where(m => m.UserId == userId)
            .Include(m => m.AllowedGroups)
            .Include(m => m.Owner)
            .Skip((page - 1) * pageSize)
            .Take(pageSize)
            .ToListAsync();

        var total = await _dbContext.MountPoints
            .Where(m => m.UserId == userId)
            .CountAsync();

        // Batch load all source/client counts to avoid concurrent DbContext access
        var mountPointIds = mountPoints.Select(m => m.Id).ToList();

        var sourceCounts = await _dbContext.SourceConnections
            .Where(sc => mountPointIds.Contains(sc.MountPointId) && sc.DisconnectedAt == null)
            .GroupBy(sc => sc.MountPointId)
            .Select(g => new { MountPointId = g.Key, Count = g.Count() })
            .ToDictionaryAsync(x => x.MountPointId, x => x.Count);

        var clientCounts = await _dbContext.ClientSessions
            .Where(cs => mountPointIds.Contains(cs.MountPointId) && cs.DisconnectedAt == null)
            .GroupBy(cs => cs.MountPointId)
            .Select(g => new { MountPointId = g.Key, Count = g.Count() })
            .ToDictionaryAsync(x => x.MountPointId, x => x.Count);

        // Map synchronously with pre-loaded counts
        var mountPointDtos = mountPoints.Select(m => MapToMountPointDto(m, sourceCounts, clientCounts)).ToList();

        return new MountPointListResponse
        {
            Total = total,
            Page = page,
            PageSize = pageSize,
            MountPoints = mountPointDtos
        };
    }

    public async Task<MountPointDto?> GetMountPointByIdAsync(int mountPointId)
    {
        var mountPoint = await _dbContext.MountPoints
            .Include(m => m.AllowedGroups)
            .Include(m => m.Owner)
            .FirstOrDefaultAsync(m => m.Id == mountPointId);

        if (mountPoint == null)
        {
            return null;
        }

        return await MapToMountPointDtoAsync(mountPoint);
    }

    public async Task<MountPointDto?> GetMountPointByNameAsync(string name)
    {
        var mountPoint = await _dbContext.MountPoints
            .Include(m => m.AllowedGroups)
            .Include(m => m.Owner)
            .FirstOrDefaultAsync(m => m.Name == name);

        if (mountPoint == null)
        {
            return null;
        }

        return await MapToMountPointDtoAsync(mountPoint);
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
            Latitude = request.Latitude,
            Longitude = request.Longitude,
            RequireClientAuthentication = request.RequireClientAuthentication,
            IsActive = request.IsActive,
            CreatedAt = DateTime.UtcNow
        };

        // Add allowed groups
        if (request.AllowedGroupIds != null && request.AllowedGroupIds.Any())
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
            MountPoint = await MapToMountPointDtoAsync(mountPoint)
        };
    }

    public async Task<UpdateMountPointResponse> UpdateMountPointAsync(int mountPointId, UpdateMountPointRequest request, string? userId = null, bool isAdmin = false)
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

        // Check authorization: user can only update their own mount points, admins can update any
        if (!isAdmin && !string.IsNullOrEmpty(userId) && mountPoint.UserId != userId)
        {
            return new UpdateMountPointResponse
            {
                Success = false,
                Message = "not authorized to update this mount point"
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

        // Update fallback coordinates if provided
        if (request.Latitude.HasValue)
        {
            mountPoint.Latitude = request.Latitude.Value;
        }

        if (request.Longitude.HasValue)
        {
            mountPoint.Longitude = request.Longitude.Value;
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
            MountPoint = await MapToMountPointDtoAsync(mountPoint)
        };
    }

    public async Task<DeleteMountPointResponse> DeleteMountPointAsync(int mountPointId, string? userId = null, bool isAdmin = false)
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

        // Check authorization: user can only delete their own mount points, admins can delete any
        if (!isAdmin && !string.IsNullOrEmpty(userId) && mountPoint.UserId != userId)
        {
            return new DeleteMountPointResponse
            {
                Success = false,
                Message = "not authorized to delete this mount point"
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
            MountPoint = await MapToMountPointDtoAsync(mountPoint)
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
            MountPoint = await MapToMountPointDtoAsync(mountPoint)
        };
    }

    /// <summary>
    /// Map MountPoint to DTO with pre-loaded counts (for batch operations)
    /// </summary>
    private MountPointDto MapToMountPointDto(
        MountPoint mountPoint,
        Dictionary<int, int> sourceCounts,
        Dictionary<int, int> clientCounts)
    {
        // HYBRID STRATEGY: ConnectionPool (in-memory) with Database fallback (pre-loaded)

        // Try ConnectionPool first (fast, in-memory)
        var source = _connectionPool.GetSourceForMountPoint(mountPoint.Name);
        var clients = _connectionPool.GetClientsForMountPoint(mountPoint.Name);

        int activeSourceCount = source != null && !source.IsDisconnected ? 1 : 0;
        int activeClientCount = clients.Count;

        // Fallback to pre-loaded database counts if ConnectionPool shows no active sources
        if (activeSourceCount == 0 && sourceCounts.TryGetValue(mountPoint.Id, out var dbSourceCount))
        {
            activeSourceCount = dbSourceCount;
        }

        if (activeClientCount == 0 && clientCounts.TryGetValue(mountPoint.Id, out var dbClientCount))
        {
            activeClientCount = dbClientCount;
        }

        return new MountPointDto
        {
            Id = mountPoint.Id,
            Name = mountPoint.Name,
            Description = mountPoint.Description,
            RequireClientAuthentication = mountPoint.RequireClientAuthentication,
            IsActive = mountPoint.IsActive,
            CreatedAt = mountPoint.CreatedAt,

            // Fallback coordinates (manual entry)
            Latitude = mountPoint.Latitude,
            Longitude = mountPoint.Longitude,

            // RTCM-extracted coordinates
            RtcmLatitude = mountPoint.RtcmLatitude,
            RtcmLongitude = mountPoint.RtcmLongitude,
            ReferenceStationId = mountPoint.ReferenceStationId,

            // RTCM message tracking
            LastRtcmMessageTime = mountPoint.LastRtcmMessageTime,
            MessageCount = mountPoint.MessageCount,

            ActiveSourceCount = activeSourceCount,
            ActiveClientCount = activeClientCount,
            AllowedGroupNames = mountPoint.AllowedGroups?.Select(g => g.Name).ToList() ?? new(),
            UserId = mountPoint.UserId,
            OwnerFullName = mountPoint.Owner?.FullName ?? "System",
            OwnerEmail = mountPoint.Owner?.Email ?? "system@ntrip.local"
        };
    }

    /// <summary>
    /// Map MountPoint to DTO with async DB queries (for single items)
    /// </summary>
    private async Task<MountPointDto> MapToMountPointDtoAsync(MountPoint mountPoint)
    {
        // HYBRID STRATEGY: ConnectionPool (in-memory) with Database fallback

        // Try ConnectionPool first (fast, in-memory)
        var source = _connectionPool.GetSourceForMountPoint(mountPoint.Name);
        var clients = _connectionPool.GetClientsForMountPoint(mountPoint.Name);

        int activeSourceCount = source != null && !source.IsDisconnected ? 1 : 0;
        int activeClientCount = clients.Count;

        // Fallback to database if ConnectionPool shows no active sources
        // This handles server restarts where ConnectionPool is empty but DB has active connections
        if (activeSourceCount == 0)
        {
            activeSourceCount = await _dbContext.SourceConnections
                .CountAsync(sc => sc.MountPointId == mountPoint.Id && sc.DisconnectedAt == null);
        }

        // Fallback to database for client count if ConnectionPool is empty
        if (activeClientCount == 0)
        {
            activeClientCount = await _dbContext.ClientSessions
                .CountAsync(cs => cs.MountPointId == mountPoint.Id && cs.DisconnectedAt == null);
        }

        return new MountPointDto
        {
            Id = mountPoint.Id,
            Name = mountPoint.Name,
            Description = mountPoint.Description,
            RequireClientAuthentication = mountPoint.RequireClientAuthentication,
            IsActive = mountPoint.IsActive,
            CreatedAt = mountPoint.CreatedAt,

            // Fallback coordinates (manual entry)
            Latitude = mountPoint.Latitude,
            Longitude = mountPoint.Longitude,

            // RTCM-extracted coordinates
            RtcmLatitude = mountPoint.RtcmLatitude,
            RtcmLongitude = mountPoint.RtcmLongitude,
            ReferenceStationId = mountPoint.ReferenceStationId,

            // RTCM message tracking
            LastRtcmMessageTime = mountPoint.LastRtcmMessageTime,
            MessageCount = mountPoint.MessageCount,

            ActiveSourceCount = activeSourceCount,
            ActiveClientCount = activeClientCount,
            AllowedGroupNames = mountPoint.AllowedGroups?.Select(g => g.Name).ToList() ?? new(),
            UserId = mountPoint.UserId,
            OwnerFullName = mountPoint.Owner?.FullName ?? "System",
            OwnerEmail = mountPoint.Owner?.Email ?? "system@ntrip.local"
        };
    }
}
