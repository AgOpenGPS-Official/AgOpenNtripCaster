# Deployment Guide

This directory contains all deployment-related files for AgOpenNtripCaster.

## Files Overview

### Docker Compose Files

- **`docker-compose.portainer.yml`** - Production deployment via Portainer (✅ Recommended)
- **`docker-compose.yml`** - Local development deployment

### Dockerfiles

- **`Dockerfile.backend`** - ASP.NET Core backend (NTRIP server + API)
- **`Dockerfile.frontend`** - React frontend build
- **`Dockerfile.nginx`** - Nginx web server with frontend

### Configuration

- **`.env.example`** - Environment variables template
- **`nginx/`** - Nginx configuration files
- **`certbot/`** - SSL certificate configuration

## Deployment Methods

### 1. Portainer (Recommended for Production)

#### Prerequisites
- Portainer installed and running
- GitHub repository access

#### Steps

1. **In Portainer:**
   - Go to **Stacks** → **Add Stack**
   - Choose **Repository** tab

2. **Configure Repository:**
   - Repository URL: `https://github.com/AgOpenGPS-Official/AgOpenNtripCaster.git`
   - Reference: `develop` (or `main` for stable releases)
   - Compose path: `deploy/docker-compose.portainer.yml`

3. **Environment Variables:**

   Either upload a `.env` file or add these variables manually:

   ```env
   # Database
   DB_NAME=ntripcaster_db
   DB_USER=ntripcaster
   DB_PASSWORD=your_secure_password_here
   DB_PORT=5432

   # Backend
   JWT_SECRET=your_jwt_secret_min_32_chars
   JWT_EXPIRATION_HOURS=24
   CORS_ORIGINS=http://your-domain.com:8080

   # SMTP (Optional)
   SMTP_HOST=smtp.gmail.com
   SMTP_PORT=587
   SMTP_USERNAME=your-email@gmail.com
   SMTP_PASSWORD=your-app-password
   SMTP_FROM_EMAIL=noreply@your-domain.com
   SMTP_FROM_NAME=AgOpen Ntripcaster
   SMTP_ENABLED=false

   # Ports
   WEB_PORT=8080
   WEB_PORT_SSL=4433
   SERVER_PORT=5000
   NTRIP_PORT=2101
   DB_PORT_EXTERNAL=5432

   # Logging
   LOG_LEVEL=Information
   ASPNETCORE_ENVIRONMENT=Production
   ```

4. **Deploy:**
   - Click **Deploy the stack**
   - Wait for Portainer to pull and build images
   - Check logs for any errors

5. **Access:**
   - Frontend: `http://your-server-ip:8080`
   - Backend API: `http://your-server-ip:5000`
   - NTRIP Server: `your-server-ip:2101`

#### Updating via Portainer

When you push changes to GitHub:

1. Go to your stack in Portainer
2. Click **Update the stack**
3. Enable **Re-pull image and redeploy**
4. Click **Update**

Portainer will automatically pull the latest code and rebuild.

---

### 2. Docker Compose (Local Development)

#### Prerequisites
- Docker and Docker Compose installed
- Git repository cloned locally

#### Steps

1. **Navigate to deploy directory:**
   ```bash
   cd ntripcaster/deploy
   ```

2. **Copy environment file:**
   ```bash
   cp .env.example .env
   nano .env  # Edit with your values
   ```

3. **Build and start:**
   ```bash
   docker-compose -f docker-compose.yml up -d --build
   ```

4. **View logs:**
   ```bash
   docker-compose -f docker-compose.yml logs -f
   ```

5. **Stop:**
   ```bash
   docker-compose -f docker-compose.yml down
   ```

#### Development Workflow

```bash
# Rebuild after code changes
docker-compose -f docker-compose.yml up -d --build

# Restart specific service
docker-compose -f docker-compose.yml restart backend

# View service logs
docker-compose -f docker-compose.yml logs -f backend
```

---

### 3. Using Pre-built Docker Images (GitHub Container Registry)

After each release, images are published to GitHub Container Registry.

#### Pull Latest Images

```bash
# Pull specific version
docker pull ghcr.io/agopengs-official/ntripcaster/backend:v1.0.0
docker pull ghcr.io/agopengs-official/ntripcaster/frontend:v1.0.0
docker pull ghcr.io/agopengs-official/ntripcaster/nginx:v1.0.0

# Or pull latest
docker pull ghcr.io/agopengs-official/ntripcaster/backend:latest
docker pull ghcr.io/agopengs-official/ntripcaster/frontend:latest
docker pull ghcr.io/agopengs-official/ntripcaster/nginx:latest
```

#### Update docker-compose.yml

Replace `build:` sections with `image:`:

```yaml
services:
  backend:
    image: ghcr.io/agopengs-official/ntripcaster/backend:latest
    # Remove the build section
    # ...rest of config

  frontend:
    image: ghcr.io/agopengs-official/ntripcaster/frontend:latest
    # ...rest of config

  nginx:
    image: ghcr.io/agopengs-official/ntripcaster/nginx:latest
    # ...rest of config
```

