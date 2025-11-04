# 🎨 UI Enhancements Guide

**Last Updated**: November 2024
**Component**: NtripStatusIndicator, Navbar, Dark Mode System

---

## Overview

This document describes the recent UI enhancements to NtripCaster, focusing on the professional status indicators and dark mode implementation.

---

## 📊 Navbar Status Indicator

### NtripStatusIndicator Component

The `NtripStatusIndicator` is a real-time status display component positioned in the navbar, next to the user profile button.

**Location**: `AgOpenNtripCaster.Client/src/components/Layout/NtripStatusIndicator.tsx`

### Visual Design

```
┌─────────────────────────────┐
│ ↓    NTRIP    ↑            │  ← Green border when active
│ IN           OUT           │     Red border when stopped
└─────────────────────────────┘
```

### Features

#### 1. **Status Display**
- **Box Border**: Green (#10b981) when RTCM listener is active
- **Box Border**: Red (#dc2626) when RTCM listener is stopped
- **Background**: Light tint of status color for visual distinction

#### 2. **Data Flow Indicators**

**Left Arrow (IN)** - Source Data Upload
- **Outline** (light gray): No sources connected
- **Filled Green**: Sources actively uploading RTCM data
- **Label**: "IN" for clarity

**Right Arrow (OUT)** - Client Data Download
- **Outline** (light gray): No clients connected
- **Filled Green**: Clients actively downloading RTCM data
- **Label**: "OUT" for clarity

#### 3. **Real-time Updates**
- Connected to `useDashboardStats` hook
- Updates every 10 seconds via SignalR
- No polling required
- Seamless synchronization with backend state

### Component Structure

```tsx
<NtripStatusIndicator>
  ├── statusCard (flex container)
  │   ├── arrowWrapper (left)
  │   │   ├── svg (arrow down)
  │   │   └── span ("IN" label)
  │   ├── statusContent
  │   │   └── statusText ("NTRIP")
  │   └── arrowWrapper (right)
  │       ├── svg (arrow up)
  │       └── span ("OUT" label)
```

### CSS Properties

**statusCard** (main container)
```css
height: 40px;  /* Matches user button height */
padding: var(--spacing-sm) var(--spacing-md);
border: 2px solid;
border-radius: var(--radius-sm);
gap: 8px;
```

**statusCard.active**
```css
border-color: var(--color-success);  /* Green */
background-color: var(--color-success-bg);  /* Light green tint */
```

**statusCard.inactive**
```css
border-color: var(--color-danger);  /* Red */
background-color: var(--color-danger-bg);  /* Light red tint */
```

**Arrow Styling**
```css
width: 24px;
height: 24px;
transition: all 0.2s ease;

&.outline {
  color: var(--color-text-muted);
  opacity: 0.6;
}

&.filled {
  color: var(--color-success);  /* Green when active */
  fill: var(--color-success);  /* SVG fill */
  opacity: 1;
}
```

**Label Styling**
```css
font-size: 0.6rem;
font-weight: 700;
text-transform: uppercase;
letter-spacing: 0.3px;
color: var(--color-text-secondary);
white-space: nowrap;
```

### Implementation Details

#### Data Flow

1. **Backend** (`NtripServerService.cs`)
   ```csharp
   var stats = new {
       activeClients = /* count */,
       activeSources = /* count */,
       rtcmListenerActive = _tcpListener != null &&
                           !_cancellationTokenSource!.Token.IsCancellationRequested
   };

   await _hubContext.Clients.All.SendAsync("DashboardStatsUpdated", stats);
   ```

2. **Frontend Hook** (`useDashboardStats.ts`)
   ```typescript
   export interface DashboardStats {
       activeClients: number;
       activeSources: number;
       rtcmListenerActive: boolean;  // ← New property
   }
   ```

3. **Component** (`NtripStatusIndicator.tsx`)
   ```typescript
   const { stats } = useDashboardStats();
   const hasUpload = (stats?.activeSources ?? 0) > 0;
   const hasDownload = (stats?.activeClients ?? 0) > 0;
   const isRunning = stats?.rtcmListenerActive ?? false;
   ```

#### Positioning in Navbar

**Before**: System Status Indicator centered, NTRIP in center
**After**: System Status Indicator centered, NTRIP on right next to user button

```tsx
// Navbar layout
<nav>
  <left>Logo + Hamburger</left>
  <center>SystemStatusIndicator</center>
  <right>NtripStatusIndicator + UserMenu</right>
</nav>
```

**Spacing**: 5px margin-right on NTRIP indicator for separation from user button

### Usage Examples

#### Integration in Navbar

```tsx
import { NtripStatusIndicator } from './NtripStatusIndicator';

export const Navbar: React.FC = () => {
  return (
    <nav className={styles.navbar}>
      {/* ... logo and center indicators ... */}
      <div className={styles.right}>
        <NtripStatusIndicator />  {/* Before user menu */}
        <div className={styles.userMenu}>
          {/* User profile button */}
        </div>
      </div>
    </nav>
  );
};
```

#### Direct Usage

```tsx
import { NtripStatusIndicator } from '../components/Layout/NtripStatusIndicator';

// Can be used anywhere in the application
function MyComponent() {
  return <NtripStatusIndicator />;
}
```

### States and Transitions

#### State 1: Inactive (No Sources, No Clients)
```
┌─────────────────────────────┐
│ ⬇    NTRIP    ⬆            │  ← Red border
│ IN           OUT           │
└─────────────────────────────┘
```
- Box: Red border and light red background
- Left arrow: Outline (gray)
- Right arrow: Outline (gray)

#### State 2: Sources Uploading
```
┌─────────────────────────────┐
│ ⬇    NTRIP    ⬆            │  ← Green border
│ IN           OUT           │
└─────────────────────────────┘
```
- Box: Green border and light green background
- Left arrow: Filled green (active upload)
- Right arrow: Outline (gray)

#### State 3: Clients Downloading
```
┌─────────────────────────────┐
│ ⬇    NTRIP    ⬆            │  ← Green border
│ IN           OUT           │
└─────────────────────────────┘
```
- Box: Green border and light green background
- Left arrow: Outline (gray)
- Right arrow: Filled green (active download)

#### State 4: Bidirectional (Full Operation)
```
┌─────────────────────────────┐
│ ⬇    NTRIP    ⬆            │  ← Green border
│ IN           OUT           │
└─────────────────────────────┘
```
- Box: Green border and light green background
- Left arrow: Filled green (sources uploading)
- Right arrow: Filled green (clients downloading)

### Theme Support

The component automatically adapts to light and dark modes through CSS variables:

**Light Mode**
```css
--color-success: #10b981;  /* Green */
--color-success-bg: #dcfce7;  /* Light green */
--color-danger: #dc2626;  /* Red */
--color-danger-bg: #fee2e2;  /* Light red */
--color-text-secondary: #6b7280;  /* Gray text for labels */
```

**Dark Mode**
```css
--color-success: #34d399;  /* Lighter green for dark bg */
--color-success-bg: #064e3b;  /* Dark green */
--color-danger: #f87171;  /* Lighter red for dark bg */
--color-danger-bg: #7f1d1d;  /* Dark red */
--color-text-secondary: #9ca3af;  /* Lighter gray text */
```

### Performance Considerations

- **No Re-renders**: Component only re-renders when dashboard stats update
- **Memo Optimization**: Consider wrapping with React.memo() if needed
- **SVG Rendering**: Lightweight SVG arrows (not images)
- **Minimal Dependencies**: Only depends on `useDashboardStats` hook

### Accessibility

- **ARIA Labels**: SVG arrows have semantic meaning through UP/DOWN direction
- **Color Contrast**: Meets WCAG AA standards for color combinations
- **Focus States**: Inherits from parent components
- **Responsive**: Scales with navbar on mobile devices

### Future Enhancements

1. **Tooltip on Hover**
   - Show detailed stats on hover
   - Display connection count, data rate, etc.

2. **Animation on Status Change**
   - Smooth color transition when listener changes
   - Pulse animation for active data flow

3. **Detailed Status Modal**
   - Click to open detailed status information
   - Real-time connection metrics

4. **Customizable Appearance**
   - Size options (compact, normal, large)
   - Color scheme preferences
   - Label customization

---

## 🎯 CSS Variables Design System

The indicator leverages the application's centralized CSS variable system defined in `globals.css`.

### Color Variables Used

```css
/* Status Colors */
--color-success: #10b981;  /* Active/green state */
--color-success-bg: #dcfce7;  /* Success background */
--color-danger: #dc2626;  /* Inactive/red state */
--color-danger-bg: #fee2e2;  /* Danger background */

/* Text Colors */
--color-text-primary: #1f2937;  /* Main text */
--color-text-secondary: #6b7280;  /* Secondary text/labels */
--color-text-muted: #9ca3af;  /* Muted/disabled text */

/* Surface */
--color-surface: #ffffff;  /* Component background */
```

### Radius Variables

```css
--radius-sm: 6px;  /* Small radius for indicator */
```

### Spacing Variables

```css
--spacing-sm: 0.5rem;  /* Small padding */
--spacing-md: 1rem;  /* Medium padding */
```

---

## 🔧 Troubleshooting

### Indicator Not Updating

**Issue**: Status indicator stays red even though listener is active

**Solutions**:
1. Check SignalR connection: `signalRService.isConnected()`
2. Verify `rtcmListenerActive` is being set in backend
3. Check browser console for WebSocket errors
4. Verify `useDashboardStats` hook is working

### Colors Look Wrong

**Issue**: Colors appear different than expected

**Solutions**:
1. Clear browser cache
2. Check `data-theme` attribute on html element
3. Verify CSS variables are loaded in `globals.css`
4. Check for CSS conflicts in component modules

### Arrows Not Aligning

**Issue**: UP and DOWN arrows don't align vertically

**Solutions**:
1. Ensure SVG viewBox is correct (0 0 24 24)
2. Check arrowWrapper flex properties
3. Verify font-size is consistent
4. Check line-height settings

---

## 📖 Related Documentation

- [DARK_MODE.md](./DARK_MODE.md) - CSS variables and theme system
- [LOCAL_DEV.md](./LOCAL_DEV.md) - Development setup
- [ARCHITECTURE_PLAN_ASPNET9.md](./ARCHITECTURE_PLAN_ASPNET9.md) - System design
- Parent: [README.md](../README.md)

---

**Questions?** Check the GitHub issues or create a new discussion.
