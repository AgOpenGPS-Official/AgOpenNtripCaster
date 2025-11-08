# Docker Architecture - AgOpen Ntripcaster

This document describes the Docker container architecture for AgOpen Ntripcaster.

## Container Architecture

The application uses a **multi-container architecture** with separate containers for each service:

```
┌─────────────────────────────────────────────────────────────────┐
│                     Docker Compose Stack                         │
├─────────────────────────────────────────────────────────────────┤
│  1. PostgreSQL (postgres:16-alpine)                              │
│     - Database storage                                           │
│     - Port: 5432 (configurable)                                  │
│     - Health checks enabled                                      │
├─────────────────────────────────────────────────────────────────┤
│  2. Backend API (ASP.NET Core 9.0)                               │
│     - REST API endpoints                                         │
│     - SignalR WebSocket hub                                      │
│     - NTRIP Server (port 2101)                                   │
│     - Health endpoint: /api/health                               │
│     - Ports: 5000 (API), 2101 (NTRIP)                            │
├─────────────────────────────────────────────────────────────────┤
│  3. Frontend Build (Node 20 Alpine)                              │
│     - Builds React + Vite application                            │
│     - Runs once, copies dist to shared volume                    │
│     - No persistent container                                    │
├─────────────────────────────────────────────────────────────────┤
│  4. NGINX (nginx:stable-alpine)                  ← ENTRY POINT   │
│     - Serves static frontend files                               │
│     - Reverse proxy to backend API (/api/*)                      │
│     - Reverse proxy to SignalR (/hubs/*)                         │
│     - Ports: 80 (HTTP), 443 (HTTPS)                              │
│     - External: 8080 (HTTP), 4433 (HTTPS)                        │
├─────────────────────────────────────────────────────────────────┤
│  5. Certbot (certbot/certbot) - OPTIONAL                         │
│     - Let's Encrypt SSL certificate management                   │
│     - Automatic renewal (when enabled)                           │
│     - Shares certificates with NGINX                             │
└─────────────────────────────────────────────────────────────────┘
```

## Why Separate NGINX Container?

This architecture follows **separation of concerns** and provides several benefits:

### 1. **Build vs Runtime Separation**
   - Frontend build happens in an ephemeral container
   - NGINX only serves pre-built static files
   - No Node.js runtime overhead in production

### 2. **Easier SSL/TLS Management**
   - NGINX handles all SSL termination
   - Certbot can manage certificates independently
   - Backend doesn't need SSL configuration

### 3. **Better Resource Efficiency**
   - Frontend build container runs once and exits
   - NGINX is lightweight (~10MB base image)
   - Backend focuses only on API logic

### 4. **Simplified Reverse Proxy**
   - Single entry point for all traffic
   - NGINX handles routing to backend services
   - WebSocket support for SignalR

### 5. **Production Best Practices**
   - Static file caching
   - Gzip compression
   - Security headers
   - Rate limiting capabilities

## Container Startup Sequence

```
1. PostgreSQL starts
   └─> Health check passes

2. Backend starts (waits for PostgreSQL health)
   └─> Runs migrations
   └─> Seeds database
   └─> Starts NTRIP server
   └─> Health check passes (/api/health)

3. Frontend Build runs
   └─> npm install
   └─> npm run build
   └─> Copies dist/ to shared volume
   └─> Container exits

4. NGINX starts (waits for frontend + backend)
   └─> Mounts frontend-dist volume
   └─> Configures reverse proxy
   └─> Starts serving traffic
   └─> Health check passes

5. Certbot (optional)
   └─> Requests/renews SSL certificates
   └─> Shares with NGINX via volume
```

## Volume Architecture

### Named Volumes
- `postgres_data` - PostgreSQL database files
- `frontend-dist` - Compiled React application
- `certbot-etc` - Let's Encrypt certificates
- `certbot-var` - Certbot state data

### Bind Mounts
- `./deploy/nginx/default.conf` → NGINX configuration
- `./deploy/logs` → Backend application logs
- `./deploy/backups` → Database backups
- `./deploy/certbot/webroot` → ACME challenge files

## Network Architecture

All containers communicate via a **bridge network** (`ntripcaster_network`):