---

## Services Architecture

```
┌─────────────────────────────────────────┐
│           Nginx (Port 8080/443)         │
│  - Serves React frontend                │
│  - Reverse proxy to backend             │
│  - SSL/TLS termination (optional)       │
└──────────────┬──────────────────────────┘
               │
    ┌──────────┴──────────┐
    │                     │
┌───▼────────┐   ┌────────▼───────┐
│  Frontend  │   │    Backend     │
│  (React)   │   │  (ASP.NET 9)   │
│            │   │                │
│  - Build   │   │  - API: 5000   │
│    only    │   │  - NTRIP: 2101 │
└────────────┘   └────────┬───────┘
                          │
                  ┌───────▼────────┐
                  │   PostgreSQL   │
                  │   (Port 5432)  │
                  └────────────────┘
```

## Volumes

Persistent data is stored in Docker volumes:

- **`postgres_data`** - Database files
- **`frontend-dist`** - Built React app
- **`certbot-etc`** - SSL certificates (Let's Encrypt)
- **`certbot-var`** - Certbot runtime data
- **`certbot-webroot`** - Certbot challenge files

Backup these volumes for disaster recovery.

---

## Health Checks

All services include health checks:

### Backend
```bash
curl http://localhost:5000/api/health
```

### Nginx
```bash
curl http://localhost:8080/
```

### PostgreSQL
```bash
docker exec ntripcaster-postgres pg_isready -U ntripcaster
```

---

## SSL/TLS Configuration (Optional)

### Let's Encrypt with Certbot

1. **Initial certificate request:**
   ```bash
   docker-compose -f docker-compose.portainer.yml run --rm certbot certonly \
     --webroot \
     --webroot-path=/var/www/certbot \
     --email your-email@example.com \
     --agree-tos \
     --no-eff-email \
     -d your-domain.com
   ```

2. **Update nginx configuration:**
   - Edit `nginx/nginx.conf`
   - Uncomment SSL server block
   - Add certificate paths

3. **Auto-renewal:**
   - Uncomment certbot service entrypoint in docker-compose
   - Certificates renew automatically every 12 hours

---

## Troubleshooting

### Backend won't start
```bash
# Check logs
docker-compose -f docker-compose.portainer.yml logs backend

# Common issues:
# - Database not ready → Wait for PostgreSQL health check
# - Missing JWT_SECRET → Set in .env
# - Port conflicts → Change ports in .env
```

### Database connection errors
```bash
# Test database connectivity
docker exec ntripcaster-backend curl postgres:5432

# Reset database (⚠️ DESTROYS DATA)
docker-compose -f docker-compose.portainer.yml down -v
docker-compose -f docker-compose.portainer.yml up -d
```

### Frontend build fails
```bash
# Rebuild with verbose output
docker-compose -f docker-compose.portainer.yml build --no-cache frontend

# Common issues:
# - npm peer dependency conflicts → Uses --legacy-peer-deps flag
# - Out of memory → Increase Docker memory limit
```

### Nginx 502 Bad Gateway
```bash
# Check backend is running
docker ps | grep backend

# Check backend logs
docker logs ntripcaster-backend

# Restart nginx
docker-compose -f docker-compose.portainer.yml restart nginx
```

---

## Performance Tuning

### Database
```yaml
# In docker-compose.yml, add to postgres service:
command:
  - "postgres"
  - "-c"
  - "max_connections=200"
  - "-c"
  - "shared_buffers=256MB"
```

### Backend
```env
# In .env file:
LOG_LEVEL=Warning  # Reduce log verbosity in production
ASPNETCORE_ENVIRONMENT=Production
```

### Nginx
```nginx
# In nginx/nginx.conf:
worker_processes auto;
worker_connections 2048;
```

---

## Security Best Practices

1. **Change default passwords** in `.env`
2. **Use strong JWT_SECRET** (min 32 characters)
3. **Enable SSL/TLS** for production
4. **Restrict CORS origins** to your domain
5. **Regular backups** of postgres_data volume
6. **Keep Docker images updated** (weekly)
7. **Monitor logs** for suspicious activity
8. **Use firewall** to restrict port access

---

## Backup and Restore

### Backup Database
```bash
docker exec ntripcaster-postgres pg_dump -U ntripcaster ntripcaster_db > backup_$(date +%Y%m%d).sql
```

### Restore Database
```bash
cat backup_20241115.sql | docker exec -i ntripcaster-postgres psql -U ntripcaster -d ntripcaster_db
```

### Backup Volumes
```bash
docker run --rm -v postgres_data:/data -v $(pwd):/backup alpine tar czf /backup/postgres_data_backup.tar.gz /data
```

---

## Support

- **GitHub Issues**: https://github.com/AgOpenGPS-Official/AgOpenNtripCaster/issues
- **Documentation**: See README.md in repository root
- **Workflow Status**: Check GitHub Actions tab for build/release status
