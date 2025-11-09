import { useMemo } from 'react';
import { useClientPositionsContext, type ClientPosition } from '../contexts/ClientPositionsContext';

export type { ClientPosition };

/**
 * Hook for real-time client positions
 * Can filter by username if provided (for user dashboard)
 * Shows all clients if no username provided (for admin)
 *
 * Uses ClientPositionsContext for persistent state across pages
 */
export const useClientPositions = (filterByUsername?: string) => {
  const { clients, isConnected } = useClientPositionsContext();

  // Filter by username if provided
  const filteredClients = useMemo(() => {
    if (!filterByUsername) {
      return Array.from(clients.values());
    }

    // Extract local part of NTRIP username (before @) for comparison
    return Array.from(clients.values()).filter((client) => {
      const clientUsernameLocal = client.username.split('@')[0];
      return clientUsernameLocal === filterByUsername;
    });
  }, [clients, filterByUsername]);

  return {
    clients: filteredClients,
    isConnected,
    clientCount: filteredClients.length,
  };
};
