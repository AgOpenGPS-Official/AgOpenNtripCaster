# 🚀 AgOpen Ntripcaster - Setup Guide

This guide helps you configure AgOpen Ntripcaster for deployment using automated setup scripts.

---

## Quick Start

### Option 1: Automated Setup (Recommended)

#### Linux/Mac
```bash
cd deploy
chmod +x setup.sh
./setup.sh
```

#### Windows (PowerShell)
```powershell
cd deploy
# Run PowerShell as Administrator, then:
.\setup.ps1
```

### Option 2: Manual Configuration

```bash
cd deploy
cp .env.example .env
nano .env  # Edit with your values
```

---

## What the Setup Scripts Do

The setup scripts (`setup.sh` for Linux/Mac and `setup.ps1` for Windows) are interactive tools that:

1. **Gather Configuration** - Prompts you for all necessary settings
2. **Validate Input** - Ensures valid domain names, emails, ports, etc.
3. **Generate Secrets** - Creates secure JWT secrets and passwords
4. **Create .env File** - Generates your environment configuration
5. **Generate Nginx Config** - Creates nginx reverse proxy configuration (Linux only)
6. **Provide Next Steps** - Shows you what to do after setup

---

## Configuration Sections

### 1. Server Configuration
```
Server Domain: ntripcaster.example.com
Server Host: 0.0.0.0 (all interfaces)
Backend API Port: 5000
NTRIP Server Port: 2101
```

### 2. Database Configuration
```
Host: postgres (or your PostgreSQL server)
Port: 5432
Database: ntripcaster_db
User: ntripcaster
Password: (securely generated)
```

### 3. JWT & Security
```
JWT Secret: (auto-generated, 32+ chars)
Admin Password: (you provide)
Source Password Salt: (auto-generated)
```

### 4. Email Configuration
```
SMTP Host: smtp.gmail.com (or your SMTP server)
SMTP Port: 587 (TLS) or 465 (SSL)
SMTP User: your-email@gmail.com
SMTP Password: (your app password)
From Email: noreply@example.com
```

### 5. SSL/TLS Options
```
1. Let's Encrypt (recommended)
   - Automatic certificate generation
   - Auto-renewal
   - Free

2. Self-Signed
   - For testing/development
   - Not trusted by browsers

3. Existing Certificates
   - Use your own certificates
   - Point to cert/key paths
```

### 6. Docker Configuration
```
Registry: (empty for Docker Hub)
Image Tag: latest or specific version
Restart Policy: unless-stopped (recommended)
```

### 7. Backup & Logging
```
Automated Backups: yes/no
Log Level: Information (or Debug/Warning/Error)
Backup Retention: 30 days
```

---

## Email Setup Guide

### Gmail (Recommended)
1. Enable 2-Factor Authentication on your Google Account
2. Go to: https://myaccount.google.com/apppasswords
3. Select "Mail" and "Windows Computer" (or your device)
4. Google generates an app password (16 characters)
5. Use this app password in `SMTP_PASSWORD`

### Office 365
```
SMTP_HOST=smtp.office365.com
SMTP_PORT=587
SMTP_USERNAME=your-email@company.com
SMTP_PASSWORD=your-password
```

### Custom SMTP Server
```
SMTP_HOST=mail.example.com
SMTP_PORT=587 (TLS) or 465 (SSL)
SMTP_USERNAME=your-username
SMTP_PASSWORD=your-password
```

---

## SSL/TLS Setup

### Option 1: Let's Encrypt (Linux/Mac)

**After running setup.sh:**

```bash
# Install Certbot
sudo apt-get install certbot python3-certbot-nginx

# Generate certificate
sudo certbot certonly --standalone -d your-domain.com

# Certificate location
/etc/letsencrypt/live/your-domain.com/fullchain.pem
/etc/letsencrypt/live/your-domain.com/privkey.pem

# Auto-renewal (runs daily)
sudo systemctl enable certbot.timer
```

### Option 2: Self-Signed (Testing)

```bash
# Generate self-signed certificate
sudo openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
  -keyout /etc/nginx/certs/privkey.pem \
  -out /etc/nginx/certs/fullchain.pem

# Set permissions
sudo chmod 644 /etc/nginx/certs/*
```

### Option 3: Existing Certificates

Just point to your certificate paths in the setup script.

---

## Post-Setup Steps

After running the setup script:

### 1. Review Generated Files
```bash
cat .env              # Review environment variables
cat nginx.conf        # Review nginx configuration (Linux)
```

