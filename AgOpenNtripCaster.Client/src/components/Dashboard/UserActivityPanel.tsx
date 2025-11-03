import React from 'react';
import { useUserActivity } from '../../hooks/useUserActivity';
import styles from './UserActivityPanel.module.css';

/**
 * UserActivityPanel displays real-time user activity events
 * including logins, logouts, user management, and configuration changes
 */
export const UserActivityPanel: React.FC = () => {
  const { userActivities, isLoading } = useUserActivity();

  const getActivityIcon = (type: string): string => {
    switch (type) {
      case 'UserLogin':
        return '🔓';
      case 'UserLogout':
        return '🔒';
      case 'UserCreated':
      case 'UserUpdated':
      case 'UserDeleted':
        return '👤';
      case 'GroupCreated':
      case 'GroupUpdated':
      case 'GroupDeleted':
        return '👥';
      case 'MountPointCreated':
      case 'MountPointUpdated':
      case 'MountPointDeleted':
        return '📡';
      case 'PermissionsChanged':
      case 'GroupPermissionGranted':
      case 'GroupPermissionRevoked':
        return '🔐';
      case 'ConfigurationChanged':
        return '⚙️';
      default:
        return '📋';
    }
  };

  const getActivityBadgeClass = (type: string): string => {
    if (type.includes('Login') || type.includes('Granted')) {
      return styles.success;
    }
    if (type.includes('Logout') || type.includes('Deleted') || type.includes('Revoked')) {
      return styles.warning;
    }
    if (type.includes('Created') || type.includes('Updated')) {
      return styles.info;
    }
    return styles.default;
  };

  if (isLoading) {
    return (
      <div className={styles.container}>
        <h2 className={styles.title}>User Activity</h2>
        <div className={styles.loadingMessage}>Loading user activities...</div>
      </div>
    );
  }

  return (
    <div className={styles.container}>
      <h2 className={styles.title}>User Activity</h2>

      {userActivities.length === 0 ? (
        <div className={styles.emptyState}>
          <p>No recent user activities</p>
        </div>
      ) : (
        <div className={styles.activityList}>
          {userActivities.map((activity) => (
            <div key={activity.id} className={styles.activityItem}>
              <div className={styles.activityHeader}>
                <span className={styles.activityIcon}>{getActivityIcon(activity.type)}</span>
                <span className={`${styles.badge} ${getActivityBadgeClass(activity.type)}`}>
                  {activity.type.replace(/([A-Z])/g, ' $1').trim()}
                </span>
                <span className={styles.timestamp}>
                  {new Date(activity.createdAt).toLocaleTimeString()}
                </span>
              </div>
              <div className={styles.activityDescription}>{activity.description}</div>
              {activity.userName && (
                <div className={styles.activityUser}>
                  <span className={styles.userLabel}>User:</span>
                  <span className={styles.userName}>{activity.userName}</span>
                </div>
              )}
            </div>
          ))}
        </div>
      )}
    </div>
  );
};

export default UserActivityPanel;
