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
}
