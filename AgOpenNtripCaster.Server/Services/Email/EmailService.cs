using System.Net;
using System.Net.Mail;

namespace AgOpenNtripCaster.Server.Services.Email;

/// <summary>
/// Service for sending emails
/// </summary>
public interface IEmailService
{
    Task<bool> SendVerificationEmailAsync(string email, string fullName, string verificationLink);
    Task<bool> SendWelcomeEmailAsync(string email, string fullName);
    Task<bool> SendSourceOfflineEmailAsync(string email, string fullName, string sourceName, string mountPointName);
    Task<bool> SendSourceOnlineEmailAsync(string email, string fullName, string sourceName, string mountPointName);
    Task<bool> SendTestEmailAsync(string email);
}

public class EmailService : IEmailService
{
    private readonly IConfiguration _configuration;
    private readonly ILogger<EmailService> _logger;

    public EmailService(IConfiguration configuration, ILogger<EmailService> logger)
    {
        _configuration = configuration;
        _logger = logger;
    }

    public async Task<bool> SendVerificationEmailAsync(string email, string fullName, string verificationLink)
    {
        try
        {
            var smtpSettings = _configuration.GetSection("Email:Smtp");
            var smtpHost = smtpSettings["Host"];
            var smtpPort = int.Parse(smtpSettings["Port"] ?? "587");
            var smtpUser = smtpSettings["Username"];
            var smtpPassword = smtpSettings["Password"];
            var fromEmail = smtpSettings["FromEmail"];
            var fromName = smtpSettings["FromName"];

            // If email verification is disabled, just return true
            if (!_configuration.GetValue<bool>("Email:VerificationRequired"))
            {
                _logger.LogInformation($"Email verification disabled - would send to {email}");
                return true;
            }

            using (var smtpClient = new SmtpClient(smtpHost, smtpPort))
            {
                smtpClient.EnableSsl = true;
                smtpClient.Credentials = new NetworkCredential(smtpUser, smtpPassword);

                var mailMessage = new MailMessage
                {
                    From = new MailAddress(fromEmail, fromName),
                    Subject = "Verify Your NtripCaster Email Address",
                    IsBodyHtml = true
                };

                mailMessage.To.Add(email);

                var htmlBody = GenerateVerificationEmailHtml(fullName, verificationLink);
                mailMessage.Body = htmlBody;

                await smtpClient.SendMailAsync(mailMessage);
                _logger.LogInformation($"Verification email sent to {email}");
                return true;
            }
        }
        catch (Exception ex)
        {
            _logger.LogError($"Failed to send verification email to {email}: {ex.Message}");
            return false;
        }
    }

    public async Task<bool> SendWelcomeEmailAsync(string email, string fullName)
    {
        try
        {
            var smtpSettings = _configuration.GetSection("Email:Smtp");
            var smtpHost = smtpSettings["Host"];
            var smtpPort = int.Parse(smtpSettings["Port"] ?? "587");
            var smtpUser = smtpSettings["Username"];
            var smtpPassword = smtpSettings["Password"];
            var fromEmail = smtpSettings["FromEmail"];
            var fromName = smtpSettings["FromName"];

            using (var smtpClient = new SmtpClient(smtpHost, smtpPort))
            {
                smtpClient.EnableSsl = true;
                smtpClient.Credentials = new NetworkCredential(smtpUser, smtpPassword);

                var mailMessage = new MailMessage
                {
                    From = new MailAddress(fromEmail, fromName),
                    Subject = "Welcome to NtripCaster!",
                    IsBodyHtml = true
                };

                mailMessage.To.Add(email);

                var htmlBody = GenerateWelcomeEmailHtml(fullName);
                mailMessage.Body = htmlBody;

                await smtpClient.SendMailAsync(mailMessage);
                _logger.LogInformation($"Welcome email sent to {email}");
                return true;
            }
        }
        catch (Exception ex)
        {
            _logger.LogError($"Failed to send welcome email to {email}: {ex.Message}");
            return false;
        }
    }

