using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using NtripCaster.Server.Models.DTOs;
using NtripCaster.Server.Services.NTRIP;

namespace AgOpenNtripCaster.Server.Controllers;

/// <summary>
/// Mount point management endpoints: CRUD operations for NTRIP mount points
/// </summary>
[ApiController]
[Route("api/[controller]")]
[Authorize(Roles = "Admin")]
public class MountPointsController : ControllerBase
{
    private readonly IMountPointService _mountPointService;
    private readonly ILogger<MountPointsController> _logger;

    public MountPointsController(IMountPointService mountPointService, ILogger<MountPointsController> logger)
    {
        _mountPointService = mountPointService;
        _logger = logger;
    }

    /// <summary>
    /// Get paginated list of all mount points (sourcetable)
    /// </summary>
    /// <param name="page">Page number (default: 1)</param>
    /// <param name="pageSize">Items per page (default: 10)</param>
    /// <returns>MountPointListResponse with paginated mount points</returns>
    [HttpGet]
    [AllowAnonymous]
    [ProducesResponseType(typeof(MountPointListResponse), 200)]
    public async Task<ActionResult<MountPointListResponse>> GetMountPoints([FromQuery] int page = 1, [FromQuery] int pageSize = 10)
    {
        if (page < 1 || pageSize < 1)
        {
            return BadRequest("Page and pageSize must be greater than 0");
        }

        var response = await _mountPointService.GetMountPointsAsync(page, pageSize);
        return Ok(response);
    }

