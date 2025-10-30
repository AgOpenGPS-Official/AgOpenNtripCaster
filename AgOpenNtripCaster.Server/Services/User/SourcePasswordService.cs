using System.Security.Cryptography;
using System.Text;
using AgOpenNtripCaster.Server.Data;
using Microsoft.EntityFrameworkCore;

namespace AgOpenNtripCaster.Server.Services.User;

/// <summary>
/// Service for managing source passwords used by BaseStations
/// Source passwords are hashed using bcrypt for security
/// </summary>
public interface ISourcePasswordService
{
    /// <summary>
    /// Generate a new random source password (32 characters)
    /// </summary>
    string GenerateSourcePassword();

    /// <summary>
    /// Generate a new source password for a user and save to database
    /// </summary>
    Task<string> GenerateAndSaveSourcePasswordAsync(string userId);

    /// <summary>
    /// Verify if the provided password matches the user's source password
    /// </summary>
    Task<bool> VerifySourcePasswordAsync(string userId, string providedPassword);

    /// <summary>
    /// Get the hashed source password for a user (for display/verification)
    /// Returns null if not set
    /// </summary>
    Task<string?> GetSourcePasswordHashAsync(string userId);
}

public class SourcePasswordService : ISourcePasswordService
{
    private readonly ApplicationDbContext _context;
    private readonly ILogger<SourcePasswordService> _logger;
    private const int PasswordLength = 32;

    public SourcePasswordService(
        ApplicationDbContext context,
        ILogger<SourcePasswordService> logger)
    {
        _context = context;
        _logger = logger;
    }

    public string GenerateSourcePassword()
    {
        // Generate simple random alphanumeric string (8-12 characters)
        const string chars = "ABCDEFGHJKLMNPQRSTUVWXYZabcdefghjkmnpqrstuvwxyz23456789"; // Avoid confusing chars (0,O,1,l,I)
        const int passwordLength = 10; // Random length between 8-12 for simplicity, we use 10 as default

        using var rng = new RNGCryptoServiceProvider();
        var buffer = new byte[passwordLength];
        rng.GetBytes(buffer);

        var sb = new StringBuilder(passwordLength);
        foreach (byte b in buffer)
        {
            sb.Append(chars[b % chars.Length]);
        }

        return sb.ToString();
    }

    public async Task<string> GenerateAndSaveSourcePasswordAsync(string userId)
    {
        try
        {
            var user = await _context.Users.FindAsync(userId);
            if (user == null)
            {
                throw new InvalidOperationException($"User {userId} not found");
            }

            // Generate new password
            var plainPassword = GenerateSourcePassword();

            // Hash the password using bcrypt
            var hashedPassword = BCrypt.Net.BCrypt.HashPassword(plainPassword);

            // Save to database
            user.SourcePassword = hashedPassword;
            _context.Users.Update(user);
            await _context.SaveChangesAsync();

            _logger.LogInformation("Generated new source password for user {UserId}", userId);

            // Return plain password (only shown once to user)
            return plainPassword;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error generating source password for user {UserId}", userId);
            throw;
        }
    }

    public async Task<bool> VerifySourcePasswordAsync(string userId, string providedPassword)
    {
        try
        {
            var user = await _context.Users.FindAsync(userId);
            if (user == null || string.IsNullOrEmpty(user.SourcePassword))
            {
                return false;
            }

            // Verify using bcrypt
            return BCrypt.Net.BCrypt.Verify(providedPassword, user.SourcePassword);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error verifying source password for user {UserId}", userId);
            return false;
        }
    }

    public async Task<string?> GetSourcePasswordHashAsync(string userId)
    {
        try
        {
            var user = await _context.Users
                .AsNoTracking()
                .FirstOrDefaultAsync(u => u.Id == userId);

            return user?.SourcePassword;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error retrieving source password for user {UserId}", userId);
            return null;
        }
    }
}
