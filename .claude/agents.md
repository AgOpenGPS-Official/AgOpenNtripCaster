# AgOpenNtripCaster Feature Development Workflow

## Core Principle: Complete Feature Implementation

When implementing ANY new feature, functionality, or change in AgOpenNtripCaster, it MUST be completed fully across ALL relevant layers before moving to the next feature. This ensures:

1. ✅ **Completeness** - No half-finished features
2. ✅ **Testability** - Each feature can be tested end-to-end immediately
3. ✅ **Traceability** - Clear audit trail of what was implemented
4. ✅ **Quality** - Proper verification at each step

---

## The 4-Layer Development Stack

Every feature touches one or more of these layers:

```
┌─────────────────────────────────────┐
│  1. USER INTERFACE (Frontend/UI)   │ ← What users see and interact with
├─────────────────────────────────────┤
│  2. BACKEND API (ASP.NET Core)     │ ← Business logic, authentication, validation
├─────────────────────────────────────┤
│  3. CASTER LOGIC (NTRIP Protocol)  │ ← Core NTRIP streaming, client/source management
├─────────────────────────────────────┤
│  4. DATABASE (Entity Framework)     │ ← Data persistence and migrations
└─────────────────────────────────────┘
```

---

## Feature Implementation Workflow

### Step-by-Step Process

For EACH new feature, follow these steps **IN ORDER** and **COMPLETELY**:

#### Phase 1: Analysis & Planning
1. **Define the feature scope**
   - What problem does it solve?
   - Who are the users?
   - What are the success criteria?

2. **Identify affected layers**
   ```
   □ Database changes needed?
   □ Backend API endpoints needed?
   □ Caster logic changes needed?
   □ UI/Frontend changes needed?
   ```

3. **Create feature checklist** (see template below)

---

#### Phase 2: Database Layer (if applicable)

**When needed:** Feature requires new data to be stored or existing schema changes

**Steps:**
1. **Design data model**
   - Create/modify entity classes in `AgOpenNtripCaster.Server/Models/Entities/`
   - Define relationships (foreign keys, navigation properties)
   - Add validation attributes

2. **Update DbContext**
   - Add `DbSet<T>` properties to `ApplicationDbContext.cs`
   - Configure entity relationships in `OnModelCreating()`

3. **Create migration**
   ```bash
   cd AgOpenNtripCaster.Server
   dotnet ef migrations add AddFeatureNameMigration
   ```

4. **Review migration**
   - Check generated SQL in `Migrations/` folder
   - Verify no unintended changes
   - Add data seeding if needed

5. **Apply migration**
   ```bash
   dotnet ef database update
   ```

6. **Verify**
   - Check database schema
   - Test insert/update/delete operations
   - Verify indexes and constraints

**Files typically modified:**
- `Models/Entities/*.cs`
- `Data/ApplicationDbContext.cs`
- `Migrations/*.cs`

---

#### Phase 3: Backend API Layer

**When needed:** Feature needs server-side logic or API endpoints

**Steps:**
1. **Create/update DTOs**
   - Request DTOs in `Models/DTOs/`
   - Response DTOs with proper documentation
   - Add validation attributes (`[Required]`, `[Range]`, etc.)

2. **Implement service layer**
   - Create service interface in `Services/*/I*Service.cs`
   - Implement service in `Services/*/*.cs`
   - Add business logic, validation, error handling
   - Inject dependencies (DbContext, other services)

3. **Create controller endpoints**
   - Add controller in `Controllers/`
   - Define HTTP methods (GET, POST, PUT, DELETE)
   - Add authorization attributes (`[Authorize]`, `[Authorize(Roles = "Admin")]`)
   - Document endpoints with XML comments and `[ProducesResponseType]`

4. **Register services**
   - Add to DI container in `Program.cs`
   ```csharp
   builder.Services.AddScoped<IFeatureService, FeatureService>();
   ```

