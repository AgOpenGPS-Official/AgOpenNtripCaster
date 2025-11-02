import { useEffect, useState } from 'react';
import { signalRService } from '../services/signalRService';
import { mountPointsApi } from '../services/mountPointsApi';
import type { MountPointDto } from '../types';

export interface UseMountPointsResult {
  mountPoints: MountPointDto[];
  loading: boolean;
  error: Error | null;
}

/**
 * Custom hook for real-time mount points with live source/client counts
 *
 * - On mount: Loads initial mount points via REST API
 * - Then: Subscribes to MountPointStatusChanged events for live updates
 * - Updates activeSourceCount and activeClientCount in real-time
 *
 * Usage:
 * ```tsx
 * const { mountPoints, loading, error } = useMountPoints();
 *
 * if (loading) return <div>Loading...</div>;
 * if (error) return <div>Error: {error.message}</div>;
 *
 * return (
 *   <div>
 *     {mountPoints.map(mp => (
 *       <div key={mp.id}>
 *         {mp.name}: {mp.activeSourceCount} sources, {mp.activeClientCount} clients
 *       </div>
 *     ))}
 *   </div>
 * );
 * ```
 */
export function useMountPoints(): UseMountPointsResult {
  const [mountPoints, setMountPoints] = useState<MountPointDto[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<Error | null>(null);

  useEffect(() => {
    let isMounted = true;

    const initializeMountPoints = async () => {
      try {
        // Load initial mount points (get all pages)
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

        if (isMounted) {
          setMountPoints(allMountPoints);
          setLoading(false);
          setError(null);
        }
      } catch (err) {
        console.error('Failed to load mount points:', err);
        if (isMounted) {
          setError(err instanceof Error ? err : new Error('Failed to load mount points'));
          setLoading(false);
        }
      }
    };

    // Load initial mount points
    initializeMountPoints();

    // Subscribe to mount point status changes
    const unsubscribeStatus = signalRService.onMountPointStatusChanged(
      (statusUpdate: {
        mountPointId: number;
        mountPointName: string;
        activeSourceCount: number;
        activeClientCount: number;
      }) => {
        if (isMounted) {
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
      }
    );

    // Cleanup subscription on unmount
    return () => {
      isMounted = false;
      unsubscribeStatus();
    };
  }, []);

  return {
    mountPoints,
    loading,
    error,
  };
}