```
External Traffic (port 8080)
    ↓
┌─────────────────────┐
│  NGINX Container    │
│  (Port 80)          │
└─────────────────────┘
    ↓ Reverse Proxy
┌─────────────────────┐      ┌─────────────────────┐
│  Backend Container  │◄────►│ PostgreSQL Container│
│  (Port 5000, 2101)  │      │  (Port 5432)        │
└─────────────────────┘      └─────────────────────┘
```

## Health Checks

Each service has health checks to ensure proper startup sequencing:

| Service | Health Check | Interval | Timeout | Retries |
|---------|--------------|----------|---------|---------|
| PostgreSQL | `pg_isready` | 10s | 5s | 5 |
| Backend | `GET /api/health` | 30s | 10s | 3 |
| NGINX | `wget http://localhost/` | 30s | 10s | 3 |

## Environment Variables

All configuration is done via environment variables:

### Database
- `DB_HOST` - PostgreSQL hostname (default: postgres)
- `DB_PORT` - PostgreSQL port (default: 5432)
- `DB_NAME` - Database name (default: ntripcaster_db)
- `DB_USER` - Database user (default: ntripcaster)
- `DB_PASSWORD` - Database password
- `DB_PORT_EXTERNAL` - External port mapping (default: 5432)

### Backend
- `JWT_SECRET` - JWT signing key (required)
- `JWT_EXPIRATION_HOURS` - Token expiration (default: 24)
- `CORS_ORIGINS` - Allowed CORS origins
- `SMTP_*` - Email configuration
- `NTRIP_PORT` - NTRIP server port (default: 2101)

### NGINX
- `WEB_PORT` - External HTTP port (default: 8080)
- `WEB_PORT_SSL` - External HTTPS port (default: 4433)

## SSL/TLS Setup

### Option 1: Let's Encrypt (Production)

1. Ensure ports 80 and 443 are accessible
2. Set environment variables:
   ```bash
   DOMAIN=your.domain.com
   EMAIL=admin@your.domain.com
   ```

3. Run certbot:
   ```bash
   docker-compose run --rm certbot certonly \
     --webroot \
     --webroot-path=/var/www/certbot \
     -d your.domain.com \
     --email admin@your.domain.com \
     --agree-tos \
     --non-interactive
   ```

4. Uncomment SSL block in `deploy/nginx/default.conf`
5. Restart NGINX: `docker-compose restart nginx`

### Option 2: Self-Signed (Development)

```bash
openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
  -keyout deploy/certs/privkey.pem \
  -out deploy/certs/fullchain.pem \
  -subj "/CN=localhost"
```

## Deployment

### Local Development
```bash
docker-compose up -d
```

### Production (Portainer)
1. Delete existing stack
2. Add new stack from repository:
   - Repository: `https://github.com/AgOpenGPS-Official/AgOpenNtripCaster.git`
   - Compose path: `docker-compose.portainer.yml`
   - Branch: `develop`
3. Configure environment variables
4. Deploy

## Troubleshooting

### Frontend not loading
- Check if frontend build completed: `docker logs ntripcaster-frontend-build`
- Verify NGINX has mounted frontend-dist volume: `docker exec ntripcaster-nginx ls /usr/share/nginx/html`

### Backend API not responding
- Check backend health: `curl http://localhost:5000/api/health`
- Check database connection: `docker logs ntripcaster-backend`

### NGINX 502 Bad Gateway
- Backend might not be healthy yet (check health checks)
- Verify backend is reachable: `docker exec ntripcaster-nginx wget -O- http://backend:5000/api/health`

### SSL certificate issues
- Check certbot logs: `docker logs ntripcaster-certbot`
- Verify domain DNS points to server
- Ensure ports 80/443 are open in firewall

## Monitoring

View logs for each service:
```bash
# All services
docker-compose logs -f

# Specific service
docker-compose logs -f nginx
docker-compose logs -f backend
docker-compose logs -f postgres

# Frontend build logs
docker logs ntripcaster-frontend-build
```

## Maintenance

### Update Application
```bash
git pull origin develop
docker-compose down
docker-compose build --no-cache
docker-compose up -d
```

### Database Backup
```bash
docker exec ntripcaster-postgres pg_dump \
  -U ntripcaster ntripcaster_db > backup.sql
```

### Database Restore
```bash
docker exec -i ntripcaster-postgres psql \
  -U ntripcaster ntripcaster_db < backup.sql
```

### Renew SSL Certificates
```bash
docker-compose run --rm certbot renew
docker-compose restart nginx
```
