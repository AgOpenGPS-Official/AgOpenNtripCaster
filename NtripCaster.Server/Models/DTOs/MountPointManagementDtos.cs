namespace NtripCaster.Server.Models.DTOs;

/// <summary>
/// Create mount point request
/// </summary>
public class CreateMountPointRequest
{
    public string Name { get; set; } = string.Empty;
    public string Description { get; set; } = string.Empty;
    public string SourcePassword { get; set; } = string.Empty;
    public bool RequireClientAuthentication { get; set; } = true;
    public bool IsActive { get; set; } = true;
    public List<int> AllowedGroupIds { get; set; } = new();
}

/// <summary>
/// Update mount point request
/// </summary>
public class UpdateMountPointRequest
{
    public string? Description { get; set; }
    public string? SourcePassword { get; set; }
    public bool? RequireClientAuthentication { get; set; }
    public bool? IsActive { get; set; }
    public List<int>? AllowedGroupIds { get; set; }
}

/// <summary>
/// Mount point data transfer object
/// </summary>
public class MountPointDto
{
    public int Id { get; set; }
    public string Name { get; set; } = string.Empty;
    public string Description { get; set; } = string.Empty;
    public bool RequireClientAuthentication { get; set; }
    public bool IsActive { get; set; }
    public DateTime CreatedAt { get; set; }
    public int ActiveSourceCount { get; set; }
    public int ActiveClientCount { get; set; }
    public List<string> AllowedGroupNames { get; set; } = new();
    public string? UserId { get; set; }
    public string? OwnerFullName { get; set; }
    public string? OwnerEmail { get; set; }
}

/// <summary>
/// Mount point list response
/// </summary>
public class MountPointListResponse
{
    public int Total { get; set; }
    public int Page { get; set; }
    public int PageSize { get; set; }
    public List<MountPointDto> MountPoints { get; set; } = new();
}

/// <summary>
/// Create mount point response
/// </summary>
public class CreateMountPointResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
    public MountPointDto? MountPoint { get; set; }
}

/// <summary>
/// Update mount point response
/// </summary>
public class UpdateMountPointResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
    public MountPointDto? MountPoint { get; set; }
}

/// <summary>
/// Delete mount point response
/// </summary>
public class DeleteMountPointResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
}

/// <summary>
/// Allow group to access mount point
/// </summary>
public class AllowGroupRequest
{
    public int GroupId { get; set; }
}

/// <summary>
/// Deny group access to mount point
/// </summary>
public class DenyGroupRequest
{
    public int GroupId { get; set; }
}

/// <summary>
/// Mount point permission response
/// </summary>
public class MountPointPermissionResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
    public MountPointDto? MountPoint { get; set; }
}
