import React, { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import styles from './AnalyticsPage.module.css';
import { analyticsApi, type Analytics } from '../../services/analyticsApi';

export const AnalyticsPage: React.FC = () => {
  const [dateRange, setDateRange] = useState('7d');
  const [stats, setStats] = useState<Analytics | null>(null);

  useEffect(() => {
    const loadAnalytics = async () => {
      try {
        const data = await analyticsApi.getOverview(dateRange);
        setStats(data);
      } catch (err) {
        console.error('Failed to load analytics:', err);
      }
    };

    loadAnalytics();
  }, [dateRange]);


  return (
    <DashboardLayout>
      <div className={styles.container}>
        <div className={styles.header}>
          <h1 className={styles.title}>Analytics & Reports</h1>
          <p className={styles.subtitle}>System performance and usage statistics</p>
        </div>

        {/* Date Range Selector */}
        <div className={styles.filterBar}>
          <select
            value={dateRange}
            onChange={(e) => setDateRange(e.target.value)}
            className={styles.dateRangeSelect}
          >
            <option value="24h">Last 24 Hours</option>
            <option value="7d">Last 7 Days</option>
            <option value="30d">Last 30 Days</option>
            <option value="90d">Last 90 Days</option>
            <option value="1y">Last Year</option>
          </select>
        </div>

        {/* Stats Overview */}
        {stats ? (
          <div className={styles.statsGrid}>
            <div className={styles.statCard}>
              <div className={styles.statIcon}>🔗</div>
              <div className={styles.statContent}>
                <div className={styles.statLabel}>Total Connections</div>
                <div className={styles.statValue}>{stats.totalConnections || 0}</div>
              </div>
            </div>

            <div className={styles.statCard}>
              <div className={styles.statIcon}>📊</div>
              <div className={styles.statContent}>
                <div className={styles.statLabel}>Data Transferred</div>
                <div className={styles.statValue}>{stats.totalDataTransferred || 0} MB</div>
              </div>
            </div>

            <div className={styles.statCard}>
              <div className={styles.statIcon}>⏱️</div>
              <div className={styles.statContent}>
                <div className={styles.statLabel}>Avg Session Duration</div>
                <div className={styles.statValue}>{stats.averageSessionDuration || 'N/A'}</div>
              </div>
            </div>

            <div className={styles.statCard}>
              <div className={styles.statIcon}>🔝</div>
              <div className={styles.statContent}>
                <div className={styles.statLabel}>Peak Connection Time</div>
                <div className={styles.statValue}>{stats.peakConnectionTime || 'N/A'}</div>
              </div>
            </div>
          </div>
        ) : null}

      </div>
    </DashboardLayout>
  );
};

export default AnalyticsPage;
