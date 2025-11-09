import { useDashboardStatsContext } from '../contexts/DashboardStatsContext';
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
 * - Uses DashboardStatsContext for persistent state across pages
 * - Data persists during navigation (no reload needed)
 * - Real-time updates via SignalR events
 * - Provides real-time updates of server statistics including active clients,
 *   sources, bandwidth, and uptime.
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
  const { stats, connected } = useDashboardStatsContext();

  return {
    stats,
    connected,
    loading: false,
    error: null,
  };
}