5. **Add logging**
   - Log important operations
   - Log errors with context
   - Use structured logging

6. **Build and test**
   ```bash
   dotnet build AgOpenNtripCaster.Server
   dotnet test AgOpenNtripCaster.Server.Tests
   ```

**Files typically modified:**
- `Models/DTOs/*.cs`
- `Services/*/*.cs`
- `Controllers/*.cs`
- `Program.cs`

---

#### Phase 4: Caster Logic Layer (if applicable)

**When needed:** Feature affects NTRIP protocol handling, streaming, or client/source management

**Steps:**
1. **Identify affected components**
   - `NtripServerService.cs` - Main caster logic
   - `ClientSession.cs` - Individual client handling
   - `SourceConnection.cs` - Source stream management

2. **Implement protocol changes**
   - RTCM message parsing/generation
   - NTRIP handshake modifications
   - Stream routing logic

3. **Update connection handling**
   - Client authentication flow
   - Source connection lifecycle
   - Data broadcast mechanism

4. **Add performance monitoring**
   - Track metrics (bytes sent/received, latency)
   - Implement health checks
   - Add SignalR real-time updates if needed

5. **Test under load**
   - Simulate multiple clients
   - Test with various network conditions
   - Monitor memory and CPU usage

**Files typically modified:**
- `Services/NtripServerService.cs`
- `Services/ClientSession.cs`
- `Services/SourceConnection.cs`
- `Hubs/*Hub.cs` (for SignalR)

---

#### Phase 5: Frontend/UI Layer

**When needed:** Feature needs user interface or admin configuration

**Steps:**
1. **Create TypeScript types**
   - Add interfaces in `src/types/index.ts`
   - Match backend DTOs exactly

2. **Create API service**
   - Add methods in `src/services/*.ts`
   - Use axios with proper error handling
   - Add TypeScript types for requests/responses

3. **Create/update React component**
   - Create component in `src/pages/` or `src/components/`
   - Implement form validation
   - Add loading and error states
   - Use consistent styling (CSS modules)

4. **Add routing** (if new page)
   - Update `src/App.tsx` with new route
   - Add to sidebar navigation in `src/components/Layout/Sidebar.tsx`
   - Implement role-based access control with `<ProtectedRoute>`

5. **Implement real-time updates** (if needed)
   - Connect to SignalR hub
   - Handle incoming events
   - Update UI reactively

6. **Build and test**
   ```bash
   cd AgOpenNtripCaster.Client
   npm run build
   npm run test
   ```

**Files typically modified:**
- `src/types/index.ts`
- `src/services/*.ts`
- `src/pages/admin/*.tsx` or `src/pages/dashboard/*.tsx`
- `src/components/**/*.tsx`
- `src/App.tsx`
- `src/components/Layout/Sidebar.tsx`

---

#### Phase 6: Integration & Testing

**Always required for EVERY feature**

**Steps:**
1. **End-to-end test**
   - Test complete user workflow
   - Verify all layers communicate correctly
   - Check error handling

2. **Role-based access test**
   - Test as Admin user
   - Test as ReadOnly user
   - Test as regular User
   - Verify proper restrictions

3. **Performance test** (for high-impact features)
   - Load test with realistic data
   - Monitor resource usage
   - Check for memory leaks

4. **Documentation**
   - Update API documentation
   - Add inline code comments
   - Update user guides if needed

---

#### Phase 7: Verification & Summary

**Required before marking feature complete**

**Steps:**
1. **Create verification checklist**
   - List all implemented components
   - Verify each layer is complete
   - Confirm all tests pass

2. **Generate feature summary**
   - What was implemented in each layer
   - What files were modified
   - What endpoints were added/changed
   - What UI components were created

3. **Review with user**
   - Present complete summary
   - Verify all requirements met
   - Get explicit confirmation before proceeding

---

## Feature Implementation Template

