import React, { createContext, useContext, useEffect, useState, useCallback } from 'react';
import { signalRService } from '../services/signalRService';
import { mountPointsApi } from '../services/mountPointsApi';
import type { MountPointDto } from '../types';

interface MountPointsContextType {
  mountPoints: MountPointDto[];
  loading: boolean;
  error: Error | null;
  refreshMountPoints: () => Promise<void>;
}

const MountPointsContext = createContext<MountPointsContextType>({
  mountPoints: [],
  loading: true,
  error: null,
  refreshMountPoints: async () => {},
});

export const useMountPointsContext = () => useContext(MountPointsContext);

/**
 * MountPointsProvider - Persistent mount points state across all pages
 *
 * - Loads initial mount points once on mount
 * - Subscribes to real-time SignalR events
 * - Keeps data in memory during page navigation
 * - Shared across all components that use useMountPoints hook
 */
export const MountPointsProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [mountPoints, setMountPoints] = useState<MountPointDto[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<Error | null>(null);

  // Load all mount points (paginate through all pages)
  const loadAllMountPoints = useCallback(async () => {
    try {
      setLoading(true);
      setError(null);

      let allMountPoints: MountPointDto[] = [];
      let page = 1;
      let hasMore = true;

      while (hasMore) {
        const response = await mountPointsApi.getMountPoints(page, 100);
        if (response.mountPoints && response.mountPoints.length > 0) {
          allMountPoints = [...allMountPoints, ...response.mountPoints];
          page++;
          hasMore = response.mountPoints.length === 100;
        } else {
          hasMore = false;
        }
      }

      setMountPoints(allMountPoints);
      setLoading(false);
    } catch (err) {
      console.error('Failed to load mount points:', err);
      setError(err instanceof Error ? err : new Error('Failed to load mount points'));
      setLoading(false);
    }
  }, []);

  // Initial load on mount
  useEffect(() => {
    loadAllMountPoints();
  }, [loadAllMountPoints]);

  // Subscribe to real-time SignalR events
  useEffect(() => {
    // Subscribe to mount point status changes (bulk updates)
    const unsubscribeStatus = signalRService.onMountPointStatusChanged(
      (statusUpdate: {
        mountPointId: number;
        mountPointName: string;
        activeSourceCount: number;
        activeClientCount: number;
      }) => {
        setMountPoints((prevMountPoints) =>
          prevMountPoints.map((mp) =>
            mp.id === statusUpdate.mountPointId
              ? {
                  ...mp,
                  activeSourceCount: statusUpdate.activeSourceCount,
                  activeClientCount: statusUpdate.activeClientCount,
                }
              : mp
          )
        );
      }
    );

    // Subscribe to source connected events (granular updates)
    const unsubscribeSourceConnected = signalRService.onSourceConnected((event) => {
      setMountPoints((prevMountPoints) =>
        prevMountPoints.map((mp) =>
          mp.id === event.mountPointId
            ? {
                ...mp,
                activeSourceCount: 1,
                // Update coordinates if provided in event
                ...(event.latitude && event.longitude
                  ? {
                      rtcmLatitude: event.latitude,
                      rtcmLongitude: event.longitude,
                    }
                  : {}),
              }
            : mp
        )
      );
    });

    // Subscribe to source disconnected events (granular updates)
    const unsubscribeSourceDisconnected = signalRService.onSourceDisconnected((event) => {
      setMountPoints((prevMountPoints) =>
        prevMountPoints.map((mp) =>
          mp.id === event.mountPointId
            ? {
                ...mp,
                activeSourceCount: 0,
              }
            : mp
        )
      );
    });

    // Cleanup subscriptions on unmount
    return () => {
      unsubscribeStatus();
      unsubscribeSourceConnected();
      unsubscribeSourceDisconnected();
    };
  }, []);

  return (
    <MountPointsContext.Provider
      value={{
        mountPoints,
        loading,
        error,
        refreshMountPoints: loadAllMountPoints,
      }}
    >
      {children}
    </MountPointsContext.Provider>
  );
};
