import React, { createContext, useContext, useEffect, useState } from 'react';
import { signalRService } from '../services/signalRService';
import type { DashboardStats } from '../services/signalRService';

interface DashboardStatsContextType {
  stats: DashboardStats | null;
  connected: boolean;
}

const DashboardStatsContext = createContext<DashboardStatsContextType>({
  stats: null,
  connected: false,
});

export const useDashboardStatsContext = () => useContext(DashboardStatsContext);

/**
 * DashboardStatsProvider - Persistent dashboard statistics across all pages
 *
 * - Subscribes to real-time SignalR DashboardStatsUpdated events
 * - Keeps latest stats in memory during page navigation
 * - Shared across all components that use useDashboardStats hook
 * - Stats include: activeClients, activeSources, bandwidth, uptime
 */
export const DashboardStatsProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [stats, setStats] = useState<DashboardStats | null>(null);
  const [connected, setConnected] = useState(false);

  useEffect(() => {
    // Check initial connection state
    setConnected(signalRService.isConnected());

    // Subscribe to real-time stats updates from SignalR
    const unsubscribeStats = signalRService.onDashboardStats((newStats: DashboardStats) => {
      setStats(newStats);
    });

    // Subscribe to connection status changes
    const unsubscribeConnection = signalRService.onConnectionStatusChange(
      (isConnected: boolean) => {
        setConnected(isConnected);
      }
    );

    // Cleanup subscriptions on unmount
    return () => {
      unsubscribeStats();
      unsubscribeConnection();
    };
  }, []);

  return (
    <DashboardStatsContext.Provider value={{ stats, connected }}>
      {children}
    </DashboardStatsContext.Provider>
  );
};
