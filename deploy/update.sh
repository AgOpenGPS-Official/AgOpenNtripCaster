#!/bin/bash

################################################################################
# NtripCaster Update Script
#
# This script updates an existing NtripCaster deployment with the latest
# code, runs migrations, and restarts services.
#
# Usage: ./update.sh [options]
#   --skip-migrations   Skip database migrations
#   --skip-restart      Don't restart services after update
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
SKIP_MIGRATIONS=false
SKIP_RESTART=false
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
        --skip-migrations)
            SKIP_MIGRATIONS=true
            shift
            ;;
        --skip-restart)
            SKIP_RESTART=true
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

# Main update process
main() {
    print_header "NtripCaster Update"

    # Check if docker-compose is running
    print_info "Checking Docker services..."
    if ! docker-compose -f "$SCRIPT_DIR/docker-compose.yml" ps | grep -q "running"; then
        print_error "Docker services are not running. Please run deploy.sh first."
        exit 1
    fi
    print_success "Docker services are running"

    # Pull latest code
    print_info "Pulling latest code from repository..."
    cd "$PROJECT_ROOT"
    git pull origin develop
    print_success "Code updated"

    # Rebuild images
    print_info "Rebuilding Docker images..."
    cd "$SCRIPT_DIR"
    docker-compose build --no-cache
    print_success "Docker images rebuilt"

    # Run migrations if not skipped
    if [ "$SKIP_MIGRATIONS" = false ]; then
        print_info "Running database migrations..."
        docker-compose exec -T backend dotnet ef database update || {
            print_error "Database migration failed"
            exit 1
        }
        print_success "Database migrations completed"
    else
        print_warning "Skipping database migrations"
    fi

    # Restart services if not skipped
    if [ "$SKIP_RESTART" = false ]; then
        print_info "Restarting services..."
        docker-compose up -d
        print_success "Services restarted"

        # Wait for services to be ready
        print_info "Waiting for services to be ready..."
        sleep 5

        if curl -s http://localhost/api/health &> /dev/null; then
            print_success "Services are healthy"
        else
            print_warning "Services may not be fully ready yet. Check logs with: docker-compose logs -f"
        fi
    else
        print_warning "Skipping service restart"
    fi

    print_header "Update Complete!"

    echo -e "${GREEN}NtripCaster has been successfully updated!${NC}\n"

    echo "╔════════════════════════════════════════════════════════════╗"
    echo "║ USEFUL COMMANDS                                            ║"
    echo "╠════════════════════════════════════════════════════════════╣"
    echo "║ View logs:      docker-compose logs -f                    ║"
    echo "║ Check status:   docker-compose ps                         ║"
    echo "║ Stop services:  docker-compose down                       ║"
    echo "║ Restart:        docker-compose restart                    ║"
    echo "╚════════════════════════════════════════════════════════════╝"
}

# Run main function
main "$@"
