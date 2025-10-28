namespace NtripCaster.Server.Models.DTOs;

/// <summary>
/// Create user request (admin only)
/// </summary>
public class CreateUserRequest
{
    public string Email { get; set; } = string.Empty;
    public string FullName { get; set; } = string.Empty;
    public string Password { get; set; } = string.Empty;
    public int MaxConnections { get; set; } = 5;
    public bool IsActive { get; set; } = true;
    public List<int> GroupIds { get; set; } = new();
}

/// <summary>
/// Update user request
/// </summary>
public class UpdateUserRequest
{
    public string? FullName { get; set; }
    public string? CurrentPassword { get; set; }
    public string? NewPassword { get; set; }
    public int? MaxConnections { get; set; }
    public bool? IsActive { get; set; }
    public List<int>? GroupIds { get; set; }
}

/// <summary>
/// Change password request
/// </summary>
public class ChangePasswordRequest
{
    public string CurrentPassword { get; set; } = string.Empty;
    public string NewPassword { get; set; } = string.Empty;
    public string ConfirmPassword { get; set; } = string.Empty;
}

/// <summary>
/// Password change response
/// </summary>
public class ChangePasswordResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
}

/// <summary>
/// User list response
/// </summary>
public class UserListResponse
{
    public int Total { get; set; }
    public int Page { get; set; }
    public int PageSize { get; set; }
    public List<UserDto> Users { get; set; } = new();
}

/// <summary>
/// User creation response
/// </summary>
public class CreateUserResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
    public UserDto? User { get; set; }
}

/// <summary>
/// User update response
/// </summary>
public class UpdateUserResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
    public UserDto? User { get; set; }
}

/// <summary>
/// User deletion response
/// </summary>
public class DeleteUserResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
}
