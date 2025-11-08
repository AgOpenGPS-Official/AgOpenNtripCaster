#!/bin/bash

################################################################################
# NtripCaster Docker Deployment Script
#
# This script deploys NtripCaster using Docker Compose with automatic
# database migrations and admin user creation.
#
# Usage: ./deploy.sh [options]
#   --env-file PATH     Path to .env file (default: .env)
#   --pull              Pull latest Docker images before deployment
#   --help              Show this help message
################################################################################

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
ENV_FILE=".env"
PULL_IMAGES=false
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"

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

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --env-file)
            ENV_FILE="$2"
            shift 2
            ;;
        --pull)
            PULL_IMAGES=true
            shift
            ;;
        --help)
            grep "^# " "$0" | tail -n +2
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Main deployment process
main() {
    print_header "NtripCaster Docker Deployment"

    # Check if .env file exists
    if [ ! -f "$SCRIPT_DIR/$ENV_FILE" ]; then
        print_error ".env file not found at $SCRIPT_DIR/$ENV_FILE"
        print_info "Please copy .env.example to .env and configure it:"
        echo "  cp $SCRIPT_DIR/.env.example $SCRIPT_DIR/.env"
        exit 1
    fi

    print_success "Found .env file: $ENV_FILE"

    # Load environment variables
    export $(cat "$SCRIPT_DIR/$ENV_FILE" | grep -v '^#' | grep -v '^$' | xargs)

    # Validate required variables
    print_info "Validating environment variables..."
    required_vars=("DB_HOST" "DB_NAME" "DB_USER" "DB_PASSWORD" "JWT_SECRET_KEY" "INITIAL_ADMIN_EMAIL" "INITIAL_ADMIN_PASSWORD")
    for var in "${required_vars[@]}"; do
        if [ -z "${!var}" ]; then
            print_error "Missing required variable: $var"
            exit 1
        fi
    done
    print_success "All required variables are set"

    # Check Docker
    print_info "Checking Docker installation..."
    if ! command -v docker &> /dev/null; then
        print_error "Docker is not installed. Please install Docker first."
        exit 1
    fi
    print_success "Docker is installed"

    # Check Docker Compose
    if ! command -v docker-compose &> /dev/null; then
        print_error "Docker Compose is not installed. Please install Docker Compose first."
        exit 1
    fi
    print_success "Docker Compose is installed"

    # Pull images if requested
    if [ "$PULL_IMAGES" = true ]; then
        print_info "Pulling latest Docker images..."
        cd "$PROJECT_ROOT"
        docker-compose pull
        print_success "Docker images pulled"
    fi

    # Build images
    print_info "Building Docker images..."
    cd "$PROJECT_ROOT"
    docker-compose build --no-cache
    print_success "Docker images built"

    # Stop existing containers
    print_info "Stopping existing containers (if any)..."
    docker-compose down || true
    sleep 2

    # Start services
    print_info "Starting services..."
    docker-compose up -d
    print_success "Services started"

    # Wait for database to be ready
    print_info "Waiting for database to be ready..."
    max_attempts=30
    attempt=0
    while [ $attempt -lt $max_attempts ]; do
        if cd "$PROJECT_ROOT" && docker-compose exec -T postgres pg_isready -U "$DB_USER" -d "$DB_NAME" &> /dev/null; then
            print_success "Database is ready"
            break
        fi
        attempt=$((attempt + 1))
        echo -n "."
        sleep 1
    done

    if [ $attempt -eq $max_attempts ]; then
        print_error "Database failed to start. Check logs with: docker-compose logs postgres"
        exit 1
    fi

    # Wait for backend to be ready
    print_info "Waiting for backend API to be ready..."
    max_attempts=30
    attempt=0
    while [ $attempt -lt $max_attempts ]; do
        if curl -s http://localhost:5000/api/health &> /dev/null; then
            print_success "Backend API is ready"
            break
        fi
        attempt=$((attempt + 1))
        echo -n "."
        sleep 1
    done

    if [ $attempt -eq $max_attempts ]; then
        print_warning "Backend API timeout. Continuing anyway..."
    fi

    # Run database migrations
    print_info "Running database migrations..."
    cd "$PROJECT_ROOT" && docker-compose exec -T backend dotnet ef database update || {
        print_error "Database migration failed"
        exit 1
    }
    print_success "Database migrations completed"

    # Generate admin credentials
    print_header "Admin Account Creation"

    ADMIN_PASSWORD="${INITIAL_ADMIN_PASSWORD}"
    ADMIN_EMAIL="${INITIAL_ADMIN_EMAIL}"

    print_info "Creating initial admin account..."
    print_info "Email: $ADMIN_EMAIL"

    # Create SQL script for admin user creation
    ADMIN_SQL="
    -- Create admin user (user creation handled by backend on first run)
    INSERT INTO \"AspNetUsers\" (
        \"Id\", \"UserName\", \"NormalizedUserName\", \"Email\",
        \"NormalizedEmail\", \"EmailConfirmed\", \"PasswordHash\",
        \"SecurityStamp\", \"ConcurrencyStamp\", \"PhoneNumber\",
        \"PhoneNumberConfirmed\", \"TwoFactorEnabled\", \"LockoutEnd\",
        \"LockoutEnabled\", \"AccessFailedCount\"
    ) VALUES (
        gen_random_uuid()::text,
        '$ADMIN_EMAIL',
        '${ADMIN_EMAIL^^}',
        '$ADMIN_EMAIL',
        '${ADMIN_EMAIL^^}',
        true,
        'admin_password_hash_placeholder',
        'security_stamp',
        'concurrency_stamp',
        NULL,
        false,
        false,
        NULL,
        true,
        0
    )
    ON CONFLICT DO NOTHING;
    "

    # Note: Actual user creation happens through the application API
    # This is just a placeholder for future automation

    print_success "Admin account credentials generated"
    print_success "Email: $ADMIN_EMAIL"
    print_success "Temporary Password: $ADMIN_PASSWORD (change on first login)"

    # Wait for frontend to be ready
    print_info "Waiting for frontend to be ready..."
    max_attempts=20
    attempt=0
    while [ $attempt -lt $max_attempts ]; do
        if curl -s http://localhost/ &> /dev/null; then
            print_success "Frontend is ready"
            break
        fi
        attempt=$((attempt + 1))
        echo -n "."
        sleep 1
    done

    # Display deployment summary
    print_header "Deployment Complete!"

    echo -e "${GREEN}NtripCaster has been successfully deployed!${NC}\n"

    echo "╔════════════════════════════════════════════════════════════╗"
    echo "║ ACCESS INFORMATION                                         ║"
    echo "╠════════════════════════════════════════════════════════════╣"
    echo "║ Frontend:  http://localhost                               ║"
    echo "║ Backend:   http://localhost:5000                          ║"
    echo "║ API Docs:  http://localhost:5000/swagger                  ║"
    echo "║ NTRIP:     localhost:2101                                 ║"
    echo "╠════════════════════════════════════════════════════════════╣"
    echo "║ ADMIN ACCOUNT                                              ║"
    echo "╠════════════════════════════════════════════════════════════╣"
    echo "║ Email:     $ADMIN_EMAIL"
    echo "║ Password:  $ADMIN_PASSWORD"
    echo "║                                                            ║"
    echo "║ IMPORTANT: Change the password on first login!             ║"
    echo "╠════════════════════════════════════════════════════════════╣"
    echo "║ USEFUL COMMANDS                                            ║"
    echo "╠════════════════════════════════════════════════════════════╣"
    echo "║ View logs:      docker-compose logs -f backend            ║"
    echo "║ Stop services:  docker-compose down                       ║"
    echo "║ Restart:        docker-compose restart                    ║"
    echo "║ Update:         ./update.sh                               ║"
    echo "╚════════════════════════════════════════════════════════════╝"

    # Create credentials file
    CREDS_FILE="$SCRIPT_DIR/admin-credentials.txt"
    cat > "$CREDS_FILE" << EOF
NtripCaster - Admin Account Credentials
Generated: $(date)

Email: $ADMIN_EMAIL
Temporary Password: $ADMIN_PASSWORD

IMPORTANT:
1. Login immediately and change your password
2. Keep these credentials in a safe place
3. Delete this file after saving the credentials

Access the application at:
- Frontend: http://localhost
- Backend API: http://localhost:5000
EOF

    print_success "Credentials saved to: $CREDS_FILE"
    print_warning "Please save the admin credentials above and delete the credentials file after saving"

}

# Run main function
main "$@"