    public async Task<bool> SendSourceOfflineEmailAsync(string email, string fullName, string sourceName, string mountPointName)
    {
        try
        {
            var smtpSettings = _configuration.GetSection("Email:Smtp");
            var smtpHost = smtpSettings["Host"];
            var smtpPort = int.Parse(smtpSettings["Port"] ?? "587");
            var smtpUser = smtpSettings["Username"];
            var smtpPassword = smtpSettings["Password"];
            var fromEmail = smtpSettings["FromEmail"];
            var fromName = smtpSettings["FromName"];

            using (var smtpClient = new SmtpClient(smtpHost, smtpPort))
            {
                smtpClient.EnableSsl = true;
                smtpClient.Credentials = new NetworkCredential(smtpUser, smtpPassword);

                var mailMessage = new MailMessage
                {
                    From = new MailAddress(fromEmail, fromName),
                    Subject = $"⚠️ GNSS Source Offline: {sourceName}",
                    IsBodyHtml = true
                };

                mailMessage.To.Add(email);

                var htmlBody = GenerateSourceOfflineEmailHtml(fullName, sourceName, mountPointName);
                mailMessage.Body = htmlBody;

                await smtpClient.SendMailAsync(mailMessage);
                _logger.LogInformation($"Source offline notification sent to {email} for source {sourceName}");
                return true;
            }
        }
        catch (Exception ex)
        {
            _logger.LogError($"Failed to send source offline email to {email}: {ex.Message}");
            return false;
        }
    }

    public async Task<bool> SendSourceOnlineEmailAsync(string email, string fullName, string sourceName, string mountPointName)
    {
        try
        {
            var smtpSettings = _configuration.GetSection("Email:Smtp");
            var smtpHost = smtpSettings["Host"];
            var smtpPort = int.Parse(smtpSettings["Port"] ?? "587");
            var smtpUser = smtpSettings["Username"];
            var smtpPassword = smtpSettings["Password"];
            var fromEmail = smtpSettings["FromEmail"];
            var fromName = smtpSettings["FromName"];

            using (var smtpClient = new SmtpClient(smtpHost, smtpPort))
            {
                smtpClient.EnableSsl = true;
                smtpClient.Credentials = new NetworkCredential(smtpUser, smtpPassword);

                var mailMessage = new MailMessage
                {
                    From = new MailAddress(fromEmail, fromName),
                    Subject = $"✅ GNSS Source Online: {sourceName}",
                    IsBodyHtml = true
                };

                mailMessage.To.Add(email);

                var htmlBody = GenerateSourceOnlineEmailHtml(fullName, sourceName, mountPointName);
                mailMessage.Body = htmlBody;

                await smtpClient.SendMailAsync(mailMessage);
                _logger.LogInformation($"Source online notification sent to {email} for source {sourceName}");
                return true;
            }
        }
        catch (Exception ex)
        {
            _logger.LogError($"Failed to send source online email to {email}: {ex.Message}");
            return false;
        }
    }

    public async Task<bool> SendTestEmailAsync(string email)
    {
        try
        {
            var smtpSettings = _configuration.GetSection("Email:Smtp");
            var smtpHost = smtpSettings["Host"];
            var smtpPort = int.Parse(smtpSettings["Port"] ?? "587");
            var smtpUser = smtpSettings["Username"];
            var smtpPassword = smtpSettings["Password"];
            var fromEmail = smtpSettings["FromEmail"];
            var fromName = smtpSettings["FromName"];

            using (var smtpClient = new SmtpClient(smtpHost, smtpPort))
            {
                smtpClient.EnableSsl = true;
                smtpClient.Credentials = new NetworkCredential(smtpUser, smtpPassword);

                var mailMessage = new MailMessage
                {
                    From = new MailAddress(fromEmail, fromName),
                    Subject = "🧪 NtripCaster Test Email",
                    IsBodyHtml = true
                };

                mailMessage.To.Add(email);

                var htmlBody = GenerateTestEmailHtml();
                mailMessage.Body = htmlBody;

                await smtpClient.SendMailAsync(mailMessage);
                _logger.LogInformation($"Test email sent to {email}");
                return true;
            }
        }
        catch (Exception ex)
        {
            _logger.LogError($"Failed to send test email to {email}: {ex.Message}");
            return false;
        }
    }

