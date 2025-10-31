import React, { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import styles from './ActivityLogPage.module.css';
import { activityApi, type ActivityDto } from '../../services/activityApi';

export const ActivityLogPage: React.FC = () => {
  const [activities, setActivities] = useState<ActivityDto[]>([]);
  const [loading, setLoading] = useState(true);
  const [filter, setFilter] = useState<string>('all');

  useEffect(() => {
    const loadActivities = async () => {
      try {
        setLoading(true);
        const data = await activityApi.getRecentActivities(100);
        setActivities(data);
      } catch (err) {
        console.error('Failed to load activities:', err);
      } finally {
        setLoading(false);
      }
    };

    loadActivities();
    const interval = setInterval(loadActivities, 5000); // Refresh every 5 seconds
    return () => clearInterval(interval);
  }, []);

  const filteredActivities = activities.filter((activity) => {
    if (filter === 'all') return true;
    return activity.type === filter;
  });

  const activityTypes = [
    { value: 'all', label: 'All Activities' },
    { value: 'SourceConnected', label: 'Source Connected' },
    { value: 'SourceDisconnected', label: 'Source Disconnected' },
    { value: 'ClientConnected', label: 'Client Connected' },
    { value: 'ClientDisconnected', label: 'Client Disconnected' },
  ];

  return (
    <DashboardLayout>
      <div className={styles.container}>
        <div className={styles.header}>
          <h1 className={styles.title}>Activity Log</h1>
          <p className={styles.subtitle}>Complete history of system events and connections</p>
        </div>

        {/* Filters */}
        <div className={styles.filterBar}>
          <select
            value={filter}
            onChange={(e) => {
              setFilter(e.target.value);
            }}
            className={styles.filterSelect}
          >
            {activityTypes.map((type) => (
              <option key={type.value} value={type.value}>
                {type.label}
              </option>
            ))}
          </select>
          <div className={styles.resultCount}>
            Showing {filteredActivities.length} activities
          </div>
        </div>

        {/* Activity Table */}
        <div className={styles.tableSection}>
          {loading ? (
            <div className={styles.loading}>
              <div className={styles.spinner}></div>
              <p>Loading activities...</p>
            </div>
          ) : filteredActivities.length === 0 ? (
            <div className={styles.emptyState}>
              <p>No activities found</p>
            </div>
          ) : (
            <div className={styles.tableWrapper}>
              <table className={styles.table}>
                <thead>
                  <tr>
                    <th>Time</th>
                    <th>Type</th>
                    <th>Description</th>
                    <th>Details</th>
                  </tr>
                </thead>
                <tbody>
                  {filteredActivities.map((activity) => (
                    <tr key={activity.id} className={styles.row}>
                      <td className={styles.timeCell}>
                        <div className={styles.time}>
                          {new Date(activity.createdAt).toLocaleTimeString()}
                        </div>
                        <div className={styles.date}>
                          {new Date(activity.createdAt).toLocaleDateString()}
                        </div>
                      </td>
                      <td className={styles.typeCell}>
                        <span className={`${styles.badge} ${styles[activity.type.toLowerCase()]}`}>
                          {activity.type === 'SourceConnected' && '📡 Source Connected'}
                          {activity.type === 'SourceDisconnected' && '📡 Source Disconnected'}
                          {activity.type === 'ClientConnected' && '🛰️ Client Connected'}
                          {activity.type === 'ClientDisconnected' && '🛰️ Client Disconnected'}
                        </span>
                      </td>
                      <td className={styles.descriptionCell}>{activity.description}</td>
                      <td className={styles.detailsCell}>
                        <code className={styles.code}>{activity.id}</code>
                      </td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          )}
        </div>
      </div>
    </DashboardLayout>
  );
};

export default ActivityLogPage;
