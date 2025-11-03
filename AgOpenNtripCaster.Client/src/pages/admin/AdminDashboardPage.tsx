import React from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import UserActivityPanel from '../../components/Dashboard/UserActivityPanel';
import styles from './AdminDashboardPage.module.css';
import { useDashboardStats } from '../../hooks/useDashboardStats';

export const AdminDashboardPage: React.FC = () => {
  // Get real-time dashboard stats via SignalR
  const { stats: signalRStats, connected: signalRConnected } = useDashboardStats();

  // Fallback stats when SignalR not available
  const stats = signalRStats ? {
    activeClients: signalRStats.activeClients,
    activeSources: signalRStats.activeSources,
    totalBytesReceived: signalRStats.totalBytesReceived,
    totalBytesSent: signalRStats.totalBytesSent,
    totalBytesTransferred: signalRStats.totalBytesTransferred,
    uptimeFormatted: signalRConnected ? signalRStats.uptimeFormatted : 'Server Offline',
  } : {
    activeClients: 0,
    activeSources: 0,
    totalBytesReceived: 0,
    totalBytesSent: 0,
    totalBytesTransferred: 0,
    uptimeFormatted: 'Waiting for server...',
  };

  return (
    <DashboardLayout>
      <div className={styles.container}>
        <div className={styles.header}>
          <h1 className={styles.title}>Admin Dashboard</h1>
          <p className={styles.subtitle}>System overview and management center</p>
        </div>

        {/* Stats Cards */}
        <div className={styles.statsGrid}>
          <a href="/admin/realtime-map" className={styles.statCard}>
            <div className={styles.statIcon}>🛰️</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Connected Rovers</div>
              <div className={styles.statValue}>{stats?.activeClients ?? 0}</div>
              <div className={styles.statSubtext}>NTRIP clients → View Map</div>
            </div>
          </a>

          <a href="/admin/mountpoints" className={styles.statCard}>
            <div className={styles.statIcon}>📡</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Base Stations</div>
              <div className={styles.statValue}>{stats?.activeSources ?? 0}</div>
              <div className={styles.statSubtext}>Data sources → Manage</div>
            </div>
          </a>

          <a href="/admin/analytics" className={styles.statCard}>
            <div className={styles.statIcon}>📊</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Data Transferred</div>
              <div className={styles.statValue}>{stats?.totalBytesTransferred.toFixed(2) ?? 0} MB</div>
              <div className={styles.statSubtext}>Total session traffic → Analytics</div>
            </div>
          </a>

          <a href="/admin/system-settings" className={styles.statCard}>
            <div className={styles.statIcon}>⏱️</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Server Uptime</div>
              <div className={styles.statValue}>{stats?.uptimeFormatted ?? 'N/A'}</div>
              <div className={styles.statSubtext}>Since last restart → Settings</div>
            </div>
          </a>
        </div>

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

      </div>
    </DashboardLayout>
  );
};

export default AdminDashboardPage;
