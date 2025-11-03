import { useEffect, useState } from 'react';
import { signalRService } from '../services/signalRService';
import { activityApi, type ActivityDto } from '../services/activityApi';

// Activity types relevant to individual users
const USER_RELEVANT_ACTIVITY_TYPES = [
  'UserLogin',
  'UserLogout',
  'UserCreated',
  'UserUpdated',
  'UserDeleted',
  'MountPointCreated',
  'MountPointUpdated',
  'MountPointDeleted',
  'PermissionsChanged',
  'GroupPermissionGranted',
  'GroupPermissionRevoked',
];

// Filter activities for a specific user
const filterActivitiesForUser = (activities: ActivityDto[], userId: string): ActivityDto[] => {
  return activities.filter(activity => {
    // Check if activity type is relevant to users
    if (!USER_RELEVANT_ACTIVITY_TYPES.includes(activity.type)) {
      return false;
    }
    // Check if activity belongs to the user
    if (activity.userId === userId) {
      return true;
    }
    // For mount point activities, show if user created the mount point
    if (['MountPointCreated', 'MountPointUpdated', 'MountPointDeleted'].includes(activity.type)) {
      // If activity has userId, it's the user who performed the action
      return activity.userId === userId;
    }
    return false;
  });
};

export interface UseActivityFeedResult {
  activities: ActivityDto[];
  loading: boolean;
  error: Error | null;
}

/**
 * Custom hook for real-time activity feed via SignalR
 *
 * - On mount: Loads initial recent activities via REST API
 * - Then: Subscribes to ActivityCreated events for new activities
 * - New activities appear at the top of the feed in real-time
 * - Optionally filters activities by userId for user-specific feeds
 *
 * This hybrid approach ensures we always show historical data on load,
 * while getting real-time updates for new activities.
 *
 * Usage:
 * ```tsx
 * // All activities
 * const { activities, loading, error } = useActivityFeed();
 *
 * // Only user-specific activities
 * const { activities, loading, error } = useActivityFeed('user123');
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
export function useActivityFeed(userIdFilter?: string): UseActivityFeedResult {
  const [activities, setActivities] = useState<ActivityDto[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<Error | null>(null);

  useEffect(() => {
    let isMounted = true;

    const initializeActivities = async () => {
      try {
        // First: Load initial recent activities via REST API
        let initialActivities = await activityApi.getRecentActivities(50);

        // Apply filtering if userIdFilter is provided
        if (userIdFilter) {
          initialActivities = filterActivitiesForUser(initialActivities, userIdFilter);
        }

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
        // Apply filtering if userIdFilter is provided
        if (userIdFilter && !filterActivitiesForUser([newActivity], userIdFilter).length) {
          return; // Activity doesn't match filter, ignore it
        }

        // Add new activity to the beginning of the list, but avoid duplicates
        setActivities((prevActivities) => {
          // Check if this activity already exists (by ID)
          if (prevActivities.some((a) => a.id === newActivity.id)) {
            return prevActivities; // Already in list, don't add it again
          }
          return [newActivity, ...prevActivities].slice(0, 50);
        });
      }
    });

    // Cleanup subscription on unmount
    return () => {
      isMounted = false;
      unsubscribeActivity();
    };
  }, [userIdFilter]);

  return {
    activities,
    loading,
    error,
  };
}
