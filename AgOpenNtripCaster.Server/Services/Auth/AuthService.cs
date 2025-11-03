using Microsoft.AspNetCore.Identity;
using Microsoft.EntityFrameworkCore;
using Microsoft.IdentityModel.Tokens;
using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models;
using AgOpenNtripCaster.Server.Models.DTOs;
using AgOpenNtripCaster.Server.Models.Entities;
using AgOpenNtripCaster.Server.Services.Email;
using System.IdentityModel.Tokens.Jwt;
using System.Security.Claims;
using System.Text;

namespace AgOpenNtripCaster.Server.Services.Auth;

/// <summary>
/// Service for handling authentication operations
/// </summary>
public interface IAuthService
{
    Task<RegisterResponse> RegisterAsync(RegisterRequest request, string baseUrl);
    Task<VerifyEmailResponse> VerifyEmailAsync(VerifyEmailRequest request);
    Task<LoginResponse> LoginAsync(LoginRequest request);
    Task<RefreshTokenResponse> RefreshTokenAsync(RefreshTokenRequest request);
}

public class AuthService : IAuthService
{
    private readonly UserManager<NtripUser> _userManager;
    private readonly IConfiguration _configuration;
    private readonly IEmailService _emailService;
    private readonly ApplicationDbContext _dbContext;
    private readonly ILogger<AuthService> _logger;

    public AuthService(
        UserManager<NtripUser> userManager,
        IConfiguration configuration,
        IEmailService emailService,
        ApplicationDbContext dbContext,
        ILogger<AuthService> logger)
    {
        _userManager = userManager;
        _configuration = configuration;
        _emailService = emailService;
        _dbContext = dbContext;
        _logger = logger;
    }

    public async Task<RegisterResponse> RegisterAsync(RegisterRequest request, string baseUrl)
    {
        // Validate input
        if (string.IsNullOrWhiteSpace(request.Email) || string.IsNullOrWhiteSpace(request.Password))
        {
            return new RegisterResponse
            {
                Success = false,
                Message = "Email and password are required"
            };
        }

        if (request.Password != request.PasswordConfirm)
        {
            return new RegisterResponse
            {
                Success = false,
                Message = "Passwords do not match"
            };
        }

        if (request.Password.Length < 8)
        {
            return new RegisterResponse
            {
                Success = false,
                Message = "Password must be at least 8 characters"
            };
        }

        // Check if email already exists
        var existingUser = await _userManager.FindByEmailAsync(request.Email);
        if (existingUser != null)
        {
            return new RegisterResponse
            {
                Success = false,
                Message = "Email already registered"
            };
        }

        // Create user
        var user = new NtripUser
        {
            UserName = request.Email,
            Email = request.Email,
            FullName = request.FullName,
            CreatedAt = DateTime.UtcNow,
            IsActive = true,
            MaxConnections = 5
        };

        var result = await _userManager.CreateAsync(user, request.Password);
        if (!result.Succeeded)
        {
            var errors = string.Join(", ", result.Errors.Select(e => e.Description));
            _logger.LogWarning($"User creation failed for {request.Email}: {errors}");
            return new RegisterResponse
            {
                Success = false,
                Message = "User creation failed: " + errors
            };
        }

        // Generate email verification token
        var emailToken = await _userManager.GenerateEmailConfirmationTokenAsync(user);
        var verificationLink = $"{baseUrl}/auth/verify-email?email={Uri.EscapeDataString(user.Email)}&token={Uri.EscapeDataString(emailToken)}";

        // Check if email verification is required
        var emailVerificationRequired = _configuration.GetValue<bool>("Email:VerificationRequired");

        if (emailVerificationRequired)
        {
            // Send verification email
            var emailSent = await _emailService.SendVerificationEmailAsync(user.Email, user.FullName, verificationLink);
            if (!emailSent)
            {
                _logger.LogWarning($"Failed to send verification email to {user.Email}");
            }
        }
        else
        {
            // Auto-confirm email if verification is disabled
            var confirmResult = await _userManager.ConfirmEmailAsync(user, emailToken);
            if (confirmResult.Succeeded)
            {
                _logger.LogInformation($"Email auto-confirmed for {user.Email} (verification disabled)");
            }
        }

        _logger.LogInformation($"User registered: {user.Email}");

        return new RegisterResponse
        {
            Success = true,
            Message = emailVerificationRequired ? "User registered. Please check your email to verify your account." : "User registered. Welcome!",
            UserId = user.Id,
            Email = user.Email
        };
    }

    public async Task<VerifyEmailResponse> VerifyEmailAsync(VerifyEmailRequest request)
    {
        if (string.IsNullOrWhiteSpace(request.Email) || string.IsNullOrWhiteSpace(request.Token))
        {
            return new VerifyEmailResponse
            {
                Success = false,
                Message = "Email and token are required"
            };
        }

        var user = await _userManager.FindByEmailAsync(request.Email);
        if (user == null)
        {
            return new VerifyEmailResponse
            {
                Success = false,
                Message = "User not found"
            };
        }

        if (user.EmailConfirmed)
        {
            return new VerifyEmailResponse
            {
                Success = true,
                Message = "Email already verified"
            };
        }

        var result = await _userManager.ConfirmEmailAsync(user, request.Token);
        if (!result.Succeeded)
        {
            var errors = string.Join(", ", result.Errors.Select(e => e.Description));
            _logger.LogWarning($"Email confirmation failed for {user.Email}: {errors}");
            return new VerifyEmailResponse
            {
                Success = false,
                Message = "Email verification failed: " + errors
            };
        }

        // Send welcome email
        await _emailService.SendWelcomeEmailAsync(user.Email, user.FullName);

        _logger.LogInformation($"Email verified for {user.Email}");

        return new VerifyEmailResponse
        {
            Success = true,
            Message = "Email verified successfully. You can now log in."
        };
    }

