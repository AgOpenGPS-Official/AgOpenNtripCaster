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
 * Subscribes to DashboardStatsUpdated events and provides real-time updates
 * of server statistics including active clients, sources, bandwidth, and uptime.
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
    // Check initial connection state
    setConnected(signalRService.isConnected());

    // Subscribe to stats updates
    const unsubscribeStats = signalRService.onDashboardStats((newStats: DashboardStats) => {
      console.log('📊 Dashboard stats received via hook:', {
        activeClients: newStats.activeClients,
        activeSources: newStats.activeSources,
      });
      setStats(newStats);
      setLoading(false);
      setError(null);
    });

    // Subscribe to connection status changes
    const unsubscribeConnection = signalRService.onConnectionStatusChange(
      (isConnected: boolean) => {
        console.log('SignalR connection status:', isConnected);
        setConnected(isConnected);
        if (!isConnected) {
          setLoading(true);
        }
      }
    );

    // Cleanup subscriptions on unmount
    return () => {
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
