import React, { createContext, useContext, useEffect, useState, useCallback } from 'react';
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

interface ClientPositionsContextType {
  clients: Map<string, ClientPosition>;
  isConnected: boolean;
}

const ClientPositionsContext = createContext<ClientPositionsContextType>({
  clients: new Map(),
  isConnected: false,
});

export const useClientPositionsContext = () => useContext(ClientPositionsContext);

const STALE_TIMEOUT_MS = 15000; // 15 seconds

/**
 * ClientPositionsProvider - Persistent client positions state across all pages
 *
 * - Subscribes to real-time SignalR position updates
 * - Keeps all client positions in memory during page navigation
 * - Marks positions as stale if not updated in 15 seconds
 * - Removes clients when they disconnect
 * - Shared across all components that use useClientPositions hook
 */
export const ClientPositionsProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [clients, setClients] = useState<Map<string, ClientPosition>>(new Map());
  const [isConnected, setIsConnected] = useState(false);

  // Validate position data
  const isValidPosition = useCallback((lat: number, lon: number): boolean => {
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
  }, []);

  // Handle position update from SignalR
  const handlePositionUpdate = useCallback((update: ClientPositionUpdate) => {
    // Validate position data before storing
    if (!isValidPosition(update.latitude, update.longitude)) {
      console.warn('Invalid position data received:', {
        clientId: update.clientId,
        lat: update.latitude,
        lon: update.longitude
      });
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
  }, [isValidPosition]);

  // Handle client disconnection from SignalR
  const handleClientDisconnected = useCallback((update: any) => {
    setClients((prevClients) => {
      const updated = new Map(prevClients);
      updated.delete(update.clientId);
      return updated;
    });
  }, []);

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

  // Set up stale timeout check
  useEffect(() => {
    const interval = setInterval(markStaleIfNeeded, 1000);
    return () => clearInterval(interval);
  }, [markStaleIfNeeded]);

  // Subscribe to SignalR events
  useEffect(() => {
    // Check initial connection state
    setIsConnected(signalRService.isConnected());

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
    };
  }, [handlePositionUpdate, handleClientDisconnected]);

  return (
    <ClientPositionsContext.Provider
      value={{
        clients,
        isConnected,
      }}
    >
      {children}
    </ClientPositionsContext.Provider>
  );
};
