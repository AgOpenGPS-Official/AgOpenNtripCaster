using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.Entities;
using Microsoft.EntityFrameworkCore;

namespace AgOpenNtripCaster.Server.Services.Email;

public interface IEmailTriggerSettingsService
{
    Task<EmailTriggerSettings> GetSettingsAsync();
    Task<EmailTriggerSettings> UpdateSettingsAsync(EmailTriggerSettings settings);
}

public class EmailTriggerSettingsService : IEmailTriggerSettingsService
{
    private readonly ApplicationDbContext _context;
    private readonly ILogger<EmailTriggerSettingsService> _logger;

    public EmailTriggerSettingsService(ApplicationDbContext context, ILogger<EmailTriggerSettingsService> logger)
    {
        _context = context;
        _logger = logger;
    }

    public async Task<EmailTriggerSettings> GetSettingsAsync()
    {
        try
        {
            var settings = await _context.EmailTriggerSettings.FirstOrDefaultAsync();

            if (settings == null)
            {
                // Create default settings if none exist
                settings = new EmailTriggerSettings
                {
                    SendVerificationEmail = true,
                    SendWelcomeEmail = true,
                    SendSourceOfflineEmail = true,
                    SendSourceOnlineEmail = true,
                    AdminEmailForSourceNotifications = "admin@ntripcaster.local",
                    UpdatedAt = DateTime.UtcNow
                };

                _context.EmailTriggerSettings.Add(settings);
                await _context.SaveChangesAsync();
                _logger.LogInformation("Created default EmailTriggerSettings");
            }

            return settings;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error getting email trigger settings");
            throw;
        }
    }

    public async Task<EmailTriggerSettings> UpdateSettingsAsync(EmailTriggerSettings settings)
    {
        try
        {
            var existing = await _context.EmailTriggerSettings.FirstOrDefaultAsync();

            if (existing == null)
            {
                _context.EmailTriggerSettings.Add(settings);
                _logger.LogInformation("Created new EmailTriggerSettings");
            }
            else
            {
                existing.SendVerificationEmail = settings.SendVerificationEmail;
                existing.SendWelcomeEmail = settings.SendWelcomeEmail;
                existing.SendSourceOfflineEmail = settings.SendSourceOfflineEmail;
                existing.SendSourceOnlineEmail = settings.SendSourceOnlineEmail;
                existing.AdminEmailForSourceNotifications = settings.AdminEmailForSourceNotifications;
                existing.UpdatedAt = DateTime.UtcNow;

                _context.EmailTriggerSettings.Update(existing);
                _logger.LogInformation("Updated EmailTriggerSettings");
            }

            await _context.SaveChangesAsync();
            return settings;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error updating email trigger settings");
            throw;
        }
    }
}
