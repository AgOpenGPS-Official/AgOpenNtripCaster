#############################################################################
# AgOpen Ntripcaster - Interactive Setup Script (PowerShell)
#
# This script helps you configure AgOpen Ntripcaster for deployment
# by gathering all necessary environment variables and generating
# configuration files.
#
# Usage: .\setup.ps1
#############################################################################

# Require administrator privileges
if (-not ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]"Administrator")) {
    Write-Host "This script requires administrator privileges. Please run as Administrator." -ForegroundColor Red
    exit 1
}

# Functions
function Write-Title {
    param([string]$text)
    Write-Host "`n`n" -NoNewline
    Write-Host "╔════════════════════════════════════════════════════════════╗" -ForegroundColor Blue
    Write-Host "║" -ForegroundColor Blue -NoNewline
    Write-Host (" " * 62) -NoNewline
    Write-Host "║" -ForegroundColor Blue
    Write-Host "║" -ForegroundColor Blue -NoNewline
    Write-Host ($text.PadRight(62)) -NoNewline
    Write-Host "║" -ForegroundColor Blue
    Write-Host "║" -ForegroundColor Blue -NoNewline
    Write-Host (" " * 62) -NoNewline
    Write-Host "║" -ForegroundColor Blue
    Write-Host "╚════════════════════════════════════════════════════════════╝" -ForegroundColor Blue
}

function Write-Section {
    param([string]$text)
    Write-Host "`n━━━ $text ━━━`n" -ForegroundColor Blue
}

function Prompt-Value {
    param(
        [string]$prompt,
        [string]$default = "",
        [ValidateScript({ $true })]
        [scriptblock]$validator = { $true }
    )

    while ($true) {
        if ([string]::IsNullOrEmpty($default)) {
            $input = Read-Host -Prompt $prompt
        } else {
            $input = Read-Host -Prompt "$prompt [$default]"
            if ([string]::IsNullOrEmpty($input)) {
                $input = $default
            }
        }

        if (& $validator $input) {
            return $input
        } else {
            Write-Host "Invalid input. Please try again." -ForegroundColor Red
        }
    }
}

function Test-ValidDomain {
    param([string]$domain)
    return $domain -match "^([a-z0-9]([a-z0-9-]{0,61}[a-z0-9])?\.)+[a-z]{2,}$"
}

function Test-ValidEmail {
    param([string]$email)
    return $email -match "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$"
}

function Test-ValidPort {
    param([string]$port)
    return $port -match "^\d+$" -and [int]$port -gt 0 -and [int]$port -lt 65536
}

function Test-NotEmpty {
    param([string]$value)
    return -not [string]::IsNullOrEmpty($value)
}

function Generate-RandomString {
    param([int]$length = 32)
    $chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
    $random = New-Object Random
    $result = ""
    for ($i = 0; $i -lt $length; $i++) {
        $result += $chars[$random.Next($chars.Length)]
    }
    return $result
}

# Clear screen and show title
Clear-Host
Write-Title "AgOpen Ntripcaster - Interactive Setup Script (Windows)"

# ============================================================
# SECTION 1: Server Configuration
# ============================================================
Write-Section "SECTION 1: SERVER CONFIGURATION"

$serverDomain = Prompt-Value -prompt "Server Domain (e.g., ntripcaster.example.com)" -validator $function:Test-ValidDomain
$serverHost = Prompt-Value -prompt "Server Host/IP (for binding)" -default "0.0.0.0" -validator $function:Test-NotEmpty
$serverPort = Prompt-Value -prompt "Backend API Port" -default "5000" -validator $function:Test-ValidPort
$ntripPort = Prompt-Value -prompt "NTRIP Server Port" -default "2101" -validator $function:Test-ValidPort

# ============================================================
# SECTION 2: Database Configuration
# ============================================================
Write-Section "SECTION 2: DATABASE CONFIGURATION"

$dbHost = Prompt-Value -prompt "PostgreSQL Host" -default "postgres" -validator $function:Test-NotEmpty
$dbPort = Prompt-Value -prompt "PostgreSQL Port" -default "5432" -validator $function:Test-ValidPort
$dbName = Prompt-Value -prompt "Database Name" -default "ntripcaster_db" -validator $function:Test-NotEmpty
$dbUser = Prompt-Value -prompt "Database User" -default "ntripcaster" -validator $function:Test-NotEmpty
$dbPassword = Prompt-Value -prompt "Database Password" -validator $function:Test-NotEmpty
$dbConnectionString = "Server=$dbHost;Port=$dbPort;Database=$dbName;User Id=$dbUser;Password=$dbPassword;"

