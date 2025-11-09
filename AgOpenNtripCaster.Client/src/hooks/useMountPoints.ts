import { useMountPointsContext } from '../contexts/MountPointsContext';
import type { MountPointDto } from '../types';

export interface UseMountPointsResult {
  mountPoints: MountPointDto[];
  loading: boolean;
  error: Error | null;
  refresh: () => Promise<void>;
}

/**
 * Custom hook for real-time mount points with live source/client counts
 *
 * - Uses MountPointsContext for persistent state across pages
 * - Data persists during navigation (no reload needed)
 * - Real-time updates via SignalR events
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
  const { mountPoints, loading, error, refreshMountPoints } = useMountPointsContext();

  return {
    mountPoints,
    loading,
    error,
    refresh: refreshMountPoints,
  };
}
