using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using NtripCaster.Server.Models.DTOs;
using NtripCaster.Server.Services.Auth;

namespace NtripCaster.Server.Controllers;

/// <summary>
/// Group management endpoints: CRUD operations for user groups
/// </summary>
[ApiController]
[Route("api/[controller]")]
[Authorize(Roles = "Admin")]
public class GroupsController : ControllerBase
{
    private readonly IGroupService _groupService;
    private readonly ILogger<GroupsController> _logger;

    public GroupsController(IGroupService groupService, ILogger<GroupsController> logger)
    {
        _groupService = groupService;
        _logger = logger;
    }

    /// <summary>
    /// Get paginated list of all groups
    /// </summary>
    /// <param name="page">Page number (default: 1)</param>
    /// <param name="pageSize">Items per page (default: 10)</param>
    /// <returns>GroupListResponse with paginated groups</returns>
    [HttpGet]
    [ProducesResponseType(typeof(GroupListResponse), 200)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    [ProducesResponseType(StatusCodes.Status403Forbidden)]
    public async Task<ActionResult<GroupListResponse>> GetGroups([FromQuery] int page = 1, [FromQuery] int pageSize = 10)
    {
        if (page < 1 || pageSize < 1)
        {
            return BadRequest("Page and pageSize must be greater than 0");
        }

        var response = await _groupService.GetGroupsAsync(page, pageSize);
        return Ok(response);
    }

    /// <summary>
    /// Get group by ID
    /// </summary>
    /// <param name="groupId">Group ID</param>
    /// <returns>GroupDto</returns>
    [HttpGet("{groupId:int}")]
    [ProducesResponseType(typeof(GroupDto), 200)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    [ProducesResponseType(StatusCodes.Status403Forbidden)]
    public async Task<ActionResult<GroupDto>> GetGroupById([FromRoute] int groupId)
    {
        var group = await _groupService.GetGroupByIdAsync(groupId);
        if (group == null)
        {
            return NotFound("Group not found");
        }

        return Ok(group);
    }

    /// <summary>
    /// Create new group
    /// </summary>
    /// <param name="request">Group creation details</param>
    /// <returns>CreateGroupResponse with created group</returns>
    [HttpPost]
    [ProducesResponseType(typeof(CreateGroupResponse), 201)]
    [ProducesResponseType(typeof(CreateGroupResponse), 400)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    [ProducesResponseType(StatusCodes.Status403Forbidden)]
    public async Task<ActionResult<CreateGroupResponse>> CreateGroup([FromBody] CreateGroupRequest request)
    {
        if (!ModelState.IsValid)
        {
            return BadRequest(ModelState);
        }

        var response = await _groupService.CreateGroupAsync(request);

        if (!response.Success)
        {
            return BadRequest(response);
        }

        return CreatedAtAction(nameof(GetGroupById), new { groupId = response.Group?.Id }, response);
    }

    /// <summary>
    /// Update group
    /// </summary>
    /// <param name="groupId">Group ID to update</param>
    /// <param name="request">Update details</param>
    /// <returns>UpdateGroupResponse with updated group</returns>
    [HttpPut("{groupId:int}")]
    [ProducesResponseType(typeof(UpdateGroupResponse), 200)]
    [ProducesResponseType(typeof(UpdateGroupResponse), 400)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    [ProducesResponseType(StatusCodes.Status403Forbidden)]
    public async Task<ActionResult<UpdateGroupResponse>> UpdateGroup(
        [FromRoute] int groupId,
        [FromBody] UpdateGroupRequest request)
    {
        if (!ModelState.IsValid)
        {
            return BadRequest(ModelState);
        }

        var response = await _groupService.UpdateGroupAsync(groupId, request);

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
    /// Delete group
    /// </summary>
    /// <param name="groupId">Group ID to delete</param>
    /// <returns>DeleteGroupResponse</returns>
    [HttpDelete("{groupId:int}")]
    [ProducesResponseType(typeof(DeleteGroupResponse), 200)]
    [ProducesResponseType(typeof(DeleteGroupResponse), 404)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    [ProducesResponseType(StatusCodes.Status403Forbidden)]
    public async Task<ActionResult<DeleteGroupResponse>> DeleteGroup([FromRoute] int groupId)
    {
        var response = await _groupService.DeleteGroupAsync(groupId);

        if (!response.Success)
        {
            return NotFound(response);
        }

        return Ok(response);
    }

    /// <summary>
    /// Add user to group
    /// </summary>
    /// <param name="groupId">Group ID</param>
    /// <param name="request">User ID to add</param>
    /// <returns>GroupMembershipResponse</returns>
    [HttpPost("{groupId:int}/add-user")]
    [ProducesResponseType(typeof(GroupMembershipResponse), 200)]
    [ProducesResponseType(typeof(GroupMembershipResponse), 400)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    [ProducesResponseType(StatusCodes.Status403Forbidden)]
    public async Task<ActionResult<GroupMembershipResponse>> AddUserToGroup(
        [FromRoute] int groupId,
        [FromBody] AddUserToGroupRequest request)
    {
        if (!ModelState.IsValid)
        {
            return BadRequest(ModelState);
        }

        var response = await _groupService.AddUserToGroupAsync(groupId, request.UserId);

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
    /// Remove user from group
    /// </summary>
    /// <param name="groupId">Group ID</param>
    /// <param name="request">User ID to remove</param>
    /// <returns>GroupMembershipResponse</returns>
    [HttpPost("{groupId:int}/remove-user")]
    [ProducesResponseType(typeof(GroupMembershipResponse), 200)]
    [ProducesResponseType(typeof(GroupMembershipResponse), 400)]
    [ProducesResponseType(StatusCodes.Status404NotFound)]
    [ProducesResponseType(StatusCodes.Status401Unauthorized)]
    [ProducesResponseType(StatusCodes.Status403Forbidden)]
    public async Task<ActionResult<GroupMembershipResponse>> RemoveUserFromGroup(
        [FromRoute] int groupId,
        [FromBody] RemoveUserFromGroupRequest request)
    {
        if (!ModelState.IsValid)
        {
            return BadRequest(ModelState);
        }

        var response = await _groupService.RemoveUserFromGroupAsync(groupId, request.UserId);

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
