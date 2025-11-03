import { useEffect, useState } from 'react';
import { signalRService } from '../services/signalRService';
import type { DashboardStats } from '../services/signalRService';

export interface UseDashboardStatsResult {
  stats: DashboardStats | null;
  connected: boolean;
  loading: boolean;
  error: Error | null;
}

/**
 * Custom hook for real-time dashboard statistics via SignalR
 *
 * - Connects to SignalR on mount
 * - Subscribes to DashboardStatsUpdated events for real-time updates
 * - Provides real-time updates of server statistics including active clients,
 *   sources, bandwidth, and uptime.
 * - Stats are loaded progressively as SignalR events arrive
 *
 * Note: No REST API call is made because the endpoint requires admin permissions
 * and stats are immediately available via SignalR subscriptions.
 *
 * Usage:
 * ```tsx
 * const { stats, connected } = useDashboardStats();
 *
 * return (
 *   <div>
 *     <p>Active Clients: {stats?.activeClients ?? 0}</p>
 *     <p>Active Sources: {stats?.activeSources ?? 0}</p>
 *   </div>
 * );
 * ```
 */
export function useDashboardStats(): UseDashboardStatsResult {
  const [stats, setStats] = useState<DashboardStats | null>(null);
  const [connected, setConnected] = useState(false);

  useEffect(() => {
    let isMounted = true;

    // Check initial connection state
    setConnected(signalRService.isConnected());

    // Subscribe to real-time stats updates from SignalR
    const unsubscribeStats = signalRService.onDashboardStats((newStats: DashboardStats) => {
      if (isMounted) {
        setStats(newStats);
      }
    });

    // Subscribe to connection status changes
    const unsubscribeConnection = signalRService.onConnectionStatusChange(
      (isConnected: boolean) => {
        if (isMounted) {
          setConnected(isConnected);
        }
      }
    );

    // Cleanup subscriptions on unmount
    return () => {
      isMounted = false;
      unsubscribeStats();
      unsubscribeConnection();
    };
  }, []);

  return {
    stats,
    connected,
    loading: false,
    error: null,
  };
}
