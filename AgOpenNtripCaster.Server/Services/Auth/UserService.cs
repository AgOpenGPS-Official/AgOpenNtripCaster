using Microsoft.AspNetCore.Identity;
using Microsoft.EntityFrameworkCore;
using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.DTOs;
using AgOpenNtripCaster.Server.Models.Entities;

namespace AgOpenNtripCaster.Server.Services.Auth;

/// <summary>
/// Service for user management operations
/// </summary>
public interface IUserService
{
    Task<UserListResponse> GetUsersAsync(int page = 1, int pageSize = 10);
    Task<UserDto?> GetUserByIdAsync(string userId);
    Task<UserDto?> GetCurrentUserAsync(string userId);
    Task<CreateUserResponse> CreateUserAsync(CreateUserRequest request);
    Task<UpdateUserResponse> UpdateUserAsync(string userId, UpdateUserRequest request, string currentUserId, bool isAdmin);
    Task<ChangePasswordResponse> ChangePasswordAsync(string userId, ChangePasswordRequest request);
    Task<DeleteUserResponse> DeleteUserAsync(string userId);
}

public class UserService : IUserService
{
    private readonly UserManager<NtripUser> _userManager;
    private readonly ApplicationDbContext _dbContext;
    private readonly ILogger<UserService> _logger;

    public UserService(
        UserManager<NtripUser> userManager,
        ApplicationDbContext dbContext,
        ILogger<UserService> logger)
    {
        _userManager = userManager;
        _dbContext = dbContext;
        _logger = logger;
    }

    public async Task<UserListResponse> GetUsersAsync(int page = 1, int pageSize = 10)
    {
        var users = await _userManager.Users
            .Include(u => u.Groups)
            .Skip((page - 1) * pageSize)
            .Take(pageSize)
            .ToListAsync();

        var total = await _userManager.Users.CountAsync();

        return new UserListResponse
        {
            Total = total,
            Page = page,
            PageSize = pageSize,
            Users = users.Select(MapToUserDto).ToList()
        };
    }

    public async Task<UserDto?> GetUserByIdAsync(string userId)
    {
        var user = await _userManager.Users
            .Include(u => u.Groups)
            .FirstOrDefaultAsync(u => u.Id == userId);

        if (user == null)
        {
            return null;
        }

        return MapToUserDto(user);
    }

    public async Task<UserDto?> GetCurrentUserAsync(string userId)
    {
        var user = await _userManager.Users
            .Include(u => u.Groups)
            .FirstOrDefaultAsync(u => u.Id == userId);

        if (user == null)
        {
            return null;
        }

        var dto = MapToUserDto(user);
        var roles = await _userManager.GetRolesAsync(user);
        dto.Roles = roles.ToList();

        return dto;
    }

    public async Task<CreateUserResponse> CreateUserAsync(CreateUserRequest request)
    {
        // Validate input
        if (string.IsNullOrWhiteSpace(request.Email) || string.IsNullOrWhiteSpace(request.Password))
        {
            return new CreateUserResponse
            {
                Success = false,
                Message = "Email and password are required"
            };
        }

        if (request.Password.Length < 8)
        {
            return new CreateUserResponse
            {
                Success = false,
                Message = "Password must be at least 8 characters"
            };
        }

        // Check if email already exists
        var existingUser = await _userManager.FindByEmailAsync(request.Email);
        if (existingUser != null)
        {
            return new CreateUserResponse
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
            IsActive = request.IsActive,
            MaxConnections = request.MaxConnections,
            EmailConfirmed = true // Auto-confirm for admin-created users
        };

        var result = await _userManager.CreateAsync(user, request.Password);
        if (!result.Succeeded)
        {
            var errors = string.Join(", ", result.Errors.Select(e => e.Description));
            _logger.LogWarning($"User creation failed for {request.Email}: {errors}");
            return new CreateUserResponse
            {
                Success = false,
                Message = "User creation failed: " + errors
            };
        }

        // Add groups if provided
        if (request.GroupIds.Any())
        {
            var groups = await _dbContext.NtripGroups
                .Where(g => request.GroupIds.Contains(g.Id))
                .ToListAsync();

            user.Groups = groups;
            await _userManager.UpdateAsync(user);
        }

        _logger.LogInformation($"User created: {user.Email}");

        return new CreateUserResponse
        {
            Success = true,
            Message = "User created successfully",
            User = MapToUserDto(user)
        };
    }

