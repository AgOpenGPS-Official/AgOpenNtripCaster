import { useEffect, useState } from 'react';
import { signalRService } from '../services/signalRService';
import type { ActivityEvent } from '../services/signalRService';
import { activityApi, type ActivityDto } from '../services/activityApi';

// User activity event types - activities relevant to the admin dashboard
const USER_ACTIVITY_TYPES = [
  'UserLogin',
  'UserLogout',
  'UserCreated',
  'UserUpdated',
  'UserDeleted',
  'MountPointCreated',
  'MountPointUpdated',
  'MountPointDeleted',
  'GroupCreated',
  'GroupUpdated',
  'GroupDeleted',
  'PermissionsChanged',
  'GroupPermissionGranted',
  'GroupPermissionRevoked',
  'ConfigurationChanged',
  'ClientConnected',
  'ClientDisconnected',
  'SourceConnected',
  'SourceDisconnected',
];

export interface UseUserActivityResult {
  userActivities: ActivityEvent[];
  isLoading: boolean;
}

/**
 * Custom hook for real-time user activity events via SignalR
 *
 * - On mount: Loads initial recent user activities via REST API
 * - Then: Subscribes to ActivityCreated events for new real-time updates
 * - Filters activity feed to show only user-initiated activities
 * - (login/logout, user/group/mount point management, configuration changes)
 *
 * Usage:
 * ```tsx
 * const { userActivities, isLoading } = useUserActivity();
 *
 * return (
 *   <div>
 *     {isLoading ? <p>Loading...</p> : null}
 *     {userActivities.map((activity) => (
 *       <div key={activity.id}>{activity.description}</div>
 *     ))}
 *   </div>
 * );
 * ```
 */
export function useUserActivity(): UseUserActivityResult {
  const [userActivities, setUserActivities] = useState<ActivityEvent[]>([]);
  const [isLoading, setIsLoading] = useState(true);

  useEffect(() => {
    let isMounted = true;

    const initializeActivities = async () => {
      try {
        // First: Load initial recent activities via REST API
        const initialActivities = await activityApi.getRecentActivities(50);

        // Filter for user-relevant activities
        const filteredActivities = initialActivities.filter((activity: ActivityDto) =>
          USER_ACTIVITY_TYPES.includes(activity.type)
        );

        if (isMounted) {
          setUserActivities(filteredActivities as unknown as ActivityEvent[]);
          setIsLoading(false);
        }
      } catch (error) {
        console.error('Failed to load initial user activities:', error);
        if (isMounted) {
          setIsLoading(false);
        }
      }
    };

    // Load initial activities
    initializeActivities();

    // Subscribe to real-time activity events
    const unsubscribeActivity = signalRService.onActivityCreated((activity: ActivityEvent) => {
      if (isMounted && USER_ACTIVITY_TYPES.includes(activity.type)) {
        // Add new user activity to the beginning of the list (newest first)
        setUserActivities((prevActivities) => {
          // Check if activity already exists (deduplication)
          if (prevActivities.some((a) => a.id === activity.id)) {
            return prevActivities;
          }
          return [activity, ...prevActivities].slice(0, 50); // Keep max 50 activities
        });
      }
    });

    // Cleanup subscription on unmount
    return () => {
      isMounted = false;
      unsubscribeActivity();
    };
  }, []);

  return {
    userActivities,
    isLoading,
  };
}
