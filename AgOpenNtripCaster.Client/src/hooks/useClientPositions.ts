import { useEffect, useState, useCallback, useMemo } from 'react';
import { signalRService } from '../services/signalRService';
import type { ClientPositionUpdate } from '../services/signalRService';

export interface ClientPosition {
  id: string;
  username: string;
  serialNumber: number;
  mountPoint: string;
  latitude: number;
  longitude: number;
  accuracy?: number;
  lastUpdate: number;
  isStale: boolean;
}

const STALE_TIMEOUT_MS = 15000; // 15 seconds

/**
 * Hook for real-time client positions
 * Can filter by username if provided (for user dashboard)
 * Shows all clients if no username provided (for admin)
 */
export const useClientPositions = (filterByUsername?: string) => {
  const [clients, setClients] = useState<Map<string, ClientPosition>>(new Map());
  const [isConnected, setIsConnected] = useState(false);

  // Mark position as stale if not updated in 15 seconds
  const markStaleIfNeeded = useCallback(() => {
    setClients((prevClients) => {
      const updated = new Map(prevClients);
      const now = Date.now();

      updated.forEach((client) => {
        const age = now - client.lastUpdate;
        client.isStale = age > STALE_TIMEOUT_MS;
      });

      return updated;
    });
  }, []);

  // Handle position update from SignalR
  const handlePositionUpdate = useCallback((update: ClientPositionUpdate) => {
    // If filtering by username, skip updates from other users
    if (filterByUsername && update.username !== filterByUsername) {
      return;
    }

    setClients((prevClients) => {
      const updated = new Map(prevClients);

      updated.set(update.clientId, {
        id: update.clientId,
        username: update.username,
        serialNumber: 1, // Will be updated when we send serial info
        mountPoint: update.mountPoint,
        latitude: update.latitude,
        longitude: update.longitude,
        accuracy: update.accuracy,
        lastUpdate: new Date(update.timestamp).getTime(),
        isStale: false,
      });

      return updated;
    });
  }, [filterByUsername]);

  // Set up stale timeout check
  useEffect(() => {
    const interval = setInterval(markStaleIfNeeded, 1000);
    return () => clearInterval(interval);
  }, [markStaleIfNeeded]);

  // Connect to SignalR on mount
  useEffect(() => {
    const connectToSignalR = async () => {
      const connected = await signalRService.connect();
      setIsConnected(connected);
    };

    connectToSignalR();

    // Subscribe to position updates
    const unsubscribePosition = signalRService.onPositionUpdate(handlePositionUpdate);

    // Subscribe to connection status changes
    const unsubscribeConnection = signalRService.onConnectionStatusChange((connected) => {
      setIsConnected(connected);
    });

    return () => {
      unsubscribePosition();
      unsubscribeConnection();
      // Don't disconnect on unmount - keep connection alive for other hooks
    };
  }, [handlePositionUpdate]);

  // Convert map to array for easier use
  const clientsArray = useMemo(() => Array.from(clients.values()), [clients]);

  return {
    clients: clientsArray,
    isConnected,
    clientCount: clients.size,
  };
};