    public async Task<UpdateUserResponse> UpdateUserAsync(string userId, UpdateUserRequest request, string currentUserId, bool isAdmin)
    {
        var user = await _userManager.FindByIdAsync(userId);
        if (user == null)
        {
            return new UpdateUserResponse
            {
                Success = false,
                Message = "User not found"
            };
        }

        // Users can only edit their own profile, unless admin
        if (userId != currentUserId && !isAdmin)
        {
            return new UpdateUserResponse
            {
                Success = false,
                Message = "You can only edit your own profile"
            };
        }

        // Update basic fields
        if (!string.IsNullOrWhiteSpace(request.FullName))
        {
            user.FullName = request.FullName;
        }

        // Admin only fields
        if (isAdmin)
        {
            if (request.IsActive.HasValue)
            {
                user.IsActive = request.IsActive.Value;
            }

            if (request.MaxConnections.HasValue)
            {
                user.MaxConnections = request.MaxConnections.Value;
            }

            // Update groups if provided
            if (request.GroupIds != null)
            {
                var groups = await _dbContext.NtripGroups
                    .Where(g => request.GroupIds.Contains(g.Id))
                    .ToListAsync();

                user.Groups = groups;
            }
        }

        // Handle password change
        if (!string.IsNullOrWhiteSpace(request.CurrentPassword) && !string.IsNullOrWhiteSpace(request.NewPassword))
        {
            if (request.NewPassword.Length < 8)
            {
                return new UpdateUserResponse
                {
                    Success = false,
                    Message = "New password must be at least 8 characters"
                };
            }

            var passwordValid = await _userManager.CheckPasswordAsync(user, request.CurrentPassword);
            if (!passwordValid)
            {
                return new UpdateUserResponse
                {
                    Success = false,
                    Message = "Current password is incorrect"
                };
            }

            var changeResult = await _userManager.ChangePasswordAsync(user, request.CurrentPassword, request.NewPassword);
            if (!changeResult.Succeeded)
            {
                var errors = string.Join(", ", changeResult.Errors.Select(e => e.Description));
                return new UpdateUserResponse
                {
                    Success = false,
                    Message = "Password change failed: " + errors
                };
            }
        }

        // Save changes
        var result = await _userManager.UpdateAsync(user);
        if (!result.Succeeded)
        {
            var errors = string.Join(", ", result.Errors.Select(e => e.Description));
            _logger.LogWarning($"User update failed for {user.Email}: {errors}");
            return new UpdateUserResponse
            {
                Success = false,
                Message = "User update failed: " + errors
            };
        }

        _logger.LogInformation($"User updated: {user.Email}");

        return new UpdateUserResponse
        {
            Success = true,
            Message = "User updated successfully",
            User = MapToUserDto(user)
        };
    }

    public async Task<ChangePasswordResponse> ChangePasswordAsync(string userId, ChangePasswordRequest request)
    {
        if (request.NewPassword != request.ConfirmPassword)
        {
            return new ChangePasswordResponse
            {
                Success = false,
                Message = "Passwords do not match"
            };
        }

        if (request.NewPassword.Length < 8)
        {
            return new ChangePasswordResponse
            {
                Success = false,
                Message = "Password must be at least 8 characters"
            };
        }

        var user = await _userManager.FindByIdAsync(userId);
        if (user == null)
        {
            return new ChangePasswordResponse
            {
                Success = false,
                Message = "User not found"
            };
        }

        var result = await _userManager.ChangePasswordAsync(user, request.CurrentPassword, request.NewPassword);
        if (!result.Succeeded)
        {
            var errors = string.Join(", ", result.Errors.Select(e => e.Description));
            _logger.LogWarning($"Password change failed for {user.Email}: {errors}");
            return new ChangePasswordResponse
            {
                Success = false,
                Message = "Password change failed: " + errors
            };
        }

        _logger.LogInformation($"Password changed for user: {user.Email}");

        return new ChangePasswordResponse
        {
            Success = true,
            Message = "Password changed successfully"
        };
    }

    public async Task<DeleteUserResponse> DeleteUserAsync(string userId)
    {
        var user = await _userManager.FindByIdAsync(userId);
        if (user == null)
        {
            return new DeleteUserResponse
            {
                Success = false,
                Message = "User not found"
            };
        }

        var result = await _userManager.DeleteAsync(user);
        if (!result.Succeeded)
        {
            var errors = string.Join(", ", result.Errors.Select(e => e.Description));
            _logger.LogWarning($"User deletion failed for {user.Email}: {errors}");
            return new DeleteUserResponse
            {
                Success = false,
                Message = "User deletion failed: " + errors
            };
        }

        _logger.LogInformation($"User deleted: {user.Email}");

        return new DeleteUserResponse
        {
            Success = true,
            Message = "User deleted successfully"
        };
    }

    private UserDto MapToUserDto(NtripUser user)
    {
        return new UserDto
        {
            Id = user.Id,
            Email = user.Email,
            FullName = user.FullName,
            EmailConfirmed = user.EmailConfirmed,
            CreatedAt = user.CreatedAt,
            MaxConnections = user.MaxConnections,
            IsActive = user.IsActive,
            Groups = user.Groups?.Select(g => g.Name).ToList() ?? new()
        };
    }
}
