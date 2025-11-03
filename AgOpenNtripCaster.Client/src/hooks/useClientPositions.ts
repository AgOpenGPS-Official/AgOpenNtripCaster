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

  // Validate position data
  const isValidPosition = (lat: number, lon: number): boolean => {
    return (
      typeof lat === 'number' &&
      typeof lon === 'number' &&
      !isNaN(lat) &&
      !isNaN(lon) &&
      lat >= -90 &&
      lat <= 90 &&
      lon >= -180 &&
      lon <= 180
    );
  };

  // Handle position update from SignalR
  const handlePositionUpdate = useCallback((update: ClientPositionUpdate) => {
    // Extract local part of NTRIP username (before @)
    const updateUsernameLocal = update.username.split('@')[0];

    console.log('✅ handlePositionUpdate called:', {
      clientId: update.clientId,
      username: update.username,
      usernameLocal: updateUsernameLocal,
      lat: update.latitude,
      lon: update.longitude,
      filterByUsername,
    });

    // If filtering by username, skip updates from other users
    if (filterByUsername && updateUsernameLocal !== filterByUsername) {
      console.log('⏭️ Skipping position update: username filter mismatch');
      return;
    }

    // Validate position data before storing
    if (!isValidPosition(update.latitude, update.longitude)) {
      console.warn('❌ Invalid position data received:', { clientId: update.clientId, lat: update.latitude, lon: update.longitude });
      return;
    }

    console.log('📌 Adding/updating client position in map');

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

  // Handle client disconnection from SignalR
  const handleClientDisconnected = useCallback((update: any) => {
    // Extract local part of NTRIP username (before @)
    const updateUsernameLocal = update.username.split('@')[0];

    // If filtering by username, skip if not matching
    if (filterByUsername && updateUsernameLocal !== filterByUsername) {
      return;
    }

    setClients((prevClients) => {
      const updated = new Map(prevClients);
      updated.delete(update.clientId);
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

    // Subscribe to client disconnected events
    const unsubscribeDisconnect = signalRService.onClientDisconnected(handleClientDisconnected);

    // Subscribe to connection status changes
    const unsubscribeConnection = signalRService.onConnectionStatusChange((connected) => {
      setIsConnected(connected);
    });

    return () => {
      unsubscribePosition();
      unsubscribeDisconnect();
      unsubscribeConnection();
      // Don't disconnect on unmount - keep connection alive for other hooks
    };
  }, [handlePositionUpdate, handleClientDisconnected]);

  // Convert map to array for easier use
  const clientsArray = useMemo(() => Array.from(clients.values()), [clients]);

  return {
    clients: clientsArray,
    isConnected,
    clientCount: clients.size,
  };
};
