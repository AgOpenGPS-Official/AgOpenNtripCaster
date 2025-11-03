import { useEffect, useState } from 'react';
import { signalRService } from '../services/signalRService';

export interface ConnectionStats {
  totalConnections: number;
  activeClientCount: number;
  activeSourceCount: number;
  throughputMbps: number;
  uploadMbps: number;
  downloadMbps: number;
  averageLatencyMs: number;
  cpuUsagePercent: number;
  memoryUsagePercent: number;
  collectedAt: string;
}

export interface UseConnectionStatsResult {
  stats: ConnectionStats | null;
  isLoading: boolean;
}

/**
 * Custom hook for real-time connection statistics via SignalR
 *
 * Subscribes to ConnectionStatsUpdated events for live performance metrics
 * including throughput, latency, CPU and memory usage.
 *
 * Usage:
 * ```tsx
 * const { stats } = useConnectionStats();
 *
 * if (!stats) return <div>Loading...</div>;
 *
 * return (
 *   <div>
 *     <p>Throughput: {stats.throughputMbps} Mbps</p>
 *     <p>CPU: {stats.cpuUsagePercent}%</p>
 *     <p>Memory: {stats.memoryUsagePercent}%</p>
 *   </div>
 * );
 * ```
 */
export function useConnectionStats(): UseConnectionStatsResult {
  const [stats, setStats] = useState<ConnectionStats | null>(null);
  const [isLoading, setIsLoading] = useState(true);

  useEffect(() => {
    let isMounted = true;

    // Subscribe to connection stats updates
    const unsubscribeStats = signalRService.onConnectionStatsUpdated(
      (newStats: ConnectionStats) => {
        if (isMounted) {
          setStats(newStats);
          if (isLoading) {
            setIsLoading(false);
          }
        }
      }
    );

    // Cleanup subscription on unmount
    return () => {
      isMounted = false;
      unsubscribeStats();
    };
  }, [isLoading]);

  return {
    stats,
    isLoading,
  };
}