    private string GenerateVerificationEmailHtml(string fullName, string verificationLink)
    {
        return $@"
<!DOCTYPE html>
<html>
<head>
    <style>
        body {{ font-family: Arial, sans-serif; }}
        .container {{ max-width: 600px; margin: 0 auto; padding: 20px; }}
        .header {{ background-color: #007bff; color: white; padding: 20px; border-radius: 5px; }}
        .content {{ padding: 20px; }}
        .button {{
            display: inline-block;
            background-color: #007bff;
            color: white;
            padding: 12px 30px;
            text-decoration: none;
            border-radius: 5px;
            margin: 20px 0;
        }}
        .footer {{ color: #666; font-size: 12px; margin-top: 20px; }}
    </style>
</head>
<body>
    <div class='container'>
        <div class='header'>
            <h1>NtripCaster - Email Verification</h1>
        </div>
        <div class='content'>
            <p>Hi {fullName},</p>
            <p>Thank you for signing up for NtripCaster! Please verify your email address by clicking the button below:</p>
            <a href='{verificationLink}' class='button'>Verify Email Address</a>
            <p>Or copy this link and paste it in your browser:</p>
            <p><code>{verificationLink}</code></p>
            <p>This link expires in 24 hours.</p>
            <p>If you did not create this account, please ignore this email.</p>
        </div>
        <div class='footer'>
            <p>&copy; 2025 NtripCaster. All rights reserved.</p>
        </div>
    </div>
</body>
</html>";
    }

    private string GenerateWelcomeEmailHtml(string fullName)
    {
        return $@"
<!DOCTYPE html>
<html>
<head>
    <style>
        body {{ font-family: Arial, sans-serif; }}
        .container {{ max-width: 600px; margin: 0 auto; padding: 20px; }}
        .header {{ background-color: #28a745; color: white; padding: 20px; border-radius: 5px; }}
        .content {{ padding: 20px; }}
        .footer {{ color: #666; font-size: 12px; margin-top: 20px; }}
    </style>
</head>
<body>
    <div class='container'>
        <div class='header'>
            <h1>Welcome to NtripCaster!</h1>
        </div>
        <div class='content'>
            <p>Hi {fullName},</p>
            <p>Your email has been verified and your account is now active!</p>
            <p>You can now:</p>
            <ul>
                <li>Log in to your account</li>
                <li>Create and manage mount points</li>
                <li>Connect GNSS sources to distribute corrections</li>
                <li>Connect clients to receive corrections</li>
            </ul>
            <p>If you have any questions, please contact our support team.</p>
        </div>
        <div class='footer'>
            <p>&copy; 2025 NtripCaster. All rights reserved.</p>
        </div>
    </div>
</body>
</html>";
    }

    private string GenerateSourceOfflineEmailHtml(string fullName, string sourceName, string mountPointName)
    {
        return $@"
<!DOCTYPE html>
<html>
<head>
    <style>
        body {{ font-family: Arial, sans-serif; }}
        .container {{ max-width: 600px; margin: 0 auto; padding: 20px; }}
        .header {{ background-color: #dc3545; color: white; padding: 20px; border-radius: 5px; }}
        .content {{ padding: 20px; }}
        .alert-box {{
            background-color: #fff3cd;
            border-left: 4px solid #dc3545;
            padding: 15px;
            margin: 15px 0;
            border-radius: 3px;
        }}
        .footer {{ color: #666; font-size: 12px; margin-top: 20px; }}
    </style>
</head>
<body>
    <div class='container'>
        <div class='header'>
            <h1>⚠️ GNSS Source Offline Alert</h1>
        </div>
        <div class='content'>
            <p>Hi {fullName},</p>
            <div class='alert-box'>
                <strong>Source Name:</strong> {sourceName}<br>
                <strong>Mount Point:</strong> {mountPointName}<br>
                <strong>Status:</strong> OFFLINE<br>
                <strong>Time:</strong> {DateTime.UtcNow:F}
            </div>
            <p>A GNSS source that you are monitoring has gone offline. This may impact your RTK correction distribution.</p>
            <p><strong>Action Required:</strong></p>
            <ul>
                <li>Check the source device connection</li>
                <li>Verify network connectivity</li>
                <li>Review system logs for error details</li>
                <li>If the problem persists, contact your administrator</li>
            </ul>
            <p>You will receive another notification when the source comes back online.</p>
        </div>
        <div class='footer'>
            <p>&copy; 2025 NtripCaster. All rights reserved.</p>
        </div>
    </div>
</body>
</html>";
    }

    private string GenerateSourceOnlineEmailHtml(string fullName, string sourceName, string mountPointName)
    {
        return $@"
<!DOCTYPE html>
<html>
<head>
    <style>
        body {{ font-family: Arial, sans-serif; }}
        .container {{ max-width: 600px; margin: 0 auto; padding: 20px; }}
        .header {{ background-color: #28a745; color: white; padding: 20px; border-radius: 5px; }}
        .content {{ padding: 20px; }}
        .success-box {{
            background-color: #d4edda;
            border-left: 4px solid #28a745;
            padding: 15px;
            margin: 15px 0;
            border-radius: 3px;
        }}
        .footer {{ color: #666; font-size: 12px; margin-top: 20px; }}
    </style>
</head>
<body>
    <div class='container'>
        <div class='header'>
            <h1>✅ GNSS Source Online</h1>
        </div>
        <div class='content'>
            <p>Hi {fullName},</p>
            <div class='success-box'>
                <strong>Source Name:</strong> {sourceName}<br>
                <strong>Mount Point:</strong> {mountPointName}<br>
                <strong>Status:</strong> ONLINE<br>
                <strong>Time:</strong> {DateTime.UtcNow:F}
            </div>
            <p>The GNSS source is now back online and operational. RTK corrections are being distributed normally.</p>
            <p>Your system should continue to operate without interruption.</p>
        </div>
        <div class='footer'>
            <p>&copy; 2025 NtripCaster. All rights reserved.</p>
        </div>
    </div>
</body>
</html>";
    }

    private string GenerateTestEmailHtml()
    {
        return $@"
<!DOCTYPE html>
<html>
<head>
    <style>
        body {{ font-family: Arial, sans-serif; }}
        .container {{ max-width: 600px; margin: 0 auto; padding: 20px; }}
        .header {{ background-color: #007bff; color: white; padding: 20px; border-radius: 5px; }}
        .content {{ padding: 20px; }}
        .test-box {{
            background-color: #e7f3ff;
            border-left: 4px solid #007bff;
            padding: 15px;
            margin: 15px 0;
            border-radius: 3px;
        }}
        .footer {{ color: #666; font-size: 12px; margin-top: 20px; }}
    </style>
</head>
<body>
    <div class='container'>
        <div class='header'>
            <h1>🧪 Test Email from NtripCaster</h1>
        </div>
        <div class='content'>
            <div class='test-box'>
                <strong>Email Configuration Status:</strong> ✅ Working<br>
                <strong>Test Time:</strong> {DateTime.UtcNow:F}<br>
                <strong>Server:</strong> NtripCaster Admin
            </div>
            <p>This is a test email to verify that your email configuration is working correctly.</p>
            <p>If you received this email, the SMTP configuration is properly set up and all email notifications will be delivered as expected.</p>
            <p><strong>Next Steps:</strong></p>
            <ul>
                <li>Verify email delivery to this address</li>
                <li>Check email formatting and styling</li>
                <li>Enable email triggers for source online/offline notifications</li>
            </ul>
        </div>
        <div class='footer'>
            <p>&copy; 2025 NtripCaster. All rights reserved.</p>
        </div>
    </div>
</body>
</html>";
    }
}
