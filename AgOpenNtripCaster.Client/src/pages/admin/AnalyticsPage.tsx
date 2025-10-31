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

  const mockStats = stats || {
    totalConnections: 0,
    totalDataTransferred: 0,
    averageSessionDuration: 'N/A',
    peakConnectionTime: 'N/A',
  };

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
        <div className={styles.statsGrid}>
          <div className={styles.statCard}>
            <div className={styles.statIcon}>🔗</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Total Connections</div>
              <div className={styles.statValue}>{mockStats.totalConnections}</div>
            </div>
          </div>

          <div className={styles.statCard}>
            <div className={styles.statIcon}>📊</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Data Transferred</div>
              <div className={styles.statValue}>{mockStats.totalDataTransferred} MB</div>
            </div>
          </div>

          <div className={styles.statCard}>
            <div className={styles.statIcon}>⏱️</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Avg Session Duration</div>
              <div className={styles.statValue}>{mockStats.averageSessionDuration}</div>
            </div>
          </div>

          <div className={styles.statCard}>
            <div className={styles.statIcon}>🔝</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Peak Connection Time</div>
              <div className={styles.statValue}>{mockStats.peakConnectionTime}</div>
            </div>
          </div>
        </div>

        {/* Reports Section */}
        <div className={styles.reportsGrid}>
          <div className={styles.reportCard}>
            <h3 className={styles.reportTitle}>📈 Connection Trends</h3>
            <p className={styles.reportDescription}>
              Visualize client and source connection patterns over time
            </p>
            <div className={styles.chartPlaceholder}>
              Chart will display here
            </div>
            <button className={styles.exportButton}>📥 Export as CSV</button>
          </div>

          <div className={styles.reportCard}>
            <h3 className={styles.reportTitle}>📊 Data Transfer Analysis</h3>
            <p className={styles.reportDescription}>
              Monitor total bytes transferred by clients and sources
            </p>
            <div className={styles.chartPlaceholder}>
              Chart will display here
            </div>
            <button className={styles.exportButton}>📥 Export as CSV</button>
          </div>

          <div className={styles.reportCard}>
            <h3 className={styles.reportTitle}>🎯 User Activity Report</h3>
            <p className={styles.reportDescription}>
              Detailed breakdown of user activity and session metrics
            </p>
            <div className={styles.chartPlaceholder}>
              Chart will display here
            </div>
            <button className={styles.exportButton}>📥 Export as CSV</button>
          </div>

          <div className={styles.reportCard}>
            <h3 className={styles.reportTitle}>⚡ Performance Metrics</h3>
            <p className={styles.reportDescription}>
              System performance and resource utilization statistics
            </p>
            <div className={styles.chartPlaceholder}>
              Chart will display here
            </div>
            <button className={styles.exportButton}>📥 Export as CSV</button>
          </div>
        </div>

        {/* Advanced Filters */}
        <div className={styles.advancedSection}>
          <h2 className={styles.sectionTitle}>🔍 Advanced Filters</h2>
          <div className={styles.filterOptions}>
            <div className={styles.filterGroup}>
              <label>Mount Point:</label>
              <select className={styles.filterSelect}>
                <option>All Mount Points</option>
                <option>Mount Point 1</option>
                <option>Mount Point 2</option>
              </select>
            </div>
            <div className={styles.filterGroup}>
              <label>User:</label>
              <select className={styles.filterSelect}>
                <option>All Users</option>
                <option>User 1</option>
                <option>User 2</option>
              </select>
            </div>
            <div className={styles.filterGroup}>
              <label>Group:</label>
              <select className={styles.filterSelect}>
                <option>All Groups</option>
                <option>Group 1</option>
                <option>Group 2</option>
              </select>
            </div>
          </div>
        </div>
      </div>
    </DashboardLayout>
  );
};

export default AnalyticsPage;
