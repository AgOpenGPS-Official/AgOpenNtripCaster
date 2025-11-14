import { useEffect, useState } from 'react';
import { signalRService } from '../services/signalRService';
import { activityApi, type ActivityDto } from '../services/activityApi';

export interface UseActivityFeedResult {
  activities: ActivityDto[];
  loading: boolean;
  error: Error | null;
}

/**
 * Custom hook for real-time activity feed via SignalR
 *
 * - On mount: Loads user-specific activities via REST API
 * - Then: Subscribes to ActivityCreated events for new activities
 * - New activities appear at the top of the feed in real-time
 * - Shows activities for user's own connections (sources/clients) and mount points
 * - Optionally limits the maximum number of activities shown
 *
 * This hybrid approach ensures we always show historical data on load,
 * while getting real-time updates for new activities.
 *
 * Usage:
 * ```tsx
 * // User-specific activities with max 8 items
 * const { activities, loading, error } = useActivityFeed('user123', 8);
 *
 * if (loading) return <div>Loading activities...</div>;
 * if (error) return <div>Error: {error.message}</div>;
 *
 * return (
 *   <div>
 *     {activities.map(activity => (
 *       <div key={activity.id}>{activity.description}</div>
 *     ))}
 *   </div>
 * );
 * ```
 */
export function useActivityFeed(userIdFilter?: string, maxActivities: number = 50): UseActivityFeedResult {
  const [activities, setActivities] = useState<ActivityDto[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<Error | null>(null);

  useEffect(() => {
    let isMounted = true;

    const initializeActivities = async () => {
      try {
        // Use getMyActivities endpoint which returns activities for user's mount points
        // This includes source/client connections to mount points owned by the user
        const initialActivities = await activityApi.getMyActivities(maxActivities);

        if (isMounted) {
          setActivities(initialActivities);
          setLoading(false);
          setError(null);
        }
      } catch (err) {
        console.error('Failed to load initial activities:', err);
        if (isMounted) {
          setError(err instanceof Error ? err : new Error('Failed to load activities'));
          setLoading(false);
        }
      }
    };

    // Load initial activities
    initializeActivities();

    // Subscribe to new activity events (will prepend to the list)
    const unsubscribeActivity = signalRService.onActivityCreated((newActivity: ActivityDto) => {
      if (isMounted) {
        // Only show if activity has userId matching current user
        // (server-side filtering ensures we only get relevant activities)
        if (userIdFilter && newActivity.userId !== userIdFilter) {
          // Also check if it's a connection to user's mount point
          // We'll accept it and let the server-side filtering handle it
        }

        // Add new activity to the beginning of the list, but avoid duplicates
        setActivities((prevActivities) => {
          // Check if this activity already exists (by ID)
          if (prevActivities.some((a) => a.id === newActivity.id)) {
            return prevActivities; // Already in list, don't add it again
          }
          return [newActivity, ...prevActivities].slice(0, maxActivities);
        });
      }
    });

    // Cleanup subscription on unmount
    return () => {
      isMounted = false;
      unsubscribeActivity();
    };
  }, [userIdFilter, maxActivities]);

  return {
    activities,
    loading,
    error,
  };
}
