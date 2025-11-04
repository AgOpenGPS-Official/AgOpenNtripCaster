#!/bin/bash

#####################################################################
# AgOpen Ntripcaster - Interactive Setup Script
#
# This script helps you configure AgOpen Ntripcaster for deployment
# by gathering all necessary environment variables and generating
# configuration files.
#
# Usage: ./setup.sh
#####################################################################

set -e

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Title
clear
echo -e "${BLUE}"
echo "╔════════════════════════════════════════════════════════════╗"
echo "║                                                            ║"
echo "║        AgOpen Ntripcaster - Interactive Setup Script       ║"
echo "║                                                            ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo -e "${NC}"

# Function to prompt user with validation
prompt_value() {
    local prompt_text="$1"
    local default_value="$2"
    local validation_fn="$3"
    local value=""

    while true; do
        if [ -z "$default_value" ]; then
            echo -ne "${YELLOW}${prompt_text}:${NC} "
        else
            echo -ne "${YELLOW}${prompt_text} [${default_value}]:${NC} "
        fi
        read -r value

        # Use default if empty
        if [ -z "$value" ] && [ -n "$default_value" ]; then
            value="$default_value"
        fi

        # Validate
        if [ -z "$validation_fn" ] || $validation_fn "$value"; then
            echo "$value"
            return 0
        else
            echo -e "${RED}Invalid input. Please try again.${NC}"
        fi
    done
}

validate_domain() {
    [[ "$1" =~ ^([a-z0-9]([a-z0-9-]{0,61}[a-z0-9])?\.)+[a-z]{2,}$ ]] && return 0 || return 1
}

validate_email() {
    [[ "$1" =~ ^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$ ]] && return 0 || return 1
}

validate_port() {
    [[ "$1" =~ ^[0-9]+$ ]] && [ "$1" -gt 0 ] && [ "$1" -lt 65536 ] && return 0 || return 1
}

validate_not_empty() {
    [ -n "$1" ] && return 0 || return 1
}

# ============================================================
# SECTION 1: Server Configuration
# ============================================================
echo -e "\n${BLUE}━━━ SECTION 1: SERVER CONFIGURATION ━━━${NC}\n"

SERVER_DOMAIN=$(prompt_value "Server Domain (e.g., ntripcaster.example.com)" "" "validate_domain")
SERVER_HOST=$(prompt_value "Server Host/IP (for nginx binding)" "0.0.0.0" "validate_not_empty")
SERVER_PORT=$(prompt_value "Backend API Port" "5000" "validate_port")
NTRIP_PORT=$(prompt_value "NTRIP Server Port" "2101" "validate_port")

# ============================================================
# SECTION 2: Database Configuration
# ============================================================
echo -e "\n${BLUE}━━━ SECTION 2: DATABASE CONFIGURATION ━━━${NC}\n"

DB_HOST=$(prompt_value "PostgreSQL Host" "postgres" "validate_not_empty")
DB_PORT=$(prompt_value "PostgreSQL Port" "5432" "validate_port")
DB_NAME=$(prompt_value "Database Name" "ntripcaster_db" "validate_not_empty")
DB_USER=$(prompt_value "Database User" "ntripcaster" "validate_not_empty")
DB_PASSWORD=$(prompt_value "Database Password" "" "validate_not_empty")
DB_CONNECTION_STRING="Server=$DB_HOST;Port=$DB_PORT;Database=$DB_NAME;User Id=$DB_USER;Password=$DB_PASSWORD;"

# ============================================================
# SECTION 3: JWT & Security
# ============================================================
echo -e "\n${BLUE}━━━ SECTION 3: JWT & SECURITY ━━━${NC}\n"

# Generate random JWT secret (32+ chars)
JWT_SECRET=$(openssl rand -base64 32 2>/dev/null || head -c 32 /dev/urandom | base64)
echo -e "${GREEN}✓ Generated JWT Secret${NC}"

# Generate random admin password
ADMIN_PASSWORD=$(prompt_value "Initial Admin Password" "" "validate_not_empty")

# Generate random source password salt
SOURCE_PASSWORD_SALT=$(openssl rand -base64 16 2>/dev/null || head -c 16 /dev/urandom | base64)
echo -e "${GREEN}✓ Generated Source Password Salt${NC}"