# ============================================================
# SECTION 3: JWT & Security
# ============================================================
Write-Section "SECTION 3: JWT & SECURITY"

$jwtSecret = Generate-RandomString -length 32
Write-Host "✓ Generated JWT Secret" -ForegroundColor Green

$adminPassword = Prompt-Value -prompt "Initial Admin Password" -validator $function:Test-NotEmpty
$sourcePasswordSalt = Generate-RandomString -length 16
Write-Host "✓ Generated Source Password Salt" -ForegroundColor Green

# ============================================================
# SECTION 4: Email Configuration
# ============================================================
Write-Section "SECTION 4: EMAIL CONFIGURATION"

$enableEmail = Prompt-Value -prompt "Enable Email Notifications? (yes/no)" -default "yes" -validator $function:Test-NotEmpty

if ($enableEmail -eq "yes" -or $enableEmail -eq "y") {
    $smtpHost = Prompt-Value -prompt "SMTP Server Host (e.g., smtp.gmail.com)" -validator $function:Test-NotEmpty
    $smtpPort = Prompt-Value -prompt "SMTP Port" -default "587" -validator $function:Test-ValidPort
    $smtpUser = Prompt-Value -prompt "SMTP Username/Email" -validator $function:Test-ValidEmail
    $smtpPassword = Prompt-Value -prompt "SMTP Password" -validator $function:Test-NotEmpty
    $smtpFromEmail = Prompt-Value -prompt "From Email Address" -default $smtpUser -validator $function:Test-ValidEmail
    $smtpFromName = Prompt-Value -prompt "From Display Name" -default "AgOpen Ntripcaster" -validator $function:Test-NotEmpty
    $emailAlertsEnabled = "true"
} else {
    $smtpHost = ""
    $smtpPort = "587"
    $smtpUser = ""
    $smtpPassword = ""
    $smtpFromEmail = ""
    $smtpFromName = "AgOpen Ntripcaster"
    $emailAlertsEnabled = "false"
}

# ============================================================
# SECTION 5: SSL/TLS Configuration
# ============================================================
Write-Section "SECTION 5: SSL/TLS CONFIGURATION"

Write-Host "SSL/TLS Options:"
Write-Host "  1 = Let's Encrypt (recommended)"
Write-Host "  2 = Self-signed"
Write-Host "  3 = Existing certificates"
Write-Host ""

$sslOption = Prompt-Value -prompt "Choose SSL/TLS option (1/2/3)" -default "1" -validator $function:Test-NotEmpty

switch ($sslOption) {
    "1" {
        $sslMethod = "letsencrypt"
        $leEmail = Prompt-Value -prompt "Let's Encrypt Email" -validator $function:Test-ValidEmail
        $certPath = "C:\nginx\certs"
        $keyPath = "C:\nginx\certs"
    }
    "2" {
        $sslMethod = "self-signed"
        $certPath = "C:\nginx\certs"
        $keyPath = "C:\nginx\certs"
        $leEmail = ""
        Write-Host "Note: Self-signed certificate will be generated during deployment" -ForegroundColor Yellow
    }
    "3" {
        $sslMethod = "existing"
        $certPath = Prompt-Value -prompt "Path to existing certificate" -default "C:\nginx\certs\cert.pem"
        $keyPath = Prompt-Value -prompt "Path to existing private key" -default "C:\nginx\certs\key.pem"
        $leEmail = ""
    }
    default {
        $sslMethod = "letsencrypt"
        $leEmail = ""
    }
}

# ============================================================
# SECTION 6: Docker Configuration
# ============================================================
Write-Section "SECTION 6: DOCKER CONFIGURATION"

$dockerRegistry = Prompt-Value -prompt "Docker Registry (empty for Docker Hub)" -default "" -validator { $true }
$imageTag = Prompt-Value -prompt "Image Tag/Version" -default "latest" -validator $function:Test-NotEmpty
$restartPolicy = Prompt-Value -prompt "Container Restart Policy (unless-stopped/always/on-failure)" -default "unless-stopped"

# ============================================================
# SECTION 7: Backup & Logging
# ============================================================
Write-Section "SECTION 7: BACKUP & LOGGING"

