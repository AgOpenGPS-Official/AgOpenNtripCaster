# SignalR Optimization Plan

## 📋 Overview

This document outlines the comprehensive optimization of SignalR communication in NtripCaster, reducing inefficient REST API polling and implementing true real-time data flow.

**Status**: IN PROGRESS
**Start Date**: 2025-11-02
**Target Completion**: 2025-11-04

---

## 🎯 Goals

1. **Reduce REST API Polling** - Move high-frequency data fetching to SignalR push model
2. **Improve Real-time Responsiveness** - Eliminate 5-30 second polling delays
3. **Lower Server Load** - Reduce database queries and HTTP requests
4. **Improve UX** - Instant updates for all dashboard/admin views
5. **Code Cleanliness** - Optimize and simplify communication layer

---

## 📊 Current State Analysis

### Inefficient REST API Calls (Before Optimization)

| Endpoint | Current Use | Frequency | Problem |
|----------|------------|-----------|---------|
| `/admin/config/stats` | Dashboard stats | Every 5s | Polling, multi-user load |
| `/activity/recent` | Activity feed | Page load + 5s | Stale data, redundant queries |
| `/mountpoints` | Mount point list | Every 5-30s | Status delays |
| `/admin/analytics/*` | Analytics dashboard | On load | Not real-time |
| `/users/*` | User data | On load | Static data |

### Current SignalR Usage (After Optimization)

| Event | Status | Receiver |
|-------|--------|----------|
| `ClientPositionUpdated` | ✅ Working | Real-time map |
| `ClientStreamStatusChanged` | ✅ Working | Status dashboard |
| `ClientConnected` | ⚠️ Partial | Hub only, not broadcast |
| `ClientDisconnected` | ⚠️ Partial | Hub only, not broadcast |
| (Others) | ❌ Missing | Not implemented yet |

---

## 🚀 Phase 1: Critical Optimizations

### 1.1 Dashboard Stats via SignalR

**Current State**: REST API polling `/admin/config/stats` every 5 seconds

**Changes**:
```typescript
// Before: API call every 5 seconds
const stats = await dashboardStatsApi.getDashboardStats();

// After: Real-time push
signalRService.onDashboardStats((stats) => {
  setStats(stats);
});
```

**Backend SignalR Events to Implement**:
- `DashboardStatsUpdated` - Push stats on client/source connect/disconnect
- Trigger: ClientHealthMonitor detects changes

**Frontend Changes**:
- Remove polling loop from DashboardPage
- Subscribe to `DashboardStatsUpdated` event
- Update stats in real-time

**Estimated Load Reduction**: 95% fewer queries

---

### 1.2 Recent Activity Feed via SignalR

**Current State**: REST API polling `/activity/recent` every page load

**Changes**:
```typescript
// Before: Static load + periodic refresh
const activities = await activityApi.getRecentActivities(20);

// After: Real-time stream
signalRService.onActivityCreated((activity) => {
  addActivityToFeed(activity);
});
```

**Backend SignalR Events to Implement**:
- `ActivityCreated` - Broadcast new activity events
- Trigger: NtripServerService when connections/disconnections happen

**Frontend Changes**:
- Load initial 20 activities via API (one-time)
- Subscribe to `ActivityCreated` event
- Prepend new activities to feed in real-time

**Estimated Load Reduction**: 80% fewer activity queries

---

### 1.3 Mount Points Status Updates via SignalR

**Current State**: REST API polling `/mountpoints` every 5-30 seconds

**Changes**:
```typescript
// Before: Polling for status changes
const mountPoints = await mountPointsApi.getMountPoints(1, 100);

// After: Real-time push updates
signalRService.onMountPointStatusChanged((update) => {
  updateMountPointStatus(update);
});
```

**Backend SignalR Events to Implement**:
- `SourceConnected` - Broadcast source connection to mount point
- `SourceDisconnected` - Broadcast source disconnection
- `MountPointStatusChanged` - Generic mount point status updates

**Frontend Changes**:
- Load mount points once on page load
- Subscribe to source connect/disconnect events
- Update activeSourceCount in real-time
- Update activeClientCount in real-time

**Estimated Load Reduction**: 90% fewer mount point queries

---

## 🎨 Phase 2: Enhanced Features

### 2.1 Real-time Alerts & Notifications

**Events to Implement**:
- `ErrorOccurred` - System errors
- `WarningOccurred` - Warnings
- `SystemAlert` - Important alerts
- `PermissionDenied` - Auth/permission events

