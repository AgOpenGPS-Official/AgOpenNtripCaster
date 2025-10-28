# Local Development Setup

Deze guide helpt je bij lokaal development met Visual Studio.

## 📋 Prerequisites

- **Visual Studio 2022** (Community Edition OK)
- **.NET 9.0 SDK** (installed automatically with VS)
- **Node.js 20+** (for React frontend)
- **PostgreSQL 15** (for database)
- **Git**

---

## 🚀 Quick Start (3 Steps)

### 1. Open Solution in Visual Studio

```bash
# Navigate to project folder
cd C:\Users\hp\Documents\GitHub\ntripcaster

# Open the solution
start NtripCaster.sln
```

Visual Studio opent met beide projecten:
- ✅ **NtripCaster.Server** (C# ASP.NET 9 - .csproj)
- ✅ **NtripCaster.Client** (React/Node - .esproj)

### 2. Setup PostgreSQL Database

**Option A: Docker (Recommended)**
```bash
cd C:\Users\hp\Documents\GitHub\ntripcaster
docker-compose up -d db
```

**Option B: Local PostgreSQL**
```bash
# Create database
createdb ntripcaster

# Create user
createuser ntripuser
psql -c "ALTER USER ntripuser WITH PASSWORD 'ntrippass123';"
psql -c "GRANT ALL PRIVILEGES ON DATABASE ntripcaster TO ntripuser;"
```

### 3. Configure Environment

Create `.env` file in `NtripCaster.Server`:

```bash
# Database
CONNECTION_STRING=Host=localhost;Port=5432;Database=ntripcaster;Username=ntripuser;Password=ntrippass123

# JWT (min 32 characters)
JWT_SECRET=your-super-secret-key-that-is-at-least-32-characters-long
JWT_REFRESH_SECRET=your-super-secret-refresh-key-at-least-32-chars

# CORS
CORS_ORIGIN=http://localhost:3000

# Logging
Logging__LogLevel__Default=Debug
Logging__LogLevel__Microsoft=Warning
```

---

## 🛠️ Building & Running

### Build Both Projects

**In Visual Studio:**
- **Ctrl+Shift+B** - Build both Backend + Frontend
- **F5** - Debug both projects together

### Backend (ASP.NET 9)

**In Visual Studio:**
1. Press **F5** to run with debugger (or right-click project → Debug)
2. API opens at: `http://localhost:5000`
3. Swagger/OpenAPI: `http://localhost:5000/openapi`

**From Command Line:**
```bash
cd NtripCaster.Server
dotnet run
```

### Frontend (React + Vite)

**In Visual Studio:**
- Right-click **NtripCaster.Client** → "Execute"
- Or use the startup selector to choose frontend

**From Command Line:**
```bash
cd NtripCaster.Client
npm install        # First time only
npm run dev
```

Frontend opens at: `http://localhost:5173`

### Run Both Together

**Option 1: Visual Studio (Recommended)**
- Press **F5** - Debugs both backend and frontend
- Press **Ctrl+Shift+B** - Builds both projects

**Option 2: Docker Compose (Production-like testing)**
```bash
docker-compose up
# Backend: http://localhost:5000
# Frontend: http://localhost:3000
# Database: localhost:5432
```

---

## 📁 Project Layout in VS

```
NtripCaster (Solution)
├── NtripCaster.Server/          [C# Project - Buildable]
│   ├── Properties/
│   ├── Models/
│   ├── Controllers/
│   ├── Services/
│   ├── Hubs/
│   ├── Data/
│   ├── Program.cs               [Startup config]
│   ├── appsettings.json
│   ├── NtripCaster.Server.csproj
│   └── Dockerfile
│
└── NtripCaster.Client/          [Node/React - Folder reference]
    ├── src/
    ├── public/
    ├── package.json
    ├── vite.config.ts
    └── Dockerfile
```

---

## 🔧 Useful VS Shortcuts

| Action | Shortcut |
|--------|----------|
| Build Solution | `Ctrl+Shift+B` |
| Run (Debug) | `F5` |
| Run (No Debug) | `Ctrl+F5` |
| Open Package Manager Console | `Ctrl+Alt+O` |
| NuGet Package Manager | `Ctrl+Alt+L` |
| Clean Solution | `Ctrl+Alt+Del` then "Clean Solution" |

---

## 🐛 Debugging

### Debug Backend

1. Set breakpoints in C# code (click left margin)
2. Press **F5** to run with debugger
3. Breakpoints will pause execution
4. Use Debug toolbar to step through code

### Debug Frontend

1. Open DevTools: **F12** (in browser)
2. Go to **Sources** tab
3. Set breakpoints in React code
4. Refresh page to hit breakpoints

---

## 🗄️ Database Management

### View/Manage Database

**Option 1: pgAdmin (Docker)**
```bash
# Access at http://localhost:5050
# User: pgadmin4@pgadmin.org
# Password: admin
```

**Option 2: Visual Studio SQL Server Object Explorer**
- View → SQL Server Object Explorer
- Add Connection → PostgreSQL (requires extension)

**Option 3: Command Line**
```bash
psql -h localhost -U ntripuser -d ntripcaster

# List tables
\dt

# View schema
\d client_sessions

# Exit
\q
```

### Reset Database

**Option 1: Drop and recreate**
```bash
dropdb ntripcaster
createdb ntripcaster
# Run backend - migrations auto-run
```

**Option 2: Via Docker**
```bash
docker-compose down -v    # Remove volume
docker-compose up -d db   # Recreate
```

---

## 📝 Common Tasks

### Add NuGet Package

```bash
# Via CLI in NtripCaster.Server folder
dotnet add package PackageName

# Or in VS: Tools → NuGet Package Manager → Package Manager Console
Install-Package PackageName
```

### Add NPM Package

```bash
cd NtripCaster.Client
npm install package-name
```

### Run Database Migrations

```bash
cd NtripCaster.Server
dotnet ef database update
```

### Clear Build Cache

```bash
# Clean solution in VS, or:
cd NtripCaster.Server
rm -r bin obj
dotnet clean
```

---

## 🚨 Troubleshooting

### "Connection refused" - Database

```bash
# Check PostgreSQL is running
docker ps | grep postgres

# If not, start it
docker-compose up -d db
```

### "Port 5000 already in use"

```bash
# Find process using port 5000
netstat -ano | findstr :5000

# Kill process
taskkill /PID <PID> /F

# Or change port in launchSettings.json
```

### "Port 3000 already in use"

```bash
# Kill process on 3000
netstat -ano | findstr :3000
taskkill /PID <PID> /F
```

### React "npm not found"

```bash
# Install Node.js from https://nodejs.org
node --version    # Verify installation
npm --version
```

### ASP.NET build fails

```bash
# Restore dependencies
cd NtripCaster.Server
dotnet restore

# Clean and rebuild
dotnet clean
dotnet build
```

---

## 🔗 Local URLs

| Service | URL |
|---------|-----|
| **Frontend** | `http://localhost:3000` |
| **Backend API** | `http://localhost:5000` |
| **API Swagger/OpenAPI** | `http://localhost:5000/openapi` |
| **Health Check** | `http://localhost:5000/health` |
| **SignalR Hub** | `ws://localhost:5000/api/ntrip-hub` |
| **Database** | `localhost:5432` |
| **pgAdmin** | `http://localhost:5050` |

---

## 💡 Pro Tips

1. **Use VS Code for React editing**
   - Better TypeScript support
   - Great React extensions

2. **Keep terminal open**
   - Monitor logs in real-time
   - Easy to restart if needed

3. **Use Docker for database**
   - No installation headaches
   - Easy reset (just `docker-compose down -v`)

4. **Hot reload frontend**
   - Vite automatically reloads on file save
   - React Fast Refresh for components

5. **Postman for API testing**
   - Test REST endpoints before frontend
   - Export/import collections

---

## 📚 References

- **ASP.NET 9 Docs:** https://learn.microsoft.com/en-us/dotnet/core/whats-new/dotnet-9
- **React Docs:** https://react.dev
- **Vite Guide:** https://vitejs.dev
- **PostgreSQL:** https://www.postgresql.org/docs/
- **Docker:** https://docs.docker.com/

---

## 🎯 Next Steps

1. ✅ Open **NtripCaster.sln** in Visual Studio
2. ✅ Setup PostgreSQL (Docker or local)
3. ✅ Create **.env** file with credentials
4. ✅ Run backend: **F5** in VS
5. ✅ Run frontend: `npm run dev` in terminal
6. ✅ Access `http://localhost:3000`

**Enjoy developing!** 🚀
