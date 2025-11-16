#!/bin/bash

################################################################################
# SSL Certificate Renewal Script for NtripCaster
#
# This script automatically renews Let's Encrypt SSL certificates and reloads
# NGINX if renewal was successful.
#
# Usage: ./renew-ssl.sh
# Cron:  0 3 * * * /path/to/renew-ssl.sh >> /var/log/certbot-renew.log 2>&1
################################################################################

set -e

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"
DEPLOY_DIR="$SCRIPT_DIR"

# Log with timestamp
log() {
    echo "[$(date +'%Y-%m-%d %H:%M:%S')] $1"
}

log "=== Starting SSL Certificate Renewal Check ==="

# Change to deploy directory
cd "$DEPLOY_DIR"

# Check if docker compose is available
if ! command -v docker &> /dev/null; then
    log "ERROR: Docker is not installed or not in PATH"
    exit 1
fi

# Try to renew certificates
# Certbot will only renew if certificates are due for renewal (< 30 days)
log "Checking certificates for renewal..."

if docker compose run --rm certbot renew --quiet; then
    log "✓ Certificate renewal check completed"

    # Reload NGINX to pick up renewed certificates
    log "Reloading NGINX configuration..."
    if docker compose exec -T nginx nginx -s reload; then
        log "✓ NGINX reloaded successfully"
    else
        log "⚠ WARNING: Failed to reload NGINX"
        exit 1
    fi
else
    log "⚠ Certificate renewal failed or no renewal needed"
    exit 0
fi

log "=== SSL Renewal Process Complete ==="
