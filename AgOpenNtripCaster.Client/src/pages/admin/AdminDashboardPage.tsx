import React, { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import ConnectionStatsPanel from '../../components/Dashboard/ConnectionStatsPanel';
import UserActivityPanel from '../../components/Dashboard/UserActivityPanel';
import styles from './AdminDashboardPage.module.css';
import { dashboardStatsApi } from '../../services/dashboardStatsApi';
import { activityApi, type ActivityDto } from '../../services/activityApi';

interface DashboardStats {
  activeClients: number;
  activeSources: number;
  totalBytesReceived: number;
  totalBytesSent: number;
  totalBytesTransferred: number;
  uptimeFormatted: string;
}

export const AdminDashboardPage: React.FC = () => {
  const [stats, setStats] = useState<DashboardStats | null>(null);
  const [activities, setActivities] = useState<ActivityDto[]>([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const loadDashboardData = async () => {
      try {
        const [statsData, activitiesData] = await Promise.all([
          dashboardStatsApi.getDashboardStats(),
          activityApi.getRecentActivities(10),
        ]);

        setStats(statsData);
        setActivities(activitiesData);
      } catch (err) {
        console.error('Failed to load admin dashboard data:', err);
      } finally {
        setLoading(false);
      }
    };

    loadDashboardData();
    const interval = setInterval(loadDashboardData, 5000);

    return () => clearInterval(interval);
  }, []);

  if (loading) {
    return (
      <DashboardLayout>
        <div className={styles.loading}>
          <div className={styles.spinner}></div>
          <p>Loading admin dashboard...</p>
        </div>
      </DashboardLayout>
    );
  }

  return (
    <DashboardLayout>
      <div className={styles.container}>
        <div className={styles.header}>
          <h1 className={styles.title}>Admin Dashboard</h1>
          <p className={styles.subtitle}>System overview and management center</p>
        </div>

        {/* Stats Cards */}
        <div className={styles.statsGrid}>
          <div className={styles.statCard}>
            <div className={styles.statIcon}>🛰️</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Connected Rovers</div>
              <div className={styles.statValue}>{stats?.activeClients ?? 0}</div>
              <div className={styles.statSubtext}>NTRIP clients</div>
            </div>
          </div>

          <div className={styles.statCard}>
            <div className={styles.statIcon}>📡</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Base Stations</div>
              <div className={styles.statValue}>{stats?.activeSources ?? 0}</div>
              <div className={styles.statSubtext}>Data sources</div>
            </div>
          </div>

          <div className={styles.statCard}>
            <div className={styles.statIcon}>📊</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Data Transferred</div>
              <div className={styles.statValue}>{stats?.totalBytesTransferred.toFixed(2) ?? 0} MB</div>
              <div className={styles.statSubtext}>Total session traffic</div>
            </div>
          </div>

          <div className={styles.statCard}>
            <div className={styles.statIcon}>⏱️</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Server Uptime</div>
              <div className={styles.statValue}>{stats?.uptimeFormatted ?? 'N/A'}</div>
              <div className={styles.statSubtext}>Since last restart</div>
            </div>
          </div>
        </div>

        {/* Real-time Connection Statistics */}
        <ConnectionStatsPanel />

        {/* User Activity Events */}
        <UserActivityPanel />

        {/* Admin Sections Grid */}
        <div className={styles.sectionsGrid}>
          <div className={styles.section}>
            <h2 className={styles.sectionTitle}>👥 User & Group Management</h2>
            <div className={styles.actionButtons}>
              <a href="/admin/users" className={styles.actionButton}>
                Manage Users
              </a>
              <a href="/admin/groups" className={styles.actionButton}>
                Manage Groups
              </a>
            </div>
          </div>

          <div className={styles.section}>
            <h2 className={styles.sectionTitle}>🔌 Network Configuration</h2>
            <div className={styles.actionButtons}>
              <a href="/admin/mountpoints" className={styles.actionButton}>
                Mount Points
              </a>
              <a href="/admin/caster-config" className={styles.actionButton}>
                Caster Config
              </a>
              <a href="/admin/network-config" className={styles.actionButton}>
                Network Config
              </a>
            </div>
          </div>

          <div className={styles.section}>
            <h2 className={styles.sectionTitle}>📊 Monitoring & Analytics</h2>
            <div className={styles.actionButtons}>
              <a href="/admin/realtime-map" className={styles.actionButton}>
                Real-time Map
              </a>
              <a href="/admin/analytics" className={styles.actionButton}>
                Analytics & Reports
              </a>
              <a href="/admin/activity-log" className={styles.actionButton}>
                Activity Log
              </a>
            </div>
          </div>

          <div className={styles.section}>
            <h2 className={styles.sectionTitle}>⚙️ System Settings</h2>
            <div className={styles.actionButtons}>
              <a href="/admin/system-settings" className={styles.actionButton}>
                System Settings
              </a>
              <a href="/admin/security" className={styles.actionButton}>
                Security Settings
              </a>
              <a href="/admin/logs" className={styles.actionButton}>
                System Logs
              </a>
            </div>
          </div>

          <div className={styles.section}>
            <h2 className={styles.sectionTitle}>🗄️ Database Management</h2>
            <div className={styles.actionButtons}>
              <a href="/admin/database" className={styles.actionButton}>
                Database Tools
              </a>
            </div>
          </div>
        </div>

        {/* Recent Activity */}
        <div className={styles.recentActivity}>
          <h2 className={styles.sectionTitle}>📋 Recent Activity</h2>
          <div className={styles.activityList}>
            {activities.length === 0 ? (
              <div className={styles.emptyState}>
                <p>No recent activity</p>
              </div>
            ) : (
              <table className={styles.activityTable}>
                <tbody>
                  {activities.map((activity) => (
                    <tr key={activity.id} className={styles.activityRow}>
                      <td className={styles.activityTime}>
                        {new Date(activity.createdAt).toLocaleTimeString()}
                      </td>
                      <td className={styles.activityType}>
                        <span className={`${styles.badge} ${styles[activity.type.toLowerCase()]}`}>
                          {activity.type === 'SourceConnected' && '📡 Source'}
                          {activity.type === 'SourceDisconnected' && '📡 Source'}
                          {activity.type === 'ClientConnected' && '🛰️ Client'}
                          {activity.type === 'ClientDisconnected' && '🛰️ Client'}
                        </span>
                      </td>
                      <td className={styles.activityDescription}>{activity.description}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            )}
          </div>
        </div>
      </div>
    </DashboardLayout>
  );
};

export default AdminDashboardPage;