    public async Task<LoginResponse> LoginAsync(LoginRequest request)
    {
        if (string.IsNullOrWhiteSpace(request.Email) || string.IsNullOrWhiteSpace(request.Password))
        {
            return new LoginResponse
            {
                Success = false,
                Message = "Email and password are required"
            };
        }

        var user = await _userManager.FindByEmailAsync(request.Email);
        if (user == null)
        {
            return new LoginResponse
            {
                Success = false,
                Message = "Invalid email or password"
            };
        }

        // Check if user is active
        if (!user.IsActive)
        {
            _logger.LogWarning($"Login attempt for inactive user: {user.Email}");
            return new LoginResponse
            {
                Success = false,
                Message = "Account is disabled"
            };
        }

        // Check if email is confirmed
        if (!user.EmailConfirmed)
        {
            return new LoginResponse
            {
                Success = false,
                Message = "Please verify your email before logging in"
            };
        }

        // Verify password
        var passwordValid = await _userManager.CheckPasswordAsync(user, request.Password);
        if (!passwordValid)
        {
            _logger.LogWarning($"Invalid password attempt for {user.Email}");
            return new LoginResponse
            {
                Success = false,
                Message = "Invalid email or password"
            };
        }

        // Generate tokens
        var accessToken = await GenerateAccessTokenAsync(user);
        var refreshToken = GenerateRefreshToken();

        // Save refresh token
        user.RefreshToken = refreshToken;
        user.RefreshTokenExpires = DateTime.UtcNow.AddDays(7);
        await _userManager.UpdateAsync(user);

        // Get user's groups
        var groups = user.Groups?.Select(g => g.Name).ToList() ?? new();

        // Get user's roles
        var roles = await _userManager.GetRolesAsync(user);

        _logger.LogInformation($"User logged in: {user.Email}");

        return new LoginResponse
        {
            Success = true,
            Message = "Login successful",
            AccessToken = accessToken,
            RefreshToken = refreshToken,
            User = new UserDto
            {
                Id = user.Id,
                Email = user.Email,
                UserName = user.Email.Split('@')[0],
                FullName = user.FullName,
                EmailConfirmed = user.EmailConfirmed,
                CreatedAt = user.CreatedAt,
                MaxConnections = user.MaxConnections,
                IsActive = user.IsActive,
                Groups = groups,
                Roles = roles.ToList()
            }
        };
    }

    public async Task<RefreshTokenResponse> RefreshTokenAsync(RefreshTokenRequest request)
    {
        if (string.IsNullOrWhiteSpace(request.RefreshToken))
        {
            return new RefreshTokenResponse
            {
                Success = false,
                Message = "Refresh token is required"
            };
        }

        var user = await _userManager.Users
            .FirstOrDefaultAsync(u => u.RefreshToken == request.RefreshToken);

        if (user == null || user.RefreshTokenExpires < DateTime.UtcNow)
        {
            return new RefreshTokenResponse
            {
                Success = false,
                Message = "Invalid or expired refresh token"
            };
        }

        if (!user.IsActive || !user.EmailConfirmed)
        {
            return new RefreshTokenResponse
            {
                Success = false,
                Message = "User account is not active or email not verified"
            };
        }

        // Generate new tokens
        var accessToken = await GenerateAccessTokenAsync(user);
        var refreshToken = GenerateRefreshToken();

        // Save new refresh token
        user.RefreshToken = refreshToken;
        user.RefreshTokenExpires = DateTime.UtcNow.AddDays(7);
        await _userManager.UpdateAsync(user);

        _logger.LogInformation($"Token refreshed for user: {user.Email}");

        return new RefreshTokenResponse
        {
            Success = true,
            Message = "Token refreshed successfully",
            AccessToken = accessToken,
            RefreshToken = refreshToken
        };
    }

    private async Task<string> GenerateAccessTokenAsync(NtripUser user)
    {
        // Use same secret as in Program.cs for JWT validation
        var jwtSecret = Environment.GetEnvironmentVariable("JWT_SECRET") ?? "your-secret-key-here-min-32-chars";
        var key = new SymmetricSecurityKey(Encoding.ASCII.GetBytes(jwtSecret));
        var creds = new SigningCredentials(key, SecurityAlgorithms.HmacSha256);

        var claims = new List<Claim>
        {
            new Claim(ClaimTypes.NameIdentifier, user.Id),
            new Claim(ClaimTypes.Email, user.Email),
            new Claim(ClaimTypes.Name, user.FullName)
        };

        // Add user roles as claims
        var roles = await _userManager.GetRolesAsync(user);
        foreach (var role in roles)
        {
            claims.Add(new Claim(ClaimTypes.Role, role));
        }

        // Add user groups as claims
        if (user.Groups != null)
        {
            foreach (var group in user.Groups)
            {
                claims.Add(new Claim("groups", group.Name));
            }
        }

        var token = new JwtSecurityToken(
            issuer: _configuration["Jwt:Issuer"],
            audience: _configuration["Jwt:Audience"],
            claims: claims,
            expires: DateTime.UtcNow.AddHours(1),
            signingCredentials: creds);

        return new JwtSecurityTokenHandler().WriteToken(token);
    }

    private string GenerateRefreshToken()
    {
        var randomNumber = new byte[32];
        using (var rng = System.Security.Cryptography.RandomNumberGenerator.Create())
        {
            rng.GetBytes(randomNumber);
            return Convert.ToBase64String(randomNumber);
        }
    }
}
