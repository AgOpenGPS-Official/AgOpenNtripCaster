# NtripCaster Deployment

This directory contains everything needed to deploy NtripCaster using Docker Compose.

## Quick Start

### 1. Prerequisites

- Docker (version 20.10+)
- Docker Compose (version 1.29+)
- Bash shell
- Git (for updates)

### 2. Configuration

1. Copy the environment template:
```bash
cp .env.example .env
```

2. Edit `.env` and set your values:
```bash
nano .env
```

**Important variables to configure:**
- `DB_PASSWORD` - Strong database password
- `JWT_SECRET_KEY` - Random 32+ character string (generate with: `openssl rand -base64 32`)
- `CORS_ALLOWED_ORIGINS` - Your domain(s)
- `SMTP_*` - Email configuration
- `INITIAL_ADMIN_EMAIL` - Admin email
- `INITIAL_ADMIN_PASSWORD` - Initial admin password (change on first login!)

### 3. Deploy

Make scripts executable:
```bash
chmod +x deploy.sh update.sh
```

Run deployment:
```bash
./deploy.sh
```

The script will:
- Build Docker images
- Start services (PostgreSQL, Backend, Frontend)
- Run database migrations
- Create initial admin account
- Display access credentials

### 4. Access

After deployment:
- **Frontend**: http://localhost
- **Backend API**: http://localhost:5000
- **API Documentation**: http://localhost:5000/swagger
- **NTRIP Server**: localhost:2101

**Admin Credentials** will be displayed and saved to `admin-credentials.txt`

## File Structure

```
deploy/
├── .env.example                # Environment template (git-tracked)
├── .env                        # Local configuration (ignored)
├── .gitignore                  # Deployment-specific ignores
├── docker-compose.yml          # Service orchestration
├── Dockerfile.backend          # Backend image definition
├── Dockerfile.frontend         # Frontend image definition
├── nginx.conf                  # Nginx configuration
├── deploy.sh                   # Initial deployment script
├── update.sh                   # Update script
├── README.md                   # This file
└── database/
    └── migrations/             # Database init scripts
```

## Common Tasks

### View Logs

```bash
# All services
docker-compose logs -f

# Specific service
docker-compose logs -f backend
docker-compose logs -f postgres
docker-compose logs -f frontend
```

### Check Service Status

```bash
docker-compose ps
```

### Stop Services

```bash
docker-compose down
```

### Restart Services

```bash
docker-compose restart
```

### Update Application

After pulling latest code:

```bash
./update.sh
```

Options:
```bash
./update.sh --skip-migrations      # Skip database migrations
./update.sh --skip-restart         # Don't restart after update
```

### Access Database Directly

```bash
docker-compose exec postgres psql -U ntripcaster -d ntripcaster
```

### Run Database Migrations

```bash
docker-compose exec backend dotnet ef database update
```

### Create New Migration

From root directory:
```bash
cd AgOpenNtripCaster.Server
dotnet ef migrations add YourMigrationName -o Migrations
```

### Reset Database (DESTRUCTIVE)

```bash
docker-compose down -v
docker-compose up -d
```

## Production Considerations

### Security

1. **Change all default passwords** before deployment
2. **Use strong JWT secret** (at least 32 characters)
3. **Configure HTTPS** (uncomment SSL section in nginx.conf)
4. **Set CORS_ALLOWED_ORIGINS** to your actual domain
5. **Secure .env file** with proper permissions
6. **Remove or secure admin-credentials.txt** after use

### SSL/HTTPS

To enable HTTPS:

1. Obtain SSL certificate (Let's Encrypt recommended)
2. Uncomment SSL section in `nginx.conf`
3. Configure certificate paths
4. Update `docker-compose.yml` to mount certificate volume

Example with Let's Encrypt:
```bash
# Install certbot
sudo apt-get install certbot python3-certbot-nginx

# Get certificate
sudo certbot certonly --standalone -d your.domain.com

# Update nginx.conf with certificate paths
# Restart services
docker-compose restart frontend
```

### Backups

**Database backups:**
```bash
docker-compose exec -T postgres pg_dump -U ntripcaster ntripcaster > backup-$(date +%Y%m%d-%H%M%S).sql
```

**Restore from backup:**
```bash
cat backup-20240101-120000.sql | docker-compose exec -T postgres psql -U ntripcaster -d ntripcaster
```

### Monitoring

Use Docker's built-in monitoring:
```bash
docker stats
```

Or configure external monitoring tools.

### Resource Limits

Edit `docker-compose.yml` to add resource limits:

```yaml
services:
  backend:
    deploy:
      resources:
        limits:
          cpus: '1'
          memory: 1G
        reservations:
          cpus: '0.5'
          memory: 512M
```

## Troubleshooting

### Services won't start

Check logs:
```bash
docker-compose logs backend
docker-compose logs postgres
```

Common issues:
- Port already in use (change ports in docker-compose.yml)
- Database not ready (wait longer)
- Invalid environment variables (verify .env file)

### Database migration fails

```bash
# Check migrations status
docker-compose exec backend dotnet ef migrations list

# View recent logs
docker-compose logs postgres -f
```

### Frontend shows errors

```bash
# Check frontend logs
docker-compose logs frontend

# Verify backend is accessible
curl http://localhost:5000/api/health
```

### High disk usage

The PostgreSQL volume can grow large. To clean up:
```bash
docker system prune -a
docker volume prune
```

## Advanced Configuration

### Custom domain

Edit `nginx.conf`:
```nginx
server_name your.domain.com;
```

### Multiple environments

Create separate env files:
```bash
cp .env.example .env.staging
cp .env.example .env.production
```

Run with specific env:
```bash
docker-compose --env-file .env.staging up
```

### Enable debug logging

In .env:
```
LOG_LEVEL=Debug
```

## Support

For issues:
1. Check logs: `docker-compose logs -f`
2. Review documentation in `/docs` folder
3. Check GitHub issues: https://github.com/your-repo/issues

## License

See LICENSE in root directory.