Use this template for EVERY feature:

```markdown
## Feature: [Feature Name]

### 1. Scope & Requirements
- **Problem:** [What problem does this solve?]
- **Users:** [Who will use this?]
- **Success Criteria:** [How do we know it's successful?]

### 2. Affected Layers
- [ ] Database
- [ ] Backend API
- [ ] Caster Logic
- [ ] Frontend UI

### 3. Implementation Checklist

#### Database Layer
- [ ] Entity classes created/updated
- [ ] DbContext updated
- [ ] Migration created
- [ ] Migration applied
- [ ] Schema verified

#### Backend API Layer
- [ ] DTOs created
- [ ] Service interface created
- [ ] Service implementation created
- [ ] Controller endpoints created
- [ ] Authorization configured
- [ ] Services registered in DI
- [ ] Logging added
- [ ] Built successfully

#### Caster Logic Layer
- [ ] Protocol changes implemented
- [ ] Connection handling updated
- [ ] Performance monitoring added
- [ ] Load tested

#### Frontend UI Layer
- [ ] TypeScript types created
- [ ] API service methods created
- [ ] React components created
- [ ] Routing configured
- [ ] Authorization implemented
- [ ] Built successfully

#### Integration & Testing
- [ ] End-to-end test passed
- [ ] Role-based access tested
- [ ] Performance acceptable
- [ ] Documentation updated

### 4. Implementation Summary

#### Database Changes
**Files modified:**
- [ ] List files

**Schema changes:**
- [ ] Describe changes

#### Backend Changes
**Files modified:**
- [ ] List files

**New endpoints:**
- [ ] List endpoints with HTTP methods

**Services added:**
- [ ] List services

#### Caster Changes
**Files modified:**
- [ ] List files

**Protocol changes:**
- [ ] Describe changes

#### Frontend Changes
**Files modified:**
- [ ] List files

**New pages/components:**
- [ ] List components

**New routes:**
- [ ] List routes

### 5. Testing Results
- [ ] Manual testing completed
- [ ] Automated tests passed
- [ ] Performance benchmarks met
- [ ] No regressions introduced

### 6. User Verification
- [ ] Feature demonstrated to user
- [ ] User confirmed all requirements met
- [ ] User approved to proceed to next feature
```

---

## Anti-Patterns to AVOID

### ❌ DON'T DO THIS:
1. **Partial implementation**
   - "Let's just do the backend first and UI later"
   - Results in: Half-finished features, forgotten requirements

2. **Jumping between features**
   - "Let's start feature B while feature A is 80% done"
   - Results in: Context switching, incomplete features

3. **Skipping layers**
   - "We don't need UI for this yet"
   - Results in: Features impossible to test or use

4. **No verification**
   - "I think it's done, let's move on"
   - Results in: Unknown state, regression bugs

### ✅ DO THIS:
1. **Complete one feature fully**
2. **Test thoroughly at each layer**
3. **Verify with user before proceeding**
4. **Document as you go**

---

## Example: Complete Feature Implementation

### Feature: "Add User Email Verification"

#### Layers Affected: ✅ Database, ✅ Backend, ❌ Caster Logic, ✅ Frontend

#### Phase 1: Database
```
✅ Created: Models/Entities/EmailVerification.cs
✅ Updated: ApplicationDbContext.cs (added DbSet)
✅ Created: Migration 20250118_AddEmailVerification
✅ Applied: Migration to database
✅ Verified: Schema in database matches design
```

#### Phase 2: Backend
```
✅ Created: Models/DTOs/VerifyEmailRequest.cs
✅ Created: Models/DTOs/VerifyEmailResponse.cs
✅ Created: Services/Auth/IEmailVerificationService.cs
✅ Implemented: Services/Auth/EmailVerificationService.cs
✅ Created: Controllers/EmailVerificationController.cs
   - POST /api/auth/verify-email
   - POST /api/auth/resend-verification
✅ Registered: Services in Program.cs
✅ Added: Logging throughout
✅ Built: Successfully (dotnet build)
```

