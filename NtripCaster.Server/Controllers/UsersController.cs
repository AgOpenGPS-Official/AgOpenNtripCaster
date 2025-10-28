using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using NtripCaster.Server.Models.DTOs;
using NtripCaster.Server.Services.Auth;
using System.Security.Claims;

namespace NtripCaster.Server.Controllers;

/// <summary>
/// User management endpoints: CRUD operations for users
/// </summary>
[ApiController]
[Route("api/[controller]")]
[Authorize]
public class UsersController : ControllerBase
{
    private readonly IUserService _userService;
    private readonly ILogger<UsersController> _logger;

    public UsersController(IUserService userService, ILogger<UsersController> logger)
    {
        _userService = userService;
        _logger = logger;
    }

    /// <summary>
    /// Get current logged-in user profile
    /// </summary>
    /// <returns>UserDto with current user data</returns>
    [HttpGet("me")]
    [ProducesResponseType(typeof(UserDto), 200)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    public async Task<ActionResult<UserDto>> GetCurrentUser()
    {
        var userId = User.FindFirst(ClaimTypes.NameIdentifier)?.Value;
        if (string.IsNullOrEmpty(userId))
        {
            return Unauthorized("User ID not found in token");
        }

        var user = await _userService.GetCurrentUserAsync(userId);
        if (user == null)
        {
            return NotFound("User not found");
        }

        return Ok(user);
    }

    /// <summary>
    /// Get paginated list of all users (admin only)
    /// </summary>
    /// <param name="page">Page number (default: 1)</param>
    /// <param name="pageSize">Items per page (default: 10)</param>
    /// <returns>UserListResponse with paginated users</returns>
    [HttpGet]
    [ProducesResponseType(typeof(UserListResponse), 200)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    public async Task<ActionResult<UserListResponse>> GetUsers([FromQuery] int page = 1, [FromQuery] int pageSize = 10)
    {
        if (page < 1 || pageSize < 1)
        {
            return BadRequest("Page and pageSize must be greater than 0");
        }

        var response = await _userService.GetUsersAsync(page, pageSize);
        return Ok(response);
    }

    /// <summary>
    /// Get user by ID
    /// </summary>
    /// <param name="userId">User ID</param>
    /// <returns>UserDto</returns>
    [HttpGet("{userId}")]
    [ProducesResponseType(typeof(UserDto), 200)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    public async Task<ActionResult<UserDto>> GetUserById([FromRoute] string userId)
    {
        var currentUserId = User.FindFirst(ClaimTypes.NameIdentifier)?.Value;

        // Users can only view their own profile or admin can view anyone
        var userRoles = User.FindAll(ClaimTypes.Role);
        var isAdmin = userRoles.Any(r => r.Value == "Admin");

        if (userId != currentUserId && !isAdmin)
        {
            return Forbid();
        }

        var user = await _userService.GetUserByIdAsync(userId);
        if (user == null)
        {
            return NotFound("User not found");
        }

        return Ok(user);
    }

    /// <summary>
    /// Create new user (admin only)
    /// </summary>
    /// <param name="request">User creation details</param>
    /// <returns>CreateUserResponse with created user</returns>
    [HttpPost]
    [Authorize(Roles = "Admin")]
    [ProducesResponseType(typeof(CreateUserResponse), 201)]
    [ProducesResponseType(typeof(CreateUserResponse), 400)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    [ProducesResponseType(StatusCodes.Status403Forbidden)]
    public async Task<ActionResult<CreateUserResponse>> CreateUser([FromBody] CreateUserRequest request)
    {
        if (!ModelState.IsValid)
        {
            return BadRequest(ModelState);
        }

        var response = await _userService.CreateUserAsync(request);

        if (!response.Success)
        {
            return BadRequest(response);
        }

        return CreatedAtAction(nameof(GetUserById), new { userId = response.User?.Id }, response);
    }

    /// <summary>
    /// Update user profile (self or admin)
    /// </summary>
    /// <param name="userId">User ID to update</param>
    /// <param name="request">Update details</param>
    /// <returns>UpdateUserResponse with updated user</returns>
    [HttpPut("{userId}")]
    [ProducesResponseType(typeof(UpdateUserResponse), 200)]
    [ProducesResponseType(typeof(UpdateUserResponse), 400)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    [ProducesResponseType(StatusCodes.Status403Forbidden)]
    public async Task<ActionResult<UpdateUserResponse>> UpdateUser(
        [FromRoute] string userId,
        [FromBody] UpdateUserRequest request)
    {
        if (!ModelState.IsValid)
        {
            return BadRequest(ModelState);
        }

        var currentUserId = User.FindFirst(ClaimTypes.NameIdentifier)?.Value;
        var userRoles = User.FindAll(ClaimTypes.Role);
        var isAdmin = userRoles.Any(r => r.Value == "Admin");

        var response = await _userService.UpdateUserAsync(userId, request, currentUserId, isAdmin);

        if (!response.Success)
        {
            if (response.Message.Contains("not found"))
            {
                return NotFound(response);
            }
            if (response.Message.Contains("only edit"))
            {
                return Forbid();
            }
            return BadRequest(response);
        }

        return Ok(response);
    }

    /// <summary>
    /// Change password for current user
    /// </summary>
    /// <param name="request">Current and new password</param>
    /// <returns>ChangePasswordResponse</returns>
    [HttpPost("change-password")]
    [ProducesResponseType(typeof(ChangePasswordResponse), 200)]
    [ProducesResponseType(typeof(ChangePasswordResponse), 400)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    public async Task<ActionResult<ChangePasswordResponse>> ChangePassword([FromBody] ChangePasswordRequest request)
    {
        if (!ModelState.IsValid)
        {
            return BadRequest(ModelState);
        }

        var userId = User.FindFirst(ClaimTypes.NameIdentifier)?.Value;
        if (string.IsNullOrEmpty(userId))
        {
            return Unauthorized("User ID not found in token");
        }

        var response = await _userService.ChangePasswordAsync(userId, request);

        if (!response.Success)
        {
            return BadRequest(response);
        }

        return Ok(response);
    }

    /// <summary>
    /// Delete user (admin only)
    /// </summary>
    /// <param name="userId">User ID to delete</param>
    /// <returns>DeleteUserResponse</returns>
    [HttpDelete("{userId}")]
    [Authorize(Roles = "Admin")]
    [ProducesResponseType(typeof(DeleteUserResponse), 200)]
    [ProducesResponseType(typeof(DeleteUserResponse), 404)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    [ProducesResponseType(StatusCodes.Status403Forbidden)]
    public async Task<ActionResult<DeleteUserResponse>> DeleteUser([FromRoute] string userId)
    {
        var response = await _userService.DeleteUserAsync(userId);

        if (!response.Success)
        {
            return NotFound(response);
        }

        return Ok(response);
    }
}
