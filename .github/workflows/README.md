# GitHub Actions Workflows

This directory contains automated CI/CD workflows for the AgOpenNtripCaster project.

## Workflows

### 1. Build and Test (`build-and-test.yml`)

**Triggers:**
- Push to `develop` or `main` branches
- Pull requests to `develop` or `main` branches

**What it does:**
- ✅ Builds the .NET backend
- ✅ Builds the React/TypeScript frontend
- ✅ Runs tests (if available)
- ✅ Builds Docker images (without pushing)
- ✅ Runs TypeScript type checking
- ✅ Runs ESLint code quality checks

**Purpose:** Ensures all code changes compile and pass quality checks before merging.

---

### 2. Release (`release.yml`)

**Triggers:**
- Push to `main` branch only

**What it does:**
1. 📦 **Automatic Version Bumping:**
   - Fetches the latest git tag (e.g., `v1.2.3`)
   - Increments the patch version (e.g., `v1.2.3` → `v1.2.4`)
   - Creates a new git tag

2. 📝 **Release Notes Generation:**
   - Generates changelog from commits since last release
   - Creates a GitHub Release with formatted notes
   - Includes Docker pull instructions

3. 🐋 **Docker Image Publishing:**
   - Builds and pushes Docker images to GitHub Container Registry (ghcr.io)
   - Tags images with both version tag and `latest`
   - Images available at:
     - `ghcr.io/[owner]/[repo]/server:v1.2.4`
     - `ghcr.io/[owner]/[repo]/server:latest`
     - `ghcr.io/[owner]/[repo]/client:v1.2.4`
     - `ghcr.io/[owner]/[repo]/client:latest`

**Purpose:** Automates the release process when changes are merged to production.

---

## Version Numbering

The project uses **Semantic Versioning** (SemVer): `vMAJOR.MINOR.PATCH`

- **MAJOR:** Breaking changes (manual bump required)
- **MINOR:** New features, backward compatible (manual bump required)
- **PATCH:** Bug fixes, automated increment ✅

### Current Behavior:
- Every push to `main` automatically increments the **PATCH** version
- Example: `v1.0.5` → `v1.0.6` → `v1.0.7`

### Manual Version Bumps:
If you need to bump MAJOR or MINOR versions:

```bash
# Bump to new minor version (e.g., v1.0.7 → v1.1.0)
git tag v1.1.0
git push origin v1.1.0

# Bump to new major version (e.g., v1.1.0 → v2.0.0)
git tag v2.0.0
git push origin v2.0.0
```

The next automatic release will then increment from your manual tag.

---

## First Release

If no tags exist yet, the workflow will:
1. Start at `v0.0.0`
2. Create the first release as `v0.0.1`

To set a custom initial version:

```bash
git tag v1.0.0
git push origin v1.0.0
```

---

## Docker Images

After each release, Docker images are automatically published to GitHub Container Registry.

### Pull Images:

```bash
# Specific version
docker pull ghcr.io/[owner]/[repo]/server:v1.2.4
docker pull ghcr.io/[owner]/[repo]/client:v1.2.4

# Latest version
docker pull ghcr.io/[owner]/[repo]/server:latest
docker pull ghcr.io/[owner]/[repo]/client:latest
```

### Update docker-compose.yml:

```yaml
services:
  ntripcaster-server:
    image: ghcr.io/[owner]/[repo]/server:latest
    # ... rest of config

  ntripcaster-client:
    image: ghcr.io/[owner]/[repo]/client:latest
    # ... rest of config
```

---

## Permissions

The workflows require these permissions (already configured):
- ✅ `contents: write` - Create releases and tags
- ✅ `packages: write` - Push Docker images to ghcr.io

No additional secrets needed - uses `GITHUB_TOKEN` automatically.

---

## Workflow Status

Check workflow status:
- GitHub repository → **Actions** tab
- View logs for each workflow run
- See build/test results
- Download artifacts (if any)

---

## Development Workflow

### Recommended Git Flow:

1. **Feature Development:**
   ```bash
   git checkout develop
   git checkout -b feature/my-new-feature
   # ... make changes ...
   git commit -m "Add new feature"
   git push origin feature/my-new-feature
   ```

2. **Create Pull Request:**
   - Open PR from `feature/my-new-feature` → `develop`
   - GitHub Actions runs build-and-test workflow
   - Review and merge when checks pass

3. **Merge to Main (Release):**
   ```bash
   git checkout main
   git merge develop
   git push origin main
   ```
   - GitHub Actions automatically creates release
   - New version tag created
   - Docker images published
   - Release notes generated

---

## Troubleshooting

### Build Fails
- Check the Actions tab for detailed error logs
- Common issues:
  - Missing dependencies in package.json
  - TypeScript errors
  - Docker build context issues

### Release Not Created
- Ensure you're pushing to `main` branch (not `master` or `develop`)
- Check that previous tag exists and is valid SemVer format
- Verify `GITHUB_TOKEN` has correct permissions

### Docker Images Not Published
- Check GitHub Container Registry permissions
- Ensure Dockerfile paths are correct
- Verify Docker build context includes all necessary files

---

## Future Enhancements

Consider adding:
- 🧪 Automated integration tests
- 📊 Code coverage reports
- 🔒 Security scanning (Dependabot, Snyk)
- 🚀 Automated deployment to staging environment
- 📧 Notifications on release (Slack, email)
