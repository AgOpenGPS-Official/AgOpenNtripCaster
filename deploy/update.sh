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
#   --no-cache          Force rebuild without Docker cache (slow!)
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
NO_CACHE=false
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." && pwd )"
DEPLOY_DIR="$SCRIPT_DIR"

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
        --no-cache)
            NO_CACHE=true
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
    if ! docker compose -f "$DEPLOY_DIR/docker-compose.yml" ps --status running | grep -q "ntripcaster-backend"; then
        print_error "Docker services are not running. Please start them first."
        print_info "Current status:"
        docker compose -f "$DEPLOY_DIR/docker-compose.yml" ps
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
    if [ "$NO_CACHE" = true ]; then
        print_warning "Using --no-cache (this will be slow!)"
        docker compose -f "$DEPLOY_DIR/docker-compose.yml" build --no-cache
    else
        docker compose -f "$DEPLOY_DIR/docker-compose.yml" build
    fi
    print_success "Docker images rebuilt"

    # Run migrations if not skipped
    if [ "$SKIP_MIGRATIONS" = false ]; then
        print_info "Running database migrations..."
        docker compose -f "$DEPLOY_DIR/docker-compose.yml" exec -T backend dotnet ef database update || {
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
        docker compose -f "$DEPLOY_DIR/docker-compose.yml" up -d
        print_success "Services restarted"

        # Wait for services to be ready
        print_info "Waiting for services to be ready..."
        sleep 10

        # Get WEB_PORT from .env or use default
        WEB_PORT=$(grep "^WEB_PORT=" "$DEPLOY_DIR/.env" 2>/dev/null | cut -d'=' -f2 || echo "8080")

        if curl -s "http://localhost:${WEB_PORT}/api/health" &> /dev/null; then
            print_success "Services are healthy"
        else
            print_warning "Services may not be fully ready yet. Check logs with: docker compose -f $DEPLOY_DIR/docker-compose.yml logs -f"
        fi

        # Cleanup old images and build cache
        print_info "Cleaning up old Docker images and build cache..."

        # Remove dangling images (untagged images from previous builds)
        DANGLING_IMAGES=$(docker images -f "dangling=true" -q | wc -l)
        if [ "$DANGLING_IMAGES" -gt 0 ]; then
            docker image prune -f > /dev/null 2>&1
            print_success "Removed $DANGLING_IMAGES dangling image(s)"
        else
            print_info "No dangling images to clean"
        fi

        # Remove build cache older than 24 hours
        CACHE_SIZE_BEFORE=$(docker system df --format "{{.BuildCache}}" 2>/dev/null || echo "0B")
        docker builder prune -f --filter "until=24h" > /dev/null 2>&1
        CACHE_SIZE_AFTER=$(docker system df --format "{{.BuildCache}}" 2>/dev/null || echo "0B")
        print_success "Build cache cleaned (was: $CACHE_SIZE_BEFORE, now: $CACHE_SIZE_AFTER)"
    else
        print_warning "Skipping service restart"
    fi

    print_header "Update Complete!"

    echo -e "${GREEN}NtripCaster has been successfully updated!${NC}\n"

    echo "╔════════════════════════════════════════════════════════════╗"
    echo "║ USEFUL COMMANDS                                            ║"
    echo "╠════════════════════════════════════════════════════════════╣"
    echo "║ View logs:      docker compose -f deploy/docker-compose.yml logs -f"
    echo "║ Check status:   docker compose -f deploy/docker-compose.yml ps"
    echo "║ Stop services:  docker compose -f deploy/docker-compose.yml down"
    echo "║ Restart:        docker compose -f deploy/docker-compose.yml restart"
    echo "╚════════════════════════════════════════════════════════════╝"
}

# Run main function
main "$@"
