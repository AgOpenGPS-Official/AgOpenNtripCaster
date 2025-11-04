# 🗺️ AgOpen Ntripcaster

**A modern, high-performance NTRIP (Networked Transport of RTCM via Internet Protocol) server for real-time RTK GNSS correction distribution.**

Built with ASP.NET 9 Core, React, TypeScript, and PostgreSQL. Designed for precision agriculture and professional surveying applications.

![Status](https://img.shields.io/badge/Status-Production%20Ready-brightgreen)
![License](https://img.shields.io/badge/License-MIT-blue)
![Version](https://img.shields.io/badge/Version-1.0.0-blue)

---

## What is AgOpen Ntripcaster?

AgOpen Ntripcaster is a comprehensive real-time correction streaming server that:

- **Receives** RTCM correction data from GNSS base stations
- **Distributes** corrections to multiple RTK clients in real-time
- **Manages** users, groups, mount points, and permissions
- **Visualizes** client locations and stream status on interactive maps
- **Monitors** system health with real-time dashboards

### Key Features

- ✅ **Low-latency streaming** (5-105ms total system latency)
- ✅ **High concurrency** (1000+ concurrent clients)
- ✅ **Real-time visualization** with WebSocket-based client tracking
- ✅ **Role-based access control** with multi-user/group permissions
- ✅ **Email notifications** for source online/offline events
- ✅ **Production-ready Docker** deployment with automated migrations
- ✅ **Modern admin dashboard** with dark mode support
- ✅ **Comprehensive monitoring** and logging

---

## Tech Stack

### Backend
- ASP.NET 9 Core • C# 13 • PostgreSQL 16 • Entity Framework Core 9
- SignalR (real-time WebSocket) • JWT Authentication • Serilog Logging
- Swagger/OpenAPI Documentation

### Frontend
- React 18 • TypeScript • Vite • Leaflet.js (mapping)
- Axios (HTTP) • React Router • CSS Modules + Design System

### Infrastructure
- Docker & Docker Compose • Nginx • PostgreSQL
- GitHub Actions (CI/CD ready)

---

## Quick Start

### 📦 Development Setup

```bash
# Clone
git clone https://github.com/AgOpenGPS-Official/ntripcasterByBKG.git
cd ntripcasterByBKG

# Backend (new terminal)
cd AgOpenNtripCaster.Server
dotnet restore && dotnet ef database update && dotnet run

# Frontend (new terminal)
cd AgOpenNtripCaster.Client
npm install && npm run dev
```

**Access:**
- Frontend: http://localhost:5173
- API: http://localhost:5000
- API Docs: http://localhost:5000/swagger
- NTRIP: tcp://localhost:2101

### 🚀 Production Deployment

```bash
cd deploy
cp .env.example .env
nano .env  # Configure

chmod +x deploy.sh
./deploy.sh
```

See [docs/DEPLOYMENT.md](./docs/DEPLOYMENT.md) for complete instructions.

---

## 📚 Documentation

All detailed documentation is in the `/docs` folder:

| Document | Purpose |
|----------|---------|
| [DEPLOYMENT.md](./docs/DEPLOYMENT.md) | Production setup, Docker, scaling, monitoring |
| [LOCAL_DEV.md](./docs/LOCAL_DEV.md) | Development setup, debugging, testing |
| [ARCHITECTURE_PLAN_ASPNET9.md](./docs/ARCHITECTURE_PLAN_ASPNET9.md) | System design, database schema, authentication |
| [DARK_MODE.md](./docs/DARK_MODE.md) | CSS variables, theme system, styling guidelines |
| [UI_ENHANCEMENTS.md](./docs/UI_ENHANCEMENTS.md) | Real-time status indicators, component design |
| [PROGRESS.md](./docs/PROGRESS.md) | Development history, completed phases, roadmap |

---

## NTRIP Protocol

### Source Connection (GNSS Base Station)

```
1. Connect TCP to hostname:2101
2. Send: "SOURCE STATION_A:sourcePassword123\r\n"
3. Stream RTCM correction data continuously
```

### Client Connection (RTK Rover)

```
1. Connect TCP to hostname:2101
2. Send NTRIP HTTP request with credentials
3. Receive RTCM correction stream
4. Send position updates every ~10 seconds
```

### Sourcetable Request

```
GET http://hostname:2101/ HTTP/1.1
Returns HTML table of available mount points
```

---

## Project Structure

```
ntripcasterByBKG/
├── AgOpenNtripCaster.Server/    # C# ASP.NET 9 backend
│   ├── Controllers/              # REST API endpoints
│   ├── Services/                 # Business logic & NTRIP server
│   ├── Models/                   # Entity & DTO classes
│   ├── Data/                     # Database context & migrations
│   └── Hubs/                     # SignalR real-time communication
├── AgOpenNtripCaster.Client/    # React + TypeScript frontend
│   ├── src/components/           # React components & pages
│   ├── src/services/             # API clients & hooks
│   ├── src/styles/               # Global CSS design system
│   └── public/                   # Static assets
├── deploy/                       # Docker & production setup
│   ├── docker-compose.yml        # Service orchestration
│   ├── deploy.sh                 # Deployment script
│   ├── update.sh                 # Update script
│   └── README.md                 # Deployment docs
├── docs/                         # Detailed documentation
└── NtripCaster.sln              # Visual Studio solution
```

---

## Security

- ✅ **Two-Tier Authentication** (mount point credentials + source password)
- ✅ **JWT Token Authentication** (stateless API access)
- ✅ **Role-Based Access Control (RBAC)** (Admin, User, Source roles)
- ✅ **Password Hashing** (BCrypt with salting)
- ✅ **CORS Protection** (configurable origins)
- ✅ **HTTPS/TLS Ready** (SSL termination via Nginx)

---

## Performance

| Metric | Value |
|--------|-------|
| System Latency | 1-5ms (excluding network) |
| Network Latency | 1-100ms (typical ISP) |
| Total Latency | 5-105ms (RTK acceptable) |
| Concurrent Clients | 1000+ supported |
| Data Throughput | 100+ Mbps per instance |

---

## License

MIT License - See [LICENSE](./LICENSE) file for details.

You are free to use, modify, and distribute commercially.

---

## Support

- 📖 **Documentation**: Check the `/docs` folder
- 🐛 **Issues**: [GitHub Issues](https://github.com/AgOpenGPS-Official/ntripcasterByBKG/issues)
- 💬 **Discussions**: [GitHub Discussions](https://github.com/AgOpenGPS-Official/ntripcasterByBKG/discussions)

---

## 🚀 Quick Commands

```bash
# Development
cd AgOpenNtripCaster.Server && dotnet run
cd AgOpenNtripCaster.Client && npm run dev

# Production
cd deploy && ./deploy.sh

# Update
cd deploy && ./update.sh

# Logs
docker-compose logs -f backend
```

---

**Ready to stream RTK corrections?**

Start with [LOCAL_DEV.md](./docs/LOCAL_DEV.md) for development or [DEPLOYMENT.md](./docs/DEPLOYMENT.md) for production.

**Questions?** Check the `/docs` folder or open an issue.

---

*Built for AgOpen GPS precision agriculture ecosystem*
*Last Updated: November 2024 | Status: Production Ready ✨*