# ============================================================
# SECTION 4: Email Configuration
# ============================================================
echo -e "\n${BLUE}━━━ SECTION 4: EMAIL CONFIGURATION ━━━${NC}\n"

ENABLE_EMAIL=$(prompt_value "Enable Email Notifications? (yes/no)" "yes" "validate_not_empty")

if [[ "$ENABLE_EMAIL" == "yes" || "$ENABLE_EMAIL" == "y" ]]; then
    SMTP_HOST=$(prompt_value "SMTP Server Host (e.g., smtp.gmail.com)" "" "validate_not_empty")
    SMTP_PORT=$(prompt_value "SMTP Port" "587" "validate_port")
    SMTP_USER=$(prompt_value "SMTP Username/Email" "" "validate_email")
    SMTP_PASSWORD=$(prompt_value "SMTP Password" "" "validate_not_empty")
    SMTP_FROM_EMAIL=$(prompt_value "From Email Address" "$SMTP_USER" "validate_email")
    SMTP_FROM_NAME=$(prompt_value "From Display Name" "AgOpen Ntripcaster" "validate_not_empty")
    EMAIL_ALERTS_ENABLED="true"
else
    SMTP_HOST=""
    SMTP_PORT="587"
    SMTP_USER=""
    SMTP_PASSWORD=""
    SMTP_FROM_EMAIL=""
    SMTP_FROM_NAME="AgOpen Ntripcaster"
    EMAIL_ALERTS_ENABLED="false"
fi

# ============================================================
# SECTION 5: SSL/TLS Configuration
# ============================================================
echo -e "\n${BLUE}━━━ SECTION 5: SSL/TLS CONFIGURATION ━━━${NC}\n"

SSL_OPTION=$(prompt_value "SSL/TLS Option: (1=Let's Encrypt, 2=Self-signed, 3=Existing certs)" "1" "validate_not_empty")

if [ "$SSL_OPTION" == "1" ]; then
    SSL_METHOD="letsencrypt"
    LE_EMAIL=$(prompt_value "Let's Encrypt Email" "" "validate_email")
    CERT_PATH="/etc/letsencrypt/live/$SERVER_DOMAIN"
    KEY_PATH="/etc/letsencrypt/live/$SERVER_DOMAIN"
elif [ "$SSL_OPTION" == "2" ]; then
    SSL_METHOD="self-signed"
    CERT_PATH="/etc/nginx/certs"
    KEY_PATH="/etc/nginx/certs"
    LE_EMAIL=""
    echo -e "${YELLOW}Note: Self-signed certificate will be generated during deployment${NC}"
elif [ "$SSL_OPTION" == "3" ]; then
    SSL_METHOD="existing"
    CERT_PATH=$(prompt_value "Path to existing certificate" "/etc/nginx/certs/cert.pem" "validate_not_empty")
    KEY_PATH=$(prompt_value "Path to existing private key" "/etc/nginx/certs/key.pem" "validate_not_empty")
    LE_EMAIL=""
else
    SSL_METHOD="letsencrypt"
    LE_EMAIL=""
fi

# ============================================================
# SECTION 6: Docker Configuration
# ============================================================
echo -e "\n${BLUE}━━━ SECTION 6: DOCKER CONFIGURATION ━━━${NC}\n"

DOCKER_REGISTRY=$(prompt_value "Docker Registry (empty for Docker Hub)" "" "validate_not_empty")
IMAGE_TAG=$(prompt_value "Image Tag/Version" "latest" "validate_not_empty")
RESTART_POLICY=$(prompt_value "Container Restart Policy (unless-stopped/always/on-failure)" "unless-stopped" "validate_not_empty")

# ============================================================
# SECTION 7: Backup & Logging
# ============================================================
echo -e "\n${BLUE}━━━ SECTION 7: BACKUP & LOGGING ━━━${NC}\n"

BACKUP_ENABLED=$(prompt_value "Enable Automated Backups? (yes/no)" "yes" "validate_not_empty")
LOG_LEVEL=$(prompt_value "Log Level (Debug/Information/Warning/Error)" "Information" "validate_not_empty")
BACKUP_RETENTION_DAYS=$(prompt_value "Backup Retention (days)" "30" "validate_port")

