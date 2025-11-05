# SSL/TLS Certificates

This directory stores SSL/TLS certificates for HTTPS support.

## Setup Options

### Option 1: Let's Encrypt (Recommended for production)
Place your Let's Encrypt certificates here:
- `fullchain.pem` - Complete certificate chain
- `privkey.pem` - Private key

Run the setup script to configure automatically:
```bash
./deploy/setup.sh
./deploy/setup.ps1  # For Windows
```

### Option 2: Self-signed certificates (Development)
Generate self-signed certificates:
```bash
openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
  -keyout deploy/certs/privkey.pem \
  -out deploy/certs/fullchain.pem \
  -subj "/CN=localhost"
```

### Option 3: Manual certificate placement
1. Copy your fullchain.pem and privkey.pem to this directory
2. Update nginx.conf with your domain name
3. Update docker-compose environment variables

## Nginx Configuration

Certificates are referenced in `deploy/nginx.conf` (uncomment SSL block to enable).

Default SSL configuration expects:
- `/etc/nginx/certs/fullchain.pem`
- `/etc/nginx/certs/privkey.pem`
