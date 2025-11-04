# 🚀 Production Deployment Guide

**Last Updated**: November 2024
**Status**: Production Ready
**Target**: Docker Compose on Linux/Windows Server

---

## Table of Contents

1. [Quick Start](#quick-start)
2. [System Requirements](#system-requirements)
3. [Pre-Deployment Checklist](#pre-deployment-checklist)
4. [Installation Steps](#installation-steps)
5. [Configuration](#configuration)
6. [Database Setup](#database-setup)
7. [SSL/TLS Setup](#ssltls-setup)
8. [Monitoring & Maintenance](#monitoring--maintenance)
9. [Troubleshooting](#troubleshooting)
10. [Scaling](#scaling)

---

## Quick Start

For experienced Docker users who know what they're doing:

```bash
# Clone repository
git clone https://github.com/your-org/ntripcaster.git
cd ntripcaster/deploy

# Configure
cp .env.example .env
nano .env  # Edit your settings

# Deploy
chmod +x deploy.sh update.sh
./deploy.sh

# Access
# Frontend: http://your-server
# API: http://your-server:5000
# NTRIP: your-server:2101
```

---

## System Requirements

### Hardware

| Component | Minimum | Recommended |
|-----------|---------|-------------|
| **CPU** | 2 cores | 4+ cores |
| **RAM** | 2GB | 4GB+ |
| **Storage** | 10GB | 50GB+ |
| **Network** | 1 Mbps | 10 Mbps+ |

### Software

- **Docker**: Version 20.10+ ([Install](https://docs.docker.com/get-docker/))
- **Docker Compose**: Version 1.29+ ([Install](https://docs.docker.com/compose/install/))
- **Linux Kernel**: 4.15+ (for iptables forwarding)
- **Git**: For cloning the repository

### Supported Operating Systems

- ✅ Ubuntu 20.04 LTS or later
- ✅ Debian 11 or later
- ✅ CentOS/RHEL 8 or later
- ✅ Docker Desktop on Windows/Mac (for dev/testing)
- ⚠️ Windows Server 2019+ with Docker Desktop

### Network Ports

| Port | Service | Protocol | Purpose |
|------|---------|----------|---------|
| **80** | Nginx | HTTP | Web traffic, Let's Encrypt ACME |
| **443** | Nginx | HTTPS | Secure web traffic |
| **2101** | NTRIP | TCP | GNSS correction streaming |
| **5000** | Backend | HTTP | API (internal only) |
| **5432** | PostgreSQL | TCP | Database (internal only) |

---

## Pre-Deployment Checklist

- [ ] Docker and Docker Compose installed and working
- [ ] Git installed for cloning repository
- [ ] At least 10GB free disk space
- [ ] Ports 80, 443, 2101 accessible from internet
- [ ] Domain name configured with DNS pointing to server IP
- [ ] SSL certificate plan decided (Let's Encrypt or manual)
- [ ] Environment variables prepared (.env file)
- [ ] Database backup strategy planned
- [ ] Monitoring plan decided (optional)
- [ ] Backup and restore procedures tested

---

## Installation Steps

### Step 1: Prepare the Server

```bash
# Update system
sudo apt update && sudo apt upgrade -y

# Install Docker (Ubuntu/Debian)
curl -fsSL https://get.docker.com -o get-docker.sh
sudo sh get-docker.sh

# Add current user to docker group
sudo usermod -aG docker $USER
newgrp docker  # Activate new group

# Verify installation
docker --version
docker-compose --version
```

### Step 2: Clone Repository

```bash
# Choose a location for the application
cd /opt  # or /home/username/apps

# Clone the repository
git clone https://github.com/your-org/ntripcaster.git
cd ntripcaster

# Verify structure
ls -la
# Should show: README.md, deploy/, docs/, AgOpenNtripCaster.Server/, etc.
```

### Step 3: Create Environment File

```bash
cd deploy

# Copy example environment
cp .env.example .env

# Edit with your values
nano .env
```

**See [Configuration](#configuration) section below for all options.**

### Step 4: Make Scripts Executable

```bash
chmod +x deploy.sh
chmod +x update.sh
```

### Step 5: Run Deployment

```bash
# Full deployment (builds images, starts services, runs migrations)
./deploy.sh

# Watch the logs
docker-compose logs -f

# Check services are running
docker-compose ps
```

### Step 6: Verify Installation

```bash
# Check all services are running
docker-compose ps

# Test API
curl http://localhost:5000/api/health
# Should return: {"status":"healthy"}

# View logs for issues
docker-compose logs backend
docker-compose logs frontend
docker-compose logs postgres
```

### Step 7: Access the Application

- **Frontend Dashboard**: http://your-server (or https://your-domain with SSL)
- **API Documentation**: http://your-server:5000/swagger
- **NTRIP Server**: your-server:2101

### Step 8: Initial Setup

1. **Login with default admin account**
   - Email: admin@ntripcaster.local
   - Password: AdminPassword123! (displayed in deployment logs)
   - **CHANGE THIS IMMEDIATELY**

2. **Configure System**
   - Go to Admin → Caster Configuration
   - Update server information
   - Configure email settings (optional)

3. **Create Mount Points**
   - Add GNSS base stations
   - Configure credentials
   - Set up source password

4. **Create Users**
   - Add operator accounts
   - Assign to appropriate groups
   - Configure permissions

---

## Configuration

### Environment Variables (.env file)

```bash
# === Docker Image Configuration ===
DOCKER_REGISTRY=docker.io
DOCKER_NAMESPACE=your-namespace
APP_NAME=ntripcaster

# === Frontend ===
FRONTEND_PORT=3000
VITE_API_URL=http://localhost:5000

# === Backend ===
BACKEND_PORT=5000
ASPNETCORE_ENVIRONMENT=Production

# === Database ===
DB_HOST=postgres
DB_PORT=5432
DB_NAME=ntripcaster
DB_USER=ntripcaster
DB_PASSWORD=change-this-to-secure-password
DB_BACKUP_DIR=./backups

# === JWT Authentication ===
JWT_SECRET=your-super-secret-key-min-32-characters-long-string
JWT_EXPIRATION_MINUTES=1440

# === CORS ===
CORS_ORIGINS=http://localhost:3000,http://localhost:5173

# === Email Configuration (SMTP) ===
SMTP_HOST=smtp.gmail.com
SMTP_PORT=587
SMTP_USERNAME=your-email@gmail.com
SMTP_PASSWORD=your-app-password
SMTP_FROM_EMAIL=noreply@ntripcaster.local
SMTP_FROM_NAME=AgOpen Ntripcaster
SMTP_ENABLE_TLS=true
SMTP_ENABLE_SSL=false

# === SSL/HTTPS Configuration ===
USE_SSL=false
SSL_CERT_PATH=/etc/letsencrypt/live/your-domain/fullchain.pem
SSL_KEY_PATH=/etc/letsencrypt/live/your-domain/privkey.pem

# === Nginx Configuration ===
NGINX_PORT=80
NGINX_SSL_PORT=443
SERVER_NAME=your-server.com

# === Logging ===
LOG_LEVEL=Information
LOG_FILE_PATH=./logs

# === NTRIP Server ===
NTRIP_PORT=2101
NTRIP_BACKLOG=128
```

### Configuration Best Practices

1. **Generate Secure Secrets**
   ```bash
   # Generate a secure JWT secret
   openssl rand -base64 32

   # Generate a secure database password
   openssl rand -base64 24
   ```

2. **Email Configuration**
   - Use Gmail app password (not regular password)
   - For Outlook: Use app-specific password
   - For custom SMTP: Ensure TLS/SSL settings are correct

3. **Domain Configuration**
   - Ensure DNS A record points to server IP
   - Wait for DNS propagation (up to 48 hours)
   - Test with: `nslookup your-domain.com`

4. **Keep Secrets Secure**
   - Never commit `.env` file to git
   - Store backups of `.env` in secure location
   - Rotate secrets regularly

---

## Database Setup

### Initial Setup (First Run)

The `deploy.sh` script automatically:
1. Creates PostgreSQL container
2. Initializes the database
3. Runs EF Core migrations
4. Seeds default admin account

### Backup Strategy

#### Automated Backups

Add to cron (daily at 2 AM):

```bash
# Edit crontab
crontab -e

# Add this line
0 2 * * * /opt/ntripcaster/deploy/backup.sh

# Or use a backup script
#!/bin/bash
cd /opt/ntripcaster/deploy
docker-compose exec -T postgres pg_dump -U $DB_USER $DB_NAME > backups/backup_$(date +%Y%m%d_%H%M%S).sql
```

#### Manual Backup

```bash
cd /opt/ntripcaster/deploy

# Backup database
docker-compose exec postgres pg_dump -U ntripcaster ntripcaster > backup_$(date +%Y%m%d_%H%M%S).sql

# Backup entire data directory
tar -czf ntripcaster_backup_$(date +%Y%m%d_%H%M%S).tar.gz \
  postgres-data/ \
  ./backups/ \
  .env
```

#### Restore from Backup

```bash
# Restore database
docker-compose exec -T postgres psql -U ntripcaster ntripcaster < backup_20231101_120000.sql

# Or restore entire backup
tar -xzf ntripcaster_backup_20231101_120000.tar.gz
docker-compose restart postgres
```

### Database Maintenance

```bash
# Connect to database
docker-compose exec postgres psql -U ntripcaster -d ntripcaster

# Common commands
SELECT version();  -- Check PostgreSQL version
SELECT COUNT(*) FROM users;  -- Count users
SELECT COUNT(*) FROM mount_points;  -- Count mount points
\dt  -- List tables
\q  -- Exit
```

---

## SSL/TLS Setup

### Option A: Let's Encrypt (Recommended)

```bash
# Install Certbot
sudo apt install certbot python3-certbot-nginx -y

# Generate certificate
sudo certbot certonly --standalone -d your-domain.com -d www.your-domain.com

# This creates:
# /etc/letsencrypt/live/your-domain.com/fullchain.pem
# /etc/letsencrypt/live/your-domain.com/privkey.pem

# Update .env file
SSL_CERT_PATH=/etc/letsencrypt/live/your-domain.com/fullchain.pem
SSL_KEY_PATH=/etc/letsencrypt/live/your-domain.com/privkey.pem
USE_SSL=true

# Auto-renew certificate (cron job)
sudo certbot renew --quiet
docker-compose restart nginx
```

### Option B: Manual Certificate

```bash
# Copy your certificate files to the server
scp your-cert.crt user@server:/opt/ntripcaster/certs/
scp your-key.key user@server:/opt/ntripcaster/certs/

# Update .env
SSL_CERT_PATH=/opt/ntripcaster/certs/your-cert.crt
SSL_KEY_PATH=/opt/ntripcaster/certs/your-key.key
USE_SSL=true

# Restart nginx
docker-compose restart nginx
```

### Option C: Self-Signed (Testing Only)

```bash
# Generate self-signed certificate
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365

# Move to deploy directory
mv *.pem deploy/certs/

# Update .env
USE_SSL=true
SSL_CERT_PATH=/opt/ntripcaster/certs/cert.pem
SSL_KEY_PATH=/opt/ntripcaster/certs/key.pem

# Note: Browsers will warn about untrusted certificate
```

---

## Monitoring & Maintenance

### Container Health

```bash
# Check status
docker-compose ps

# View logs
docker-compose logs -f backend     # Backend logs
docker-compose logs -f frontend    # Frontend logs
docker-compose logs -f postgres    # Database logs
docker-compose logs -f nginx       # Reverse proxy logs

# Check resource usage
docker stats

# Restart a service
docker-compose restart backend
docker-compose restart postgres
```

### System Health Check

```bash
# API health endpoint
curl http://localhost:5000/api/health

# Database connection
docker-compose exec postgres pg_isready -U ntripcaster

# Disk space
df -h

# Memory usage
free -h

# Docker cleanup (removes unused images)
docker system prune -a
```

### Log Rotation

Configure Docker log rotation in `docker-compose.yml`:

```yaml
services:
  backend:
    logging:
      driver: "json-file"
      options:
        max-size: "100m"
        max-file: "10"
```

### Performance Monitoring

```bash
# Monitor in real-time
watch -n 1 'docker stats --no-stream'

# Check slow queries (if enabled)
docker-compose exec postgres psql -U ntripcaster -d ntripcaster \
  -c "SELECT query, mean_exec_time FROM pg_stat_statements ORDER BY mean_exec_time DESC LIMIT 10;"
```

---

## Troubleshooting

### Services Won't Start

```bash
# Check Docker daemon
sudo systemctl status docker

# View error messages
docker-compose logs backend

# Start with verbose output
docker-compose --verbose up backend

# Common issues:
# - Port already in use: Change port in .env
# - Out of disk space: Clean up with 'docker system prune'
# - Permission denied: Check docker group membership
```

### Database Connection Issues

```bash
# Test PostgreSQL connection
docker-compose exec postgres psql -U ntripcaster -d ntripcaster -c "SELECT 1;"

# Check database credentials in .env
cat .env | grep DB_

# View PostgreSQL logs
docker-compose logs postgres

# Reset database (caution: deletes data)
docker-compose down
docker volume rm ntripcaster_postgres_data
docker-compose up -d postgres
./deploy.sh  # Re-run migrations
```

### Frontend Shows Blank Page

```bash
# Check browser console for errors (F12)
# Check frontend logs
docker-compose logs frontend

# Common causes:
# - Backend not running: docker-compose logs backend
# - CORS error: Check CORS_ORIGINS in .env
# - Wrong API URL: Check VITE_API_URL in .env

# Rebuild frontend
docker-compose down frontend
docker-compose up -d frontend
```

### SSL Certificate Issues

```bash
# Check certificate validity
openssl x509 -in /etc/letsencrypt/live/your-domain.com/fullchain.pem -noout -dates

# Test HTTPS connection
curl -v https://your-domain.com/

# Check Nginx configuration
docker-compose exec nginx nginx -t

# View Nginx logs
docker-compose logs nginx
```

### High Memory Usage

```bash
# Check which container is consuming memory
docker stats

# Increase limits in docker-compose.yml
services:
  backend:
    mem_limit: 1g  # Limit to 1GB

# Clear Docker cache
docker system prune -a --volumes

# Restart services
docker-compose restart
```

---

## Scaling

### Horizontal Scaling (Multiple Instances)

For production with high load, run multiple backend instances:

```yaml
# docker-compose.yml
services:
  backend:
    deploy:
      replicas: 3  # Run 3 instances
    environment:
      - ASPNETCORE_URLS=http://+:5000
```

Or use a proper orchestration tool:
- **Kubernetes**: Full production setup
- **Docker Swarm**: Built-in Docker clustering
- **AWS ECS**: Managed container service

### Load Balancing

Nginx (already configured) will distribute load across multiple backend instances.

### Database Optimization

```bash
# Create indexes for slow queries
docker-compose exec postgres psql -U ntripcaster -d ntripcaster << EOF
CREATE INDEX idx_client_sessions_user_id ON client_sessions(user_id);
CREATE INDEX idx_client_sessions_disconnected ON client_sessions(disconnected_at);
CREATE INDEX idx_source_connections_mount_point ON source_connections(mount_point_id);
EOF

# Analyze query performance
EXPLAIN ANALYZE SELECT * FROM client_sessions WHERE user_id = '...' LIMIT 10;
```

### Caching Layer

Consider adding Redis for:
- Session caching
- Rate limiting
- Real-time data caching

```yaml
services:
  redis:
    image: redis:7-alpine
    ports:
      - "6379:6379"
    volumes:
      - redis_data:/data

volumes:
  redis_data:
```

---

## Disaster Recovery

### Complete Backup

```bash
#!/bin/bash
BACKUP_DIR="/backup/ntripcaster"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)

mkdir -p $BACKUP_DIR

# Backup database
docker-compose exec -T postgres pg_dump -U ntripcaster ntripcaster | \
  gzip > $BACKUP_DIR/db_backup_$TIMESTAMP.sql.gz

# Backup application data
tar -czf $BACKUP_DIR/app_data_$TIMESTAMP.tar.gz \
  postgres-data/ \
  .env \
  logs/

# Keep only last 30 days
find $BACKUP_DIR -name "*.gz" -mtime +30 -delete

echo "Backup completed: $BACKUP_DIR"
```

### Complete Restore

```bash
#!/bin/bash
BACKUP_FILE="$1"  # Pass backup file as argument

if [ ! -f "$BACKUP_FILE" ]; then
  echo "Backup file not found: $BACKUP_FILE"
  exit 1
fi

# Shutdown services
docker-compose down

# Restore data
tar -xzf $BACKUP_FILE

# Restore database
gunzip -c db_backup_*.sql.gz | docker-compose exec -T postgres \
  psql -U ntripcaster -d ntripcaster

# Restart services
docker-compose up -d

echo "Restore completed"
```

---

## Monitoring Solutions

### Option 1: Docker Stats (Built-in)

```bash
docker stats --no-stream
```

### Option 2: Prometheus + Grafana

```yaml
services:
  prometheus:
    image: prom/prometheus
    volumes:
      - ./prometheus.yml:/etc/prometheus/prometheus.yml
    ports:
      - "9090:9090"

  grafana:
    image: grafana/grafana
    ports:
      - "3000:3000"
    volumes:
      - grafana_data:/var/lib/grafana
```

### Option 3: ELK Stack (Elasticsearch, Logstash, Kibana)

For centralized logging and analysis.

---

## Security Hardening

### 1. Firewall Configuration

```bash
# Enable UFW
sudo ufw enable

# Allow only necessary ports
sudo ufw allow 22/tcp   # SSH
sudo ufw allow 80/tcp   # HTTP
sudo ufw allow 443/tcp  # HTTPS
sudo ufw allow 2101/tcp # NTRIP

# Verify rules
sudo ufw status
```

### 2. SSH Security

```bash
# Disable password login
sudo nano /etc/ssh/sshd_config
# Set: PasswordAuthentication no
# Set: PermitRootLogin no

# Restart SSH
sudo systemctl restart sshd
```

### 3. Regular Updates

```bash
# Enable automatic updates
sudo apt install unattended-upgrades
sudo systemctl enable unattended-upgrades
```

### 4. Secrets Management

```bash
# Use Docker secrets (Swarm) or external secret managers
# Never hardcode secrets in docker-compose.yml

# Use .env file (add to .gitignore)
# Or use docker secret for production:
echo "my-secret" | docker secret create db_password -
```

---

## Related Documentation

- [README.md](../README.md) - Project overview
- [LOCAL_DEV.md](./LOCAL_DEV.md) - Development setup
- [ARCHITECTURE_PLAN_ASPNET9.md](./ARCHITECTURE_PLAN_ASPNET9.md) - System design
- [DARK_MODE.md](./DARK_MODE.md) - UI theme system

---

## Support & Troubleshooting

- **Issues**: [GitHub Issues](https://github.com/your-org/ntripcaster/issues)
- **Discussions**: [GitHub Discussions](https://github.com/your-org/ntripcaster/discussions)
- **Docker Docs**: [docker.com/docs](https://docs.docker.com/)
- **Docker Compose Docs**: [docs.docker.com/compose](https://docs.docker.com/compose/)

---

**Last Updated**: November 2024
**Status**: Production Ready ✨

For the latest updates, check the repository's [deploy/README.md](../deploy/README.md).
