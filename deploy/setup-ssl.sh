#!/bin/bash

################################################################################
# SSL/TLS Setup Script for NtripCaster
#
# This script sets up Let's Encrypt SSL certificates using Certbot
#
# Usage: ./setup-ssl.sh
################################################################################

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"
DEPLOY_DIR="$SCRIPT_DIR"

# Functions
print_header() {
    echo -e "\n${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║${NC} $1"
    echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}\n"
}

print_success() {
    echo -e "${GREEN}✓${NC} $1"
}

print_error() {
    echo -e "${RED}✗${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}⚠${NC} $1"
}

print_info() {
    echo -e "${BLUE}ℹ${NC} $1"
}

# Main setup process
main() {
    print_header "NtripCaster SSL/TLS Setup"

    # Check if running as root
    if [ "$EUID" -eq 0 ]; then
        print_warning "Running as root - this is fine for production setup"
    fi

    # Check if .env exists
    if [ ! -f "$DEPLOY_DIR/.env" ]; then
        print_error ".env file not found in deploy directory"
        print_info "Please create a .env file first (copy from .env.example)"
        exit 1
    fi

    # Read domain from .env or ask user
    DOMAIN=$(grep "^DOMAIN=" "$DEPLOY_DIR/.env" 2>/dev/null | cut -d'=' -f2 || echo "")
    EMAIL=$(grep "^SSL_EMAIL=" "$DEPLOY_DIR/.env" 2>/dev/null | cut -d'=' -f2 || echo "")

    if [ -z "$DOMAIN" ]; then
        echo -e "${BLUE}Enter your domain name (e.g., ntripcaster.example.com):${NC}"
        read -r DOMAIN

        if [ -z "$DOMAIN" ]; then
            print_error "Domain name is required"
            exit 1
        fi
    fi

    if [ -z "$EMAIL" ]; then
        echo -e "${BLUE}Enter your email for Let's Encrypt notifications:${NC}"
        read -r EMAIL

        if [ -z "$EMAIL" ]; then
            print_error "Email is required"
            exit 1
        fi
    fi

    print_info "Domain: $DOMAIN"
    print_info "Email: $EMAIL"

    # Update .env with domain and email if not already there
    if ! grep -q "^DOMAIN=" "$DEPLOY_DIR/.env"; then
        echo "DOMAIN=$DOMAIN" >> "$DEPLOY_DIR/.env"
        print_success "Added DOMAIN to .env"
    fi

    if ! grep -q "^SSL_EMAIL=" "$DEPLOY_DIR/.env"; then
        echo "SSL_EMAIL=$EMAIL" >> "$DEPLOY_DIR/.env"
        print_success "Added SSL_EMAIL to .env"
    fi

    # Check if services are running
    print_info "Checking if Docker services are running..."
    if ! docker compose -f "$DEPLOY_DIR/docker-compose.yml" ps --status running | grep -q "ntripcaster-nginx"; then
        print_error "NGINX is not running. Please start services first:"
        print_info "docker compose -f $DEPLOY_DIR/docker-compose.yml up -d"
        exit 1
    fi
    print_success "Services are running"

    # Check DNS
    print_info "Checking DNS resolution for $DOMAIN..."
    if ! host "$DOMAIN" > /dev/null 2>&1; then
        print_warning "DNS resolution failed for $DOMAIN"
        print_warning "Make sure your domain's A record points to this server's IP"
        echo -e "${YELLOW}Continue anyway? (y/N)${NC}"
        read -r continue_anyway
        if [ "$continue_anyway" != "y" ] && [ "$continue_anyway" != "Y" ]; then
            exit 1
        fi
    else
        print_success "DNS resolution OK"
    fi

    # Request certificate
    print_info "Requesting SSL certificate from Let's Encrypt..."
    print_warning "This may take a minute..."

    docker compose -f "$DEPLOY_DIR/docker-compose.yml" run --rm certbot certonly \
        --webroot \
        --webroot-path /var/www/certbot \
        --email "$EMAIL" \
        --agree-tos \
        --no-eff-email \
        -d "$DOMAIN"

    if [ $? -eq 0 ]; then
        print_success "SSL certificate obtained successfully!"
    else
        print_error "Failed to obtain SSL certificate"
        print_info "Common issues:"
        print_info "  1. Domain DNS not pointing to this server"
        print_info "  2. Port 80 not accessible from internet"
        print_info "  3. Firewall blocking HTTP traffic"
        exit 1
    fi

    # Enable SSL in NGINX config
    print_info "Enabling SSL in NGINX configuration..."

    # Create SSL-enabled NGINX config
    cat > "$DEPLOY_DIR/nginx/default.conf" << 'EOF'
# HTTP server - redirect to HTTPS
server {
    listen 80;
    server_name ${DOMAIN};

    # Let's Encrypt challenge location (for certbot renewal)
    location /.well-known/acme-challenge/ {
        root /var/www/certbot;
    }

    # Redirect all other HTTP traffic to HTTPS
    location / {
        return 301 https://$server_name$request_uri;
    }
}

# HTTPS server
server {
    listen 443 ssl http2;
    server_name ${DOMAIN};

    # SSL certificates
    ssl_certificate /etc/letsencrypt/live/${DOMAIN}/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/${DOMAIN}/privkey.pem;

    # SSL configuration
    ssl_protocols TLSv1.2 TLSv1.3;
    ssl_ciphers ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384;
    ssl_prefer_server_ciphers off;
    ssl_session_cache shared:SSL:10m;
    ssl_session_timeout 10m;

    # Root directory for static files
    root /usr/share/nginx/html;
    index index.html;

    # Client max body size
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

    # Backend API proxy
    location /api/ {
        proxy_pass http://backend:5000;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection 'upgrade';
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        proxy_cache_bypass $http_upgrade;
        proxy_read_timeout 86400;
    }

    # SignalR WebSocket proxy
    location /hubs/ {
        proxy_pass http://backend:5000;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        proxy_read_timeout 86400;
        proxy_send_timeout 86400;
    }

    # Frontend static files - React Router support
    location / {
        try_files $uri $uri/ /index.html;
        expires 1h;
        add_header Cache-Control "public, max-age=3600";
    }

    # Cache busting for assets
    location ~* \.(js|css|png|jpg|jpeg|gif|ico|svg|woff|woff2|ttf|eot)$ {
        expires 1y;
        add_header Cache-Control "public, max-age=31536000, immutable";
        try_files $uri =404;
    }

    # Security headers
    add_header X-Frame-Options "SAMEORIGIN" always;
    add_header X-Content-Type-Options "nosniff" always;
    add_header X-XSS-Protection "1; mode=block" always;
    add_header Referrer-Policy "strict-origin-when-cross-origin" always;
    add_header Strict-Transport-Security "max-age=31536000; includeSubDomains" always;

    # Deny access to hidden files
    location ~ /\. {
        deny all;
        access_log off;
        log_not_found off;
    }
}
EOF

    # Substitute domain in config
    sed -i "s/\${DOMAIN}/$DOMAIN/g" "$DEPLOY_DIR/nginx/default.conf"
    print_success "NGINX configuration updated"

    # Reload NGINX
    print_info "Reloading NGINX..."
    docker compose -f "$DEPLOY_DIR/docker-compose.yml" exec nginx nginx -s reload
    print_success "NGINX reloaded"

    # Setup automatic renewal
    print_info "Setting up automatic certificate renewal..."
    print_info "Certbot will automatically renew certificates before they expire"
    print_info "The certbot container is configured to check for renewals daily"

    print_header "SSL Setup Complete!"

    echo -e "${GREEN}Your NtripCaster is now secured with SSL/TLS!${NC}\n"
    echo "╔════════════════════════════════════════════════════════════╗"
    echo "║ HTTPS Access:                                              ║"
    echo "║   https://$DOMAIN"
    echo "╠════════════════════════════════════════════════════════════╣"
    echo "║ Certificate Details:                                       ║"
    echo "║   Issued by:  Let's Encrypt                                ║"
    echo "║   Valid for:  90 days (auto-renewed)                       ║"
    echo "║   Email:      $EMAIL"
    echo "╠════════════════════════════════════════════════════════════╣"
    echo "║ HTTP to HTTPS:                                             ║"
    echo "║   All HTTP traffic is now redirected to HTTPS              ║"
    echo "╚════════════════════════════════════════════════════════════╝"
}

# Run main function
main "$@"
