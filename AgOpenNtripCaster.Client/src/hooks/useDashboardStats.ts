import { useEffect, useState } from 'react';
import { signalRService } from '../services/signalRService';
import { dashboardStatsApi } from '../services/dashboardStatsApi';
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
 * - On mount: Loads initial stats via REST API (immediate data)
 * - Then: Subscribes to DashboardStatsUpdated events for real-time updates
 * - Provides real-time updates of server statistics including active clients,
 *   sources, bandwidth, and uptime.
 *
 * This hybrid approach ensures we always show data, even on first load,
 * while getting real-time updates when connections/disconnections occur.
 *
 * Usage:
 * ```tsx
 * const { stats, connected, loading, error } = useDashboardStats();
 *
 * if (loading) return <div>Loading stats...</div>;
 * if (error) return <div>Error: {error.message}</div>;
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
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<Error | null>(null);

  useEffect(() => {
    let isMounted = true;

    const initializeStats = async () => {
      try {
        // First: Load initial stats via REST API
        console.log('📊 Loading initial dashboard stats via API...');
        const initialStats = await dashboardStatsApi.getDashboardStats();

        if (isMounted) {
          setStats(initialStats);
          setLoading(false);
          setError(null);
        }
      } catch (err) {
        console.error('Failed to load initial dashboard stats:', err);
        if (isMounted) {
          setError(err instanceof Error ? err : new Error('Failed to load stats'));
          setLoading(false);
        }
      }
    };

    // Load initial stats
    initializeStats();

    // Check initial connection state
    setConnected(signalRService.isConnected());

    // Subscribe to real-time stats updates (will override API data when events arrive)
    const unsubscribeStats = signalRService.onDashboardStats((newStats: DashboardStats) => {
      if (isMounted) {
        setStats(newStats);
        setError(null);
      }
    });

    // Subscribe to connection status changes
    const unsubscribeConnection = signalRService.onConnectionStatusChange(
      (isConnected: boolean) => {
        if (isMounted) {
          console.log('SignalR connection status:', isConnected);
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
    loading,
    error,
  };
}