#### Phase 3: Caster Logic
```
N/A - Feature doesn't affect NTRIP protocol
```

#### Phase 4: Frontend
```
✅ Created: src/types/index.ts (VerifyEmailRequest interface)
✅ Created: src/services/auth.ts (verifyEmail, resendVerification methods)
✅ Created: src/pages/auth/VerifyEmailPage.tsx
✅ Updated: src/App.tsx (added /auth/verify-email route)
✅ Updated: src/pages/admin/UsersManagement.tsx (verification status column)
✅ Built: Successfully (npm run build)
```

#### Phase 5: Testing
```
✅ Manual test: User registration → email sent → click link → account verified
✅ Manual test: Expired link → resend button works
✅ Manual test: Login blocked before verification
✅ Manual test: Admin sees verification status in Users table
✅ Role test: ReadOnly user can view verification status but not resend
```

#### Phase 6: Verification Summary

**Presented to user:**
```
Feature "Add User Email Verification" is now COMPLETE:

DATABASE:
- Added EmailVerification table
- Migration 20250118_AddEmailVerification applied

BACKEND:
- POST /api/auth/verify-email - Verify email with token
- POST /api/auth/resend-verification - Resend verification email
- EmailVerificationService handles all logic
- Proper error handling and logging added

FRONTEND:
- /auth/verify-email page shows verification status
- Resend button when link expires
- Users Management table shows verification status (⚠️ Unverified badge)
- Login blocked until email verified

TESTED:
- Complete registration → verification → login flow works
- Expired links handled properly
- Admin UI shows correct status
- All roles tested (Admin, ReadOnly, User)

READY FOR PRODUCTION: Yes
User approval: [WAITING FOR CONFIRMATION]
```

---

## Workflow Summary

```
┌─────────────────────────────────────────────────────────────┐
│ 1. Feature Definition & Planning                            │
├─────────────────────────────────────────────────────────────┤
│ 2. Database Implementation (if needed)                      │
│    └─> Entities → DbContext → Migration → Verify           │
├─────────────────────────────────────────────────────────────┤
│ 3. Backend API Implementation                               │
│    └─> DTOs → Services → Controllers → Register → Build    │
├─────────────────────────────────────────────────────────────┤
│ 4. Caster Logic Implementation (if needed)                  │
│    └─> Protocol → Connections → Monitoring → Load Test     │
├─────────────────────────────────────────────────────────────┤
│ 5. Frontend UI Implementation                               │
│    └─> Types → API Service → Components → Routes → Build   │
├─────────────────────────────────────────────────────────────┤
│ 6. Integration Testing                                      │
│    └─> E2E Test → Role Test → Performance Test             │
├─────────────────────────────────────────────────────────────┤
│ 7. Verification & User Approval                             │
│    └─> Summary → Demo → Approval → Proceed                 │
└─────────────────────────────────────────────────────────────┘
```

**ONLY after step 7 approval: Move to next feature**

---

## Benefits of This Workflow

1. **No Context Switching** - Complete focus on one feature at a time
2. **Immediate Testability** - Each feature can be tested end-to-end right away
3. **Clear Progress** - Always know exactly what's done and what's remaining
4. **Reduced Bugs** - Thorough testing prevents regressions
5. **Better Documentation** - Clear record of all changes
6. **User Confidence** - They see complete, working features, not fragments

---

## Agent Instructions

When implementing features in AgOpenNtripCaster:

1. **Read this document first** before starting ANY feature
2. **Use the template** for every feature
3. **Follow the phases** in order, completely
4. **Generate summaries** after each feature
5. **Wait for user approval** before proceeding to next feature
6. **Update this document** if workflow improvements are discovered

Remember: **One feature, completely finished, is worth more than ten features half-done.**
