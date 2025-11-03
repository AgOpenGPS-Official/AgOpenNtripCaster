import { useEffect, useState } from 'react';
import { signalRService } from '../services/signalRService';
import type { ActivityEvent } from '../services/signalRService';

// User activity event types
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
];

export interface UseUserActivityResult {
  userActivities: ActivityEvent[];
  isLoading: boolean;
}

/**
 * Custom hook for real-time user activity events via SignalR
 *
 * Filters activity feed to show only user-initiated activities
 * (login/logout, user management, group management, configuration changes)
 *
 * Usage:
 * ```tsx
 * const { userActivities } = useUserActivity();
 *
 * return (
 *   <div>
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

    // Subscribe to activity events
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
        if (isLoading) {
          setIsLoading(false);
        }
      }
    });

    // Set initial loading state
    setTimeout(() => {
      if (isMounted && isLoading) {
        setIsLoading(false);
      }
    }, 2000);

    // Cleanup subscription on unmount
    return () => {
      isMounted = false;
      unsubscribeActivity();
    };
  }, [isLoading]);

  return {
    userActivities,
    isLoading,
  };
}