**Use Case**: Admin gets instant notification when critical error occurs

---

### 2.2 Connection Statistics Streaming

**Events to Implement**:
- `StatisticsUpdate` - Periodic stats push (throughput, frame rates, etc)

**Use Case**: Real-time analytics dashboard with live metrics

---

### 2.3 User Activity Events

**Events to Implement**:
- `UserLoggedIn` - User login events
- `UserLoggedOut` - User logout events
- `UserPermissionChanged` - Permission changes

**Use Case**: Admin sees who's online/offline instantly

---

## 📝 Implementation Strategy

### Backend Changes Needed

1. **NtripHub.cs** - Add new event methods
   - `OnDashboardStatsChanged()`
   - `OnActivityCreated()`
   - `OnMountPointStatusChanged()`
   - `OnSystemAlert()`

2. **NtripServerService.cs** - Broadcast events
   - Broadcast stats changes on client/source events
   - Broadcast activities to all clients

3. **ClientHealthMonitor.cs** - Trigger broadcasts
   - On client disconnect → broadcast dashboard stats update
   - On client disconnect → broadcast activity

4. **Create ActivityHub.cs** (optional)
   - Separate hub for activity events if needed

### Frontend Changes Needed

1. **signalRService.ts** - Add event handlers
   - `onDashboardStats()`
   - `onActivityCreated()`
   - `onMountPointStatusChanged()`
   - `onSystemAlert()`

2. **DashboardPage.tsx** - Replace polling
   - Remove `dashboardStatsApi` polling
   - Subscribe to `onDashboardStats()`

3. **useMountPoints.ts** (new hook) - Real-time mount point updates
   - Load once on mount
   - Subscribe to source connect/disconnect events
   - Update state in real-time

4. **useActivityFeed.ts** (new hook) - Real-time activity
   - Load initial activities
   - Subscribe to `onActivityCreated()`

---

## 🔍 Performance Impact

### Before Optimization

```
Dashboard Load (10 users):
- /admin/config/stats: 2 calls/user/10s = 20 calls/10s = 2 QPS
- /activity/recent: 1 call/user/load = 10 calls/10s (spiky)
- /mountpoints: 1 call/user/30s = 0.33 QPS

Total: ~2.5 QPS sustained + spiky activity calls
DB Load: High on dashboard endpoints
```

### After Optimization

```
Dashboard Load (10 users):
- SignalR broadcast: 1 message/event (shared across users!)
- Activity stream: 1 message/event
- Mount point updates: 1 message/event

Total: Near-zero polling, bandwidth-efficient broadcasts
DB Load: Minimal, only on actual changes
```

---

## ✅ Testing Strategy

### Phase 1 Verification

- [ ] Dashboard stats update in real-time when clients connect/disconnect
- [ ] Activity feed shows new entries immediately
- [ ] Mount point active counts update instantly
- [ ] No API polling in Network tab (DevTools)
- [ ] Multiple dashboards show same data in sync

### Phase 2 Verification

- [ ] Alerts appear instantly
- [ ] Statistics update smoothly
- [ ] User activity events are timely

---

## 🐛 Potential Issues & Mitigation

| Issue | Risk | Mitigation |
|-------|------|-----------|
| Message ordering | Medium | Use timestamps, handle out-of-order |
| Duplicate events | Medium | Add event IDs for deduplication |
| Connection drops | Medium | Re-subscribe on reconnect |
| Memory leaks | Low | Proper unsubscribe in cleanup |
| Rate limiting needed | Low | Add debounce if too many events |

---

## 📅 Milestone Timeline

| Milestone | Target Date | Status |
|-----------|------------|--------|
| Phase 1.1 - Dashboard Stats | Nov 2 | 🔄 In Progress |
| Phase 1.2 - Activity Feed | Nov 2 | 🔄 In Progress |
| Phase 1.3 - Mount Points | Nov 3 | ⏳ Pending |
| Phase 2 - Enhanced Features | Nov 3-4 | ⏳ Pending |
| Testing & Refinement | Nov 4 | ⏳ Pending |
| Code Review & Merge | Nov 4 | ⏳ Pending |

---

## 📚 References

- [SignalR Documentation](https://learn.microsoft.com/en-us/aspnet/signalr/)
- [NtripCaster Architecture](../ARCHITECTURE_PLAN_ASPNET9.md)
- [Current Progress](./PROGRESS.md)

---

**Last Updated**: 2025-11-02
**Author**: SignalR Optimization Task
**Status**: 🔄 In Progress
