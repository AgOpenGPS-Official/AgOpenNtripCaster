namespace AgOpenNtripCaster.Server.Models.DTOs;

/// <summary>
/// Create group request
/// </summary>
public class CreateGroupRequest
{
    public string Name { get; set; } = string.Empty;
    public string Description { get; set; } = string.Empty;
    public bool IsActive { get; set; } = true;
}

/// <summary>
/// Update group request
/// </summary>
public class UpdateGroupRequest
{
    public string? Name { get; set; }
    public string? Description { get; set; }
    public bool? IsActive { get; set; }
}

/// <summary>
/// Group data transfer object
/// </summary>
public class GroupDto
{
    public int Id { get; set; }
    public string Name { get; set; } = string.Empty;
    public string Description { get; set; } = string.Empty;
    public bool IsActive { get; set; }
    public DateTime CreatedAt { get; set; }
    public int UserCount { get; set; }
    public List<string> UserEmails { get; set; } = new();
    public List<string> MountPointNames { get; set; } = new();
}

/// <summary>
/// Group list response
/// </summary>
public class GroupListResponse
{
    public int Total { get; set; }
    public int Page { get; set; }
    public int PageSize { get; set; }
    public List<GroupDto> Groups { get; set; } = new();
}

/// <summary>
/// Create group response
/// </summary>
public class CreateGroupResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
    public GroupDto? Group { get; set; }
}

/// <summary>
/// Update group response
/// </summary>
public class UpdateGroupResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
    public GroupDto? Group { get; set; }
}

/// <summary>
/// Delete group response
/// </summary>
public class DeleteGroupResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
}

/// <summary>
/// Add user to group request
/// </summary>
public class AddUserToGroupRequest
{
    public string UserId { get; set; } = string.Empty;
}

/// <summary>
/// Remove user from group request
/// </summary>
public class RemoveUserFromGroupRequest
{
    public string UserId { get; set; } = string.Empty;
}

/// <summary>
/// Group membership response
/// </summary>
public class GroupMembershipResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
    public GroupDto? Group { get; set; }
}
