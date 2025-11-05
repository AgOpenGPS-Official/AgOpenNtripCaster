# Docker Compose Guide

This guide explains the different Docker Compose files and how to use them for various deployment scenarios.

---

## Files Overview

### `docker-compose.yml` (Default)
**Best for:** Local development and standard Docker CLI deployments

**How it works:**
- Builds images from source code
- Uses relative paths to find Dockerfiles and source code
- Requires the entire repository to be present
- Run from the `deploy` folder: `docker-compose up -d`

**Requirements:**
- Clone the full repository
- Have `AgOpenNtripCaster.Server` and `AgOpenNtripCaster.Client` folders in parent directory
- Docker and Docker Compose installed

**Usage:**
```bash
cd deploy
cp .env.example .env
nano .env  # Configure your environment
docker-compose up -d
```

---

### `docker-compose.portainer.yml` (Portainer)
**Best for:** Portainer deployments with automatic builds from GitHub

**How it works:**
- Builds Docker images directly from GitHub repository
- Portainer clones the repo and builds automatically
- Includes build context for both backend and frontend
- Simpler than manual builds - one-click deployment

**Requirements:**
- Docker and Portainer installed
- Portainer must have internet access to reach GitHub
- `.env` file with your configuration

**Usage in Portainer (Recommended Method):**
1. Create a new stack
2. Click "Repository" tab (not "Editor")
3. Fill in:
   - **Repository URL:** `https://github.com/AgOpenGPS-Official/AgOpenNtripCaster.git`
   - **Compose path:** `deploy/docker-compose.portainer.yml`
   - **Branch:** `develop` (or your preferred branch)
4. Add environment variables or upload `.env` file
5. Click "Deploy the stack"

**Portainer will:**
- Clone the repository
- Build images from source
- Start the containers
- All automatically!

---

## Environment Configuration

Both compose files use the same `.env` file for configuration.

### Creating .env File

**Option 1: Using Setup Script (Recommended)**
```bash
cd deploy
chmod +x setup.sh
./setup.sh
```

**Option 2: Manual Copy**
```bash
cd deploy
cp .env.example .env
nano .env  # Edit with your values
```

### Required Variables

**Minimum** (must be filled in):
```
DB_NAME=ntripcaster_db
DB_USER=ntripcaster
DB_PASSWORD=your_secure_password
JWT_SECRET=min_32_chars_random_string
ADMIN_PASSWORD=your_admin_password
SMTP_ENABLED=false
```

**Recommended** (for full features):
```
SERVER_DOMAIN=ntripcaster.example.com
SMTP_ENABLED=true
SMTP_HOST=smtp.gmail.com
SMTP_USERNAME=your-email@gmail.com
SMTP_PASSWORD=your_app_password
```

---

## Deployment Scenarios

### Scenario 1: Local Development
**Use:** `docker-compose.yml`

```bash
cd deploy
cp .env.example .env
nano .env  # Set minimal variables

docker-compose up -d
```

**Access:**
- Frontend: http://localhost:8080
- API: http://localhost:5000
- NTRIP: localhost:2101

---

### Scenario 2: Portainer on Server (GitHub Auto-Build)
**Use:** `docker-compose.portainer.yml`

