using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.Entities;
using Microsoft.EntityFrameworkCore;

namespace AgOpenNtripCaster.Server.Services.Email;

public interface IEmailSmtpSettingsService
{
    Task<EmailSmtpSettings> GetSettingsAsync();
    Task<EmailSmtpSettings> UpdateSettingsAsync(EmailSmtpSettings settings);
}

public class EmailSmtpSettingsService : IEmailSmtpSettingsService
{
    private readonly ApplicationDbContext _context;
    private readonly ILogger<EmailSmtpSettingsService> _logger;
    private readonly IConfiguration _configuration;

    public EmailSmtpSettingsService(
        ApplicationDbContext context,
        ILogger<EmailSmtpSettingsService> logger,
        IConfiguration configuration)
    {
        _context = context;
        _logger = logger;
        _configuration = configuration;
    }

    /// <summary>
    /// Get SMTP settings from database, or load from appsettings if not in DB
    /// </summary>
    public async Task<EmailSmtpSettings> GetSettingsAsync()
    {
        try
        {
            var settings = await _context.EmailSmtpSettings.FirstOrDefaultAsync();

            if (settings == null)
            {
                _logger.LogInformation("No SMTP settings in database, loading from appsettings.json");

                // Load from appsettings.json for backward compatibility
                var smtpSection = _configuration.GetSection("Email:Smtp");
                settings = new EmailSmtpSettings
                {
                    Host = smtpSection["Host"] ?? string.Empty,
                    Port = int.TryParse(smtpSection["Port"], out var port) ? port : 587,
                    Username = smtpSection["Username"] ?? string.Empty,
                    Password = smtpSection["Password"] ?? string.Empty,
                    FromEmail = smtpSection["FromEmail"] ?? string.Empty,
                    FromName = smtpSection["FromName"] ?? "NtripCaster",
                    EnableSsl = true,
                    EnableTls = true,
                    IsConfigured = !string.IsNullOrEmpty(smtpSection["Host"]),
                    UpdatedAt = DateTime.UtcNow
                };

                // Save to database for future use
                try
                {
                    _context.EmailSmtpSettings.Add(settings);
                    await _context.SaveChangesAsync();
                    _logger.LogInformation("SMTP settings migrated from appsettings.json to database");
                }
                catch (Exception ex)
                {
                    _logger.LogWarning(ex, "Could not save SMTP settings to database, using in-memory version");
                }
            }

            return settings;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error loading SMTP settings");
            throw;
        }
    }

    /// <summary>
    /// Update SMTP settings in database
    /// </summary>
    public async Task<EmailSmtpSettings> UpdateSettingsAsync(EmailSmtpSettings settings)
    {
        try
        {
            var existingSettings = await _context.EmailSmtpSettings.FirstOrDefaultAsync();

            if (existingSettings == null)
            {
                // Create new settings record
                settings.UpdatedAt = DateTime.UtcNow;
                _context.EmailSmtpSettings.Add(settings);
                await _context.SaveChangesAsync();
                _logger.LogInformation("SMTP settings created successfully with ID {SettingsId}", settings.Id);
            }
            else
            {
                // Update existing settings
                existingSettings.Host = settings.Host;
                existingSettings.Port = settings.Port;
                existingSettings.Username = settings.Username;
                existingSettings.Password = settings.Password;
                existingSettings.FromEmail = settings.FromEmail;
                existingSettings.FromName = settings.FromName;
                existingSettings.EnableSsl = settings.EnableSsl;
                existingSettings.EnableTls = settings.EnableTls;
                existingSettings.IsConfigured = settings.IsConfigured;
                existingSettings.UpdatedAt = DateTime.UtcNow;

                _context.EmailSmtpSettings.Update(existingSettings);
                await _context.SaveChangesAsync();
                _logger.LogInformation("SMTP settings updated successfully");

                // Return the updated record from database
                return existingSettings;
            }

            // Return the newly created record
            return settings;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error updating SMTP settings");
            throw;
        }
    }
}