# ============================================================
# Generate .env file
# ============================================================
echo -e "\n${BLUE}━━━ GENERATING CONFIGURATION FILES ━━━${NC}\n"

cat > ".env" << EOF
# ============================================================
# AgOpen Ntripcaster Environment Configuration
# Generated: $(date)
# ============================================================

# Server Configuration
SERVER_DOMAIN=$SERVER_DOMAIN
SERVER_HOST=$SERVER_HOST
SERVER_PORT=$SERVER_PORT
NTRIP_PORT=$NTRIP_PORT
ASPNETCORE_URLS=http://0.0.0.0:$SERVER_PORT

# Database Configuration
DB_CONNECTION_STRING=$DB_CONNECTION_STRING
DB_HOST=$DB_HOST
DB_PORT=$DB_PORT
DB_NAME=$DB_NAME
DB_USER=$DB_USER
DB_PASSWORD=$DB_PASSWORD

# JWT Configuration
JWT_SECRET=$JWT_SECRET
JWT_EXPIRATION_HOURS=24
JWT_REFRESH_EXPIRATION_DAYS=30

# Initial Admin
ADMIN_EMAIL=admin@$SERVER_DOMAIN
ADMIN_PASSWORD=$ADMIN_PASSWORD

# Email Configuration
SMTP_ENABLED=$EMAIL_ALERTS_ENABLED
SMTP_HOST=$SMTP_HOST
SMTP_PORT=$SMTP_PORT
SMTP_USERNAME=$SMTP_USER
SMTP_PASSWORD=$SMTP_PASSWORD
SMTP_FROM_EMAIL=$SMTP_FROM_EMAIL
SMTP_FROM_NAME=$SMTP_FROM_NAME

# SSL/TLS Configuration
SSL_METHOD=$SSL_METHOD
SSL_CERT_PATH=$CERT_PATH/fullchain.pem
SSL_KEY_PATH=$KEY_PATH/privkey.pem
LE_EMAIL=$LE_EMAIL

# Docker Configuration
DOCKER_REGISTRY=$DOCKER_REGISTRY
IMAGE_TAG=$IMAGE_TAG
RESTART_POLICY=$RESTART_POLICY

# Logging
LOG_LEVEL=$LOG_LEVEL
ASPNETCORE_ENVIRONMENT=Production

# Backup Configuration
BACKUP_ENABLED=$BACKUP_ENABLED
BACKUP_RETENTION_DAYS=$BACKUP_RETENTION_DAYS

# Security
CORS_ORIGINS=https://$SERVER_DOMAIN,http://localhost:3000
ALLOW_REGISTRATION=true
EOF

echo -e "${GREEN}✓ Created .env file${NC}"

# ============================================================
# Generate nginx configuration
# ============================================================
cat > "nginx.conf" << 'EOF'
# ============================================================
# AgOpen Ntripcaster Nginx Configuration
# Generated by setup.sh
# ============================================================

user nginx;
worker_processes auto;
error_log /var/log/nginx/error.log warn;
pid /var/run/nginx.pid;

events {
    worker_connections 1024;
}