**Steps:**
1. Open Portainer UI (usually http://server-ip:9000)
2. Go to: Stacks → Add stack
3. Click "Repository" tab
4. Enter:
   - Repository URL: `https://github.com/AgOpenGPS-Official/AgOpenNtripCaster.git`
   - Compose path: `deploy/docker-compose.portainer.yml`
   - Branch: `develop`
5. Scroll down to "Environment"
6. Add environment variables or create `.env` file:
   ```
   DB_NAME=ntripcaster_db
   DB_USER=ntripcaster
   DB_PASSWORD=your_secure_password
   JWT_SECRET=your_jwt_secret_32_chars_min
   ADMIN_PASSWORD=your_admin_password
   SMTP_ENABLED=false
   ```
7. Click "Deploy the stack"

**Portainer will:**
- Clone from GitHub automatically
- Build backend and frontend images
- Start all services
- Show deployment status

**Access:**
- Frontend: http://server-ip:8080
- API: http://server-ip:5000
- NTRIP: server-ip:2101

---

### Scenario 3: Docker Compose on Server (No Portainer)
**Use:** `docker-compose.yml`

```bash
# On server
git clone https://github.com/AgOpenGPS-Official/AgOpenNtripCaster.git
cd AgOpenNtripCaster/deploy

./setup.sh  # or manually create .env
docker-compose up -d
```

---

### Scenario 4: Using Pre-Built Images
**Use:** `docker-compose.portainer.yml` with custom image names

Edit the compose file to use your registry:
```yaml
backend:
  image: your-registry/ntripcaster-backend:latest

frontend:
  image: your-registry/ntripcaster-frontend:latest
```

---

## Building Docker Images

### Automatic (Portainer)
Portainer builds images automatically when using `docker-compose.portainer.yml` with Repository option.

### Manual Build (Local)

**Backend**
```bash
cd AgOpenNtripCaster.Server
docker build -f ../deploy/Dockerfile.backend -t agopen/ntripcaster-backend:latest .
```

**Frontend**
```bash
cd AgOpenNtripCaster.Client
docker build -f ../deploy/Dockerfile.frontend -t agopen/ntripcaster-frontend:latest .
```

**Using Docker Compose**
```bash
cd deploy
docker-compose build
```

### Push to Registry
```bash
# Tag images
docker tag agopen/ntripcaster-backend:latest your-registry/ntripcaster-backend:latest
docker tag agopen/ntripcaster-frontend:latest your-registry/ntripcaster-frontend:latest

# Push
docker push your-registry/ntripcaster-backend:latest
docker push your-registry/ntripcaster-frontend:latest
```

---

## Common Issues & Fixes

### Issue: "no such file or directory" in Portainer
**Cause:** Relative paths don't work in Portainer
**Fix:** Use `docker-compose.portainer.yml` instead

### Issue: Build fails with "path not found"
**Cause:** Docker build context doesn't include required files
**Fix:** Ensure you're running from correct directory or use pre-built images

### Issue: Database connection failed
**Cause:** PostgreSQL not ready when backend starts
**Fix:** The compose file has `depends_on: condition: service_healthy` - ensure PostgreSQL starts first

### Issue: Environment variables empty
**Cause:** .env file not in correct location or not loaded
**Fix:**
- Ensure `.env` is in the `deploy` folder
- Check file permissions: `chmod 644 .env`
- Try explicitly loading: `docker-compose --env-file .env up -d`

### Issue: Port already in use
**Cause:** Another service using port 5000, 8080, 2101, etc.
**Fix:** Change ports in `.env`:
```
SERVER_PORT=5001
WEB_PORT=8081
NTRIP_PORT=2102
```

---

## Volumes & Persistence

The compose files create volumes for data persistence:

- `postgres_data` - PostgreSQL database files
- `ntripcaster_logs` - Application logs
- `./backups` - Database backups (host folder)
- `./certs` - SSL certificates (host folder)

### Accessing Volume Data
```bash
# View logs
docker-compose logs -f backend

# Backup database
docker-compose exec postgres pg_dump -U $DB_USER $DB_NAME > backup.sql

# View volumes
docker volume ls | grep ntripcaster
```

---

## Networks

Both compose files create an isolated network:
- Network name: `ntripcaster_network`
- Services can communicate using hostnames (e.g., `backend:5000`)
- External access through published ports

---

## Security Notes

🔒 **Before Production:**

1. **Change Default Passwords**
   - `ADMIN_PASSWORD` - Change immediately after first login
   - `DB_PASSWORD` - Use strong, unique password

2. **Enable HTTPS**
   - Set `SSL_METHOD` in environment
   - Copy certificates to `./certs/` folder
   - Update nginx configuration

3. **Enable Email**
   - Configure SMTP settings
   - Use app passwords (not account passwords) for Gmail

4. **Firewall**
   - Only expose necessary ports (80, 443 via nginx)
   - Block direct access to 5000 (API) and 2101 (NTRIP) if possible
   - Restrict database port (5432) to internal network only

5. **Backups**
   - Enable automated backups: `BACKUP_ENABLED=true`
   - Store backups securely
   - Test restore procedures regularly

---

## Advanced Usage

### Custom Network
Attach to existing network instead of creating new:
```yaml
networks:
  ntripcaster_network:
    external: true
    name: my-existing-network
```

### Custom Volumes
Use bind mounts for easier access:
```yaml
volumes:
  postgres_data: /var/lib/ntripcaster/postgres
  ntripcaster_logs: /var/log/ntripcaster
```

### Multiple Instances
Run multiple instances with different compose files:
```bash
docker-compose -f docker-compose.yml -p ntripcaster-1 up -d
docker-compose -f docker-compose.yml -p ntripcaster-2 up -d
```

---

## Support

- 📖 See [SETUP_GUIDE.md](./SETUP_GUIDE.md) for interactive setup
- 📖 See [DEPLOYMENT.md](../docs/DEPLOYMENT.md) for production guide
- 🐛 [Report issues on GitHub](https://github.com/AgOpenGPS-Official/AgOpenNtripCaster/issues)
