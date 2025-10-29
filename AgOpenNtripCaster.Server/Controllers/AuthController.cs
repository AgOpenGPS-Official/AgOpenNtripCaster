using Microsoft.AspNetCore.Mvc;
using AgOpenNtripCaster.Server.Models.DTOs;
using AgOpenNtripCaster.Server.Services.Auth;

namespace AgOpenNtripCaster.Server.Controllers;

/// <summary>
/// Authentication endpoints: register, login, email verification, token refresh
/// </summary>
[ApiController]
[Route("api/[controller]")]
public class AuthController : ControllerBase
{
    private readonly IAuthService _authService;
    private readonly ILogger<AuthController> _logger;

    public AuthController(IAuthService authService, ILogger<AuthController> logger)
    {
        _authService = authService;
        _logger = logger;
    }

    /// <summary>
    /// Register a new user account with email verification
    /// </summary>
    /// <param name="request">Registration details (email, password, fullName)</param>
    /// <returns>RegisterResponse with success status and message</returns>
    [HttpPost("register")]
    [ProducesResponseType(typeof(RegisterResponse), 200)]
    [ProducesResponseType(typeof(RegisterResponse), 400)]
    public async Task<ActionResult<RegisterResponse>> Register([FromBody] RegisterRequest request)
    {
        if (!ModelState.IsValid)
        {
            return BadRequest(ModelState);
        }

        var baseUrl = $"{Request.Scheme}://{Request.Host}";
        var response = await _authService.RegisterAsync(request, baseUrl);

        if (!response.Success)
        {
            return BadRequest(response);
        }

        return Ok(response);
    }

    /// <summary>
    /// Verify user email with token from verification link
    /// </summary>
    /// <param name="request">Email and verification token</param>
    /// <returns>VerifyEmailResponse with success status</returns>
    [HttpPost("verify-email")]
    [ProducesResponseType(typeof(VerifyEmailResponse), 200)]
    [ProducesResponseType(typeof(VerifyEmailResponse), 400)]
    public async Task<ActionResult<VerifyEmailResponse>> VerifyEmail([FromBody] VerifyEmailRequest request)
    {
        if (!ModelState.IsValid)
        {
            return BadRequest(ModelState);
        }

        var response = await _authService.VerifyEmailAsync(request);

        if (!response.Success)
        {
            return BadRequest(response);
        }

        return Ok(response);
    }

    /// <summary>
    /// GET endpoint for email verification link (redirects to frontend)
    /// </summary>
    /// <param name="email">User email</param>
    /// <param name="token">Verification token</param>
    /// <returns>Redirect to frontend verification page or error</returns>
    [HttpGet("verify-email")]
    [ProducesResponseType(StatusCodes.Status302Found)]
    [ProducesResponseType(StatusCodes.Status400BadRequest)]
    public async Task<IActionResult> VerifyEmailGet([FromQuery] string email, [FromQuery] string token)
    {
        if (string.IsNullOrEmpty(email) || string.IsNullOrEmpty(token))
        {
            return BadRequest("Email and token are required");
        }

        var response = await _authService.VerifyEmailAsync(new VerifyEmailRequest { Email = email, Token = token });

        // Redirect to frontend with result
        var frontendUrl = $"{Request.Scheme}://{Request.Host.Host}:{Request.Host.Port ?? (Request.Scheme == "https" ? 443 : 80)}";
        if (response.Success)
        {
            return Redirect($"{frontendUrl}/auth/verify-success");
        }
        else
        {
            return Redirect($"{frontendUrl}/auth/verify-error?message={Uri.EscapeDataString(response.Message)}");
        }
    }

    /// <summary>
    /// Login with email and password
    /// </summary>
    /// <param name="request">Email and password</param>
    /// <returns>LoginResponse with JWT access token and refresh token</returns>
    [HttpPost("login")]
    [ProducesResponseType(typeof(LoginResponse), 200)]
    [ProducesResponseType(typeof(LoginResponse), 401)]
    public async Task<ActionResult<LoginResponse>> Login([FromBody] LoginRequest request)
    {
        if (!ModelState.IsValid)
        {
            return BadRequest(ModelState);
        }

        var response = await _authService.LoginAsync(request);

        if (!response.Success)
        {
            return Unauthorized(response);
        }

        return Ok(response);
    }

    /// <summary>
    /// Refresh JWT access token using refresh token
    /// </summary>
    /// <param name="request">Refresh token</param>
    /// <returns>RefreshTokenResponse with new access and refresh tokens</returns>
    [HttpPost("refresh")]
    [ProducesResponseType(typeof(RefreshTokenResponse), 200)]
    [ProducesResponseType(typeof(RefreshTokenResponse), 401)]
    public async Task<ActionResult<RefreshTokenResponse>> RefreshToken([FromBody] RefreshTokenRequest request)
    {
        if (!ModelState.IsValid)
        {
            return BadRequest(ModelState);
        }

        var response = await _authService.RefreshTokenAsync(request);

        if (!response.Success)
        {
            return Unauthorized(response);
        }

        return Ok(response);
    }
}
