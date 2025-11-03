namespace AgOpenNtripCaster.Server.Models.DTOs;

/// <summary>
/// User registration request with email verification
/// </summary>
public class RegisterRequest
{
    public string Email { get; set; } = string.Empty;
    public string FullName { get; set; } = string.Empty;
    public string Password { get; set; } = string.Empty;
    public string PasswordConfirm { get; set; } = string.Empty;
}

/// <summary>
/// User registration response
/// </summary>
public class RegisterResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
    public string? UserId { get; set; }
    public string? Email { get; set; }
}

/// <summary>
/// Email verification request
/// </summary>
public class VerifyEmailRequest
{
    public string Token { get; set; } = string.Empty;
    public string Email { get; set; } = string.Empty;
}

/// <summary>
/// Email verification response
/// </summary>
public class VerifyEmailResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
}

/// <summary>
/// User login request (email + password)
/// </summary>
public class LoginRequest
{
    public string Email { get; set; } = string.Empty;
    public string Password { get; set; } = string.Empty;
}

/// <summary>
/// User login response
/// </summary>
public class LoginResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
    public string? AccessToken { get; set; }
    public string? RefreshToken { get; set; }
    public UserDto? User { get; set; }
}

/// <summary>
/// Token refresh request
/// </summary>
public class RefreshTokenRequest
{
    public string RefreshToken { get; set; } = string.Empty;
}

/// <summary>
/// Token refresh response
/// </summary>
public class RefreshTokenResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
    public string? AccessToken { get; set; }
    public string? RefreshToken { get; set; }
}

/// <summary>
/// User data transfer object
/// </summary>
public class UserDto
{
    public string Id { get; set; } = string.Empty;
    public string Email { get; set; } = string.Empty;
    public string UserName { get; set; } = string.Empty;
    public string FullName { get; set; } = string.Empty;
    public bool EmailConfirmed { get; set; }
    public DateTime CreatedAt { get; set; }
    public int MaxConnections { get; set; }
    public bool IsActive { get; set; }
    public List<string> Groups { get; set; } = new();
    public List<string> Roles { get; set; } = new();
}