http {
    include /etc/nginx/mime.types;
    default_type application/octet-stream;

    log_format main '$remote_addr - $remote_user [$time_local] "$request" '
                    '$status $body_bytes_sent "$http_referer" '
                    '"$http_user_agent" "$http_x_forwarded_for"';

    access_log /var/log/nginx/access.log main;

    sendfile on;
    tcp_nopush on;
    tcp_nodelay on;
    keepalive_timeout 65;
    types_hash_max_size 2048;
    client_max_body_size 20M;

    # Gzip compression
    gzip on;
    gzip_vary on;
    gzip_proxied any;
    gzip_comp_level 6;
    gzip_types text/plain text/css text/xml text/javascript
               application/json application/javascript application/xml+rss
               application/rss+xml font/truetype font/opentype
               application/vnd.ms-fontobject image/svg+xml;

    # Rate limiting
    limit_req_zone $binary_remote_addr zone=general:10m rate=10r/s;
    limit_req_zone $binary_remote_addr zone=api:10m rate=20r/s;
    limit_req_zone $binary_remote_addr zone=auth:10m rate=5r/m;

    # Redirect HTTP to HTTPS
    server {
        listen 80;
        listen [::]:80;
        server_name _;
        return 301 https://$host$request_uri;
    }

    # HTTPS Server
    server {
        listen 443 ssl http2;
        listen [::]:443 ssl http2;
        server_name DOMAIN_PLACEHOLDER;

        # SSL Configuration
        ssl_certificate /etc/nginx/certs/fullchain.pem;
        ssl_certificate_key /etc/nginx/certs/privkey.pem;
        ssl_protocols TLSv1.2 TLSv1.3;
        ssl_ciphers HIGH:!aNULL:!MD5;
        ssl_prefer_server_ciphers on;
        ssl_session_cache shared:SSL:10m;
        ssl_session_timeout 10m;

        # Security Headers
        add_header Strict-Transport-Security "max-age=31536000; includeSubDomains" always;
        add_header X-Frame-Options "SAMEORIGIN" always;
        add_header X-Content-Type-Options "nosniff" always;
        add_header X-XSS-Protection "1; mode=block" always;
        add_header Referrer-Policy "strict-origin-when-cross-origin" always;

        # Frontend (React)
        location / {
            proxy_pass http://localhost:5173;
            proxy_http_version 1.1;
            proxy_set_header Upgrade $http_upgrade;
            proxy_set_header Connection 'upgrade';
            proxy_set_header Host $host;
            proxy_set_header X-Real-IP $remote_addr;
            proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
            proxy_set_header X-Forwarded-Proto $scheme;
            proxy_cache_bypass $http_upgrade;

            # Rate limiting
            limit_req zone=general burst=20 nodelay;
        }

        # API Endpoints
        location /api/ {
            proxy_pass http://localhost:5000;
            proxy_http_version 1.1;
            proxy_set_header Host $host;
            proxy_set_header X-Real-IP $remote_addr;
            proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
            proxy_set_header X-Forwarded-Proto $scheme;

            # Rate limiting for API
            limit_req zone=api burst=50 nodelay;
        }

        # SignalR WebSocket
        location /signalr/ {
            proxy_pass http://localhost:5000;
            proxy_http_version 1.1;
            proxy_set_header Upgrade $http_upgrade;
            proxy_set_header Connection "upgrade";
            proxy_set_header Host $host;
            proxy_set_header X-Real-IP $remote_addr;
            proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
            proxy_set_header X-Forwarded-Proto $scheme;
            proxy_read_timeout 86400;
        }

        # Authentication endpoint (stricter rate limiting)
        location /api/auth/ {
            proxy_pass http://localhost:5000;
            proxy_set_header Host $host;
            proxy_set_header X-Real-IP $remote_addr;
            proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;

            limit_req zone=auth burst=10 nodelay;
        }

        # Static files caching
        location ~* \.(js|css|png|jpg|jpeg|gif|ico|svg|woff|woff2|ttf|eot)$ {
            proxy_pass http://localhost:5173;
            expires 30d;
            add_header Cache-Control "public, immutable";
        }

        # Health check endpoint
        location /health {
            access_log off;
            return 200 "healthy\n";
            add_header Content-Type text/plain;
        }
    }
}
EOF

echo -e "${GREEN}✓ Created nginx.conf file${NC}"

# ============================================================
# Generate docker-compose.yml if needed
# ============================================================
echo -e "\n${BLUE}Ready for Deployment!${NC}\n"

echo -e "${GREEN}✓ Configuration Summary:${NC}"
echo "  Domain: $SERVER_DOMAIN"
echo "  Database: $DB_USER@$DB_HOST:$DB_PORT/$DB_NAME"
echo "  Backend Port: $SERVER_PORT"
echo "  NTRIP Port: $NTRIP_PORT"
echo "  SSL Method: $SSL_METHOD"
echo ""

echo -e "${YELLOW}Next Steps:${NC}"
echo "1. Review .env file: cat .env"
echo "2. Review nginx.conf file: cat nginx.conf"
echo "3. (If using Let's Encrypt) Install Certbot:"
echo "   sudo apt-get install certbot python3-certbot-nginx"
echo "4. (If using Let's Encrypt) Generate certificate:"
echo "   sudo certbot certonly --standalone -d $SERVER_DOMAIN"
echo "5. Start deployment: ./deploy.sh"
echo ""

echo -e "${GREEN}✓ Setup Complete!${NC}\n"