### 2. Verify Database Connection
```bash
# Test PostgreSQL connection
psql -h $DB_HOST -U $DB_USER -d $DB_NAME -c "SELECT version();"
```

### 3. (Linux Only) Install SSL Certificate
```bash
# If using Let's Encrypt
sudo certbot certonly --standalone -d $SERVER_DOMAIN

# If using self-signed
sudo mkdir -p /etc/nginx/certs
sudo openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
  -keyout /etc/nginx/certs/privkey.pem \
  -out /etc/nginx/certs/fullchain.pem
```

### 4. Start Deployment
```bash
./deploy.sh
# or
docker-compose up -d
```

---

## Environment Variables Reference

| Variable | Required | Example | Notes |
|----------|----------|---------|-------|
| `SERVER_DOMAIN` | Yes | ntripcaster.example.com | Full domain name |
| `SERVER_PORT` | Yes | 5000 | Backend API port |
| `NTRIP_PORT` | Yes | 2101 | NTRIP protocol port |
| `DB_CONNECTION_STRING` | Yes | Server=... | Full connection string |
| `JWT_SECRET` | Yes | random_string_32+ | Minimum 32 characters |
| `ADMIN_PASSWORD` | Yes | strong_password | Change on first login |
| `SMTP_ENABLED` | No | true/false | Email notifications |
| `SMTP_HOST` | If email | smtp.gmail.com | SMTP server |
| `SSL_METHOD` | Yes | letsencrypt | letsencrypt, self-signed, existing |
| `LOG_LEVEL` | No | Information | Debug, Information, Warning, Error |

---

## Troubleshooting

### Setup Script Won't Run
```bash
# Linux/Mac
chmod +x setup.sh
./setup.sh

# Windows (run as Administrator)
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
.\setup.ps1
```

### Invalid Domain Error
- Use a valid domain name (e.g., `ntripcaster.example.com`)
- Avoid subdomains with hyphens at the start/end
- Example valid domains: `example.com`, `sub.example.com`, `my-ntrip.example.com`

### Email Test Fails
- Verify SMTP credentials are correct
- For Gmail, use app password (not your account password)
- Check firewall allows SMTP port (587 or 465)
- Verify email address is correctly formatted

### Certificate Generation Fails
- Ensure you can access your domain from the internet
- DNS must be configured and pointing to your server
- Port 80 must be accessible for Let's Encrypt validation
- Check firewall rules

### Database Connection Error
- Verify PostgreSQL is running
- Check DB_HOST is correct (localhost, 127.0.0.1, or server IP)
- Verify DB_USER and DB_PASSWORD
- Ensure database exists and user has permissions

---

## Security Best Practices

### Passwords
- Use strong, random passwords (20+ characters)
- Never use the same password in multiple places
- Change admin password on first login

### Secrets
- Keep .env file secure (never commit to git)
- Use different secrets for development and production
- Rotate secrets periodically

### Backups
- Enable automated backups (`BACKUP_ENABLED=true`)
- Test backup restoration regularly
- Keep backups in secure location

### SSL/TLS
- Always use HTTPS in production
- Keep certificates current
- Use strong cipher suites

---

## Advanced Configuration

### Custom Domain with Subdomain
```
SERVER_DOMAIN=ntrip.gps.example.com
ADMIN_EMAIL=admin@gps.example.com
```

### Custom SMTP Port (SSL)
```
SMTP_PORT=465
```

### Multiple NTRIP Servers
Run multiple instances with different:
- `NTRIP_PORT` (2101, 2102, 2103, ...)
- `DB_NAME` (ntripcaster_1, ntripcaster_2, ...)
- `SERVER_PORT` (5000, 5001, 5002, ...)

### Backup Schedule (Cron Format)
```
BACKUP_SCHEDULE=0 2 * * *     # Daily at 2:00 AM UTC
BACKUP_SCHEDULE=0 */6 * * *   # Every 6 hours
BACKUP_SCHEDULE=30 2 * * 0    # Weekly Sunday at 2:30 AM
```

---

## Support

For issues or questions:
- 📖 Check [DEPLOYMENT.md](./DEPLOYMENT.md)
- 🐛 Report issues on [GitHub](https://github.com/AgOpenGPS-Official/AgOpenNtripCaster/issues)
- 💬 Discuss on [GitHub Discussions](https://github.com/AgOpenGPS-Official/AgOpenNtripCaster/discussions)

---

**Ready to deploy?** Run the setup script and follow the prompts! 🚀