$backupEnabled = Prompt-Value -prompt "Enable Automated Backups? (yes/no)" -default "yes"
$logLevel = Prompt-Value -prompt "Log Level (Debug/Information/Warning/Error)" -default "Information"
$backupRetentionDays = Prompt-Value -prompt "Backup Retention (days)" -default "30" -validator $function:Test-ValidPort

# ============================================================
# Generate .env file
# ============================================================
Write-Section "GENERATING CONFIGURATION FILES"

$envContent = @"
# ============================================================
# AgOpen Ntripcaster Environment Configuration
# Generated: $(Get-Date)
# ============================================================

# Server Configuration
SERVER_DOMAIN=$serverDomain
SERVER_HOST=$serverHost
SERVER_PORT=$serverPort
NTRIP_PORT=$ntripPort
ASPNETCORE_URLS=http://0.0.0.0:$serverPort

# Database Configuration
DB_CONNECTION_STRING=$dbConnectionString
DB_HOST=$dbHost
DB_PORT=$dbPort
DB_NAME=$dbName
DB_USER=$dbUser
DB_PASSWORD=$dbPassword

# JWT Configuration
JWT_SECRET=$jwtSecret
JWT_EXPIRATION_HOURS=24
JWT_REFRESH_EXPIRATION_DAYS=30

# Initial Admin
ADMIN_EMAIL=admin@$serverDomain
ADMIN_PASSWORD=$adminPassword

# Email Configuration
SMTP_ENABLED=$emailAlertsEnabled
SMTP_HOST=$smtpHost
SMTP_PORT=$smtpPort
SMTP_USERNAME=$smtpUser
SMTP_PASSWORD=$smtpPassword
SMTP_FROM_EMAIL=$smtpFromEmail
SMTP_FROM_NAME=$smtpFromName

# SSL/TLS Configuration
SSL_METHOD=$sslMethod
SSL_CERT_PATH=$certPath\fullchain.pem
SSL_KEY_PATH=$keyPath\privkey.pem
LE_EMAIL=$leEmail

# Docker Configuration
DOCKER_REGISTRY=$dockerRegistry
IMAGE_TAG=$imageTag
RESTART_POLICY=$restartPolicy

# Logging
LOG_LEVEL=$logLevel
ASPNETCORE_ENVIRONMENT=Production

# Backup Configuration
BACKUP_ENABLED=$backupEnabled
BACKUP_RETENTION_DAYS=$backupRetentionDays

# Security
CORS_ORIGINS=https://$serverDomain,http://localhost:3000
ALLOW_REGISTRATION=true
"@

Set-Content -Path ".env" -Value $envContent -Force
Write-Host "✓ Created .env file" -ForegroundColor Green

# ============================================================
# Generate docker-compose.yml snippet
# ============================================================
$dockerComposeSnippet = @"
# Add this to your docker-compose.yml

services:
  backend:
    environment:
      - ASPNETCORE_URLS=http://0.0.0.0:$serverPort
      - DB_CONNECTION_STRING=$dbConnectionString
      - JWT_SECRET=$jwtSecret

  postgres:
    environment:
      - POSTGRES_DB=$dbName
      - POSTGRES_USER=$dbUser
      - POSTGRES_PASSWORD=$dbPassword
"@

Set-Content -Path "docker-compose.env.snippet" -Value $dockerComposeSnippet -Force
Write-Host "✓ Created docker-compose.env.snippet file" -ForegroundColor Green

# ============================================================
# Summary
# ============================================================
Write-Host "`n" -NoNewline
Write-Host "Ready for Deployment!" -ForegroundColor Green
Write-Host "`n"
Write-Host "Configuration Summary:" -ForegroundColor Green
Write-Host "  Domain: $serverDomain"
Write-Host "  Database: $dbUser@$dbHost`:$dbPort/$dbName"
Write-Host "  Backend Port: $serverPort"
Write-Host "  NTRIP Port: $ntripPort"
Write-Host "  SSL Method: $sslMethod"
Write-Host ""

Write-Host "Next Steps:" -ForegroundColor Yellow
Write-Host "1. Review .env file: cat .env"
Write-Host "2. (If using Let's Encrypt) Generate certificate using Certbot on your server"
Write-Host "3. Update docker-compose.yml with database credentials"
Write-Host "4. Start deployment: docker-compose up -d"
Write-Host ""

Write-Host "✓ Setup Complete!" -ForegroundColor Green
Write-Host ""