    /// <summary>
    /// Get current user's own mount points (sources they created)
    /// </summary>
    /// <param name="page">Page number (default: 1)</param>
    /// <param name="pageSize">Items per page (default: 10)</param>
    /// <returns>MountPointListResponse with user's mount points</returns>
    [HttpGet("my-mountpoints")]
    [Authorize]
    [ProducesResponseType(typeof(MountPointListResponse), 200)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    public async Task<ActionResult<MountPointListResponse>> GetMyMountPoints([FromQuery] int page = 1, [FromQuery] int pageSize = 10)
    {
        if (page < 1 || pageSize < 1)
        {
            return BadRequest("Page and pageSize must be greater than 0");
        }

        var userId = User.FindFirst(System.Security.Claims.ClaimTypes.NameIdentifier)?.Value;
        if (string.IsNullOrEmpty(userId))
        {
            return Unauthorized("User ID not found in token");
        }

        var response = await _mountPointService.GetUserMountPointsAsync(userId, page, pageSize);
        return Ok(response);
    }

    /// <summary>
    /// Get mount point by ID
    /// </summary>
    /// <param name="mountPointId">Mount point ID</param>
    /// <returns>MountPointDto</returns>
    [HttpGet("{mountPointId:int}")]
    [AllowAnonymous]
    [ProducesResponseType(typeof(MountPointDto), 200)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    public async Task<ActionResult<MountPointDto>> GetMountPointById([FromRoute] int mountPointId)
    {
        var mountPoint = await _mountPointService.GetMountPointByIdAsync(mountPointId);
        if (mountPoint == null)
        {
            return NotFound("Mount point not found");
        }

        return Ok(mountPoint);
    }

    /// <summary>
    /// Create new mount point (any authenticated user)
    /// </summary>
    /// <param name="request">Mount point creation details</param>
    /// <returns>CreateMountPointResponse with created mount point</returns>
    [HttpPost]
    [Authorize]
    [ProducesResponseType(typeof(CreateMountPointResponse), 201)]
    [ProducesResponseType(typeof(CreateMountPointResponse), 400)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    public async Task<ActionResult<CreateMountPointResponse>> CreateMountPoint([FromBody] CreateMountPointRequest request)
    {
        if (!ModelState.IsValid)
        {
            return BadRequest(ModelState);
        }

        var userId = User.FindFirst(System.Security.Claims.ClaimTypes.NameIdentifier)?.Value;
        if (string.IsNullOrEmpty(userId))
        {
            return Unauthorized("User ID not found in token");
        }

        var response = await _mountPointService.CreateMountPointAsync(request, userId);

        if (!response.Success)
        {
            return BadRequest(response);
        }

        return CreatedAtAction(nameof(GetMountPointById), new { mountPointId = response.MountPoint?.Id }, response);
    }

    /// <summary>
    /// Update mount point (admin only)
    /// </summary>
    /// <param name="mountPointId">Mount point ID to update</param>
    /// <param name="request">Update details</param>
    /// <returns>UpdateMountPointResponse with updated mount point</returns>
    [HttpPut("{mountPointId:int}")]
    [Authorize(Roles = "Admin")]
    [ProducesResponseType(typeof(UpdateMountPointResponse), 200)]
    [ProducesResponseType(typeof(UpdateMountPointResponse), 400)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    [ProducesResponseType(StatusCodes.Status403Forbidden)]
    public async Task<ActionResult<UpdateMountPointResponse>> UpdateMountPoint(
        [FromRoute] int mountPointId,
        [FromBody] UpdateMountPointRequest request)
    {
        if (!ModelState.IsValid)
        {
            return BadRequest(ModelState);
        }

        var response = await _mountPointService.UpdateMountPointAsync(mountPointId, request);

        if (!response.Success)
        {
            if (response.Message.Contains("not found"))
            {
                return NotFound(response);
            }
            return BadRequest(response);
        }

        return Ok(response);
    }

    /// <summary>
    /// Delete mount point (admin only)
    /// </summary>
    /// <param name="mountPointId">Mount point ID to delete</param>
    /// <returns>DeleteMountPointResponse</returns>
    [HttpDelete("{mountPointId:int}")]
    [Authorize(Roles = "Admin")]
    [ProducesResponseType(typeof(DeleteMountPointResponse), 200)]
    [ProducesResponseType(typeof(DeleteMountPointResponse), 404)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    [ProducesResponseType(StatusCodes.Status403Forbidden)]
    public async Task<ActionResult<DeleteMountPointResponse>> DeleteMountPoint([FromRoute] int mountPointId)
    {
        var response = await _mountPointService.DeleteMountPointAsync(mountPointId);

        if (!response.Success)
        {
            return NotFound(response);
        }

        return Ok(response);
    }

    /// <summary>
    /// Allow group to access mount point (admin only)
    /// </summary>
    /// <param name="mountPointId">Mount point ID</param>
    /// <param name="request">Group ID to allow</param>
    /// <returns>MountPointPermissionResponse</returns>
    [HttpPost("{mountPointId:int}/allow-group")]
    [Authorize(Roles = "Admin")]
    [ProducesResponseType(typeof(MountPointPermissionResponse), 200)]
    [ProducesResponseType(typeof(MountPointPermissionResponse), 400)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    [ProducesResponseType(StatusCodes.Status403Forbidden)]
    public async Task<ActionResult<MountPointPermissionResponse>> AllowGroup(
        [FromRoute] int mountPointId,
        [FromBody] AllowGroupRequest request)
    {
        if (!ModelState.IsValid)
        {
            return BadRequest(ModelState);
        }

        var response = await _mountPointService.AllowGroupAsync(mountPointId, request.GroupId);

        if (!response.Success)
        {
            if (response.Message.Contains("not found"))
            {
                return NotFound(response);
            }
            return BadRequest(response);
        }

        return Ok(response);
    }

    /// <summary>
    /// Deny group access to mount point (admin only)
    /// </summary>
    /// <param name="mountPointId">Mount point ID</param>
    /// <param name="request">Group ID to deny</param>
    /// <returns>MountPointPermissionResponse</returns>
    [HttpPost("{mountPointId:int}/deny-group")]
    [Authorize(Roles = "Admin")]
    [ProducesResponseType(typeof(MountPointPermissionResponse), 200)]
    [ProducesResponseType(typeof(MountPointPermissionResponse), 400)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    [ProducesResponseType(StatusCodes.Status403Forbidden)]
    public async Task<ActionResult<MountPointPermissionResponse>> DenyGroup(
        [FromRoute] int mountPointId,
        [FromBody] DenyGroupRequest request)
    {
        if (!ModelState.IsValid)
        {
            return BadRequest(ModelState);
        }

        var response = await _mountPointService.DenyGroupAsync(mountPointId, request.GroupId);

        if (!response.Success)
        {
            if (response.Message.Contains("not found"))
            {
                return NotFound(response);
            }
            return BadRequest(response);
        }

        return Ok(response);
    }
}
