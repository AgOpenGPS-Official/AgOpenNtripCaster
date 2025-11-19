import React, { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import styles from './PerformancePage.module.css';
import { performanceApi } from '../../services/performanceApi';
import type { PerformanceMetricsDto } from '../../services/performanceApi';

export const PerformancePage: React.FC = () => {
  const [metrics, setMetrics] = useState<PerformanceMetricsDto | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  const fetchMetrics = async () => {
    try {
      setLoading(true);
      const data = await performanceApi.getCurrentMetrics();
      setMetrics(data);
      setError(null);
    } catch (err: any) {
      setError(err.response?.data?.message || 'Failed to load performance metrics');
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchMetrics();

    // Refresh metrics every 10 seconds
    const interval = setInterval(fetchMetrics, 10000);
    return () => clearInterval(interval);
  }, []);

  if (loading && !metrics) {
    return (
      <DashboardLayout>
        <div className={styles.container}>
          <h1>Performance Metrics</h1>
          <p>Loading...</p>
        </div>
      </DashboardLayout>
    );
  }

  if (error) {
    return (
      <DashboardLayout>
        <div className={styles.container}>
          <h1>Performance Metrics</h1>
          <div className={styles.error}>{error}</div>
          <button onClick={fetchMetrics} className={styles.retryButton}>
            Retry
          </button>
        </div>
      </DashboardLayout>
    );
  }

  if (!metrics) return null;

  const formatBytes = (bytes: number): string => {
    if (bytes === 0) return '0 B';
    const k = 1024;
    const sizes = ['B', 'KB', 'MB', 'GB', 'TB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return `${(bytes / Math.pow(k, i)).toFixed(2)} ${sizes[i]}`;
  };

  const formatTime = (ms: number): string => {
    return `${ms.toFixed(2)} ms`;
  };

  return (
    <DashboardLayout>
      <div className={styles.container}>
        <div className={styles.header}>
          <h1>Zero-Copy Performance Metrics 🚀</h1>
          <p className={styles.subtitle}>Real-time broadcasting performance statistics</p>
        </div>

        {/* Zero-Copy Metrics */}
        <div className={styles.section}>
          <h2>Zero-Copy Broadcasting</h2>
          <div className={styles.statsGrid}>
            <div className={styles.statCard}>
              <div className={styles.statLabel}>Memory Saved</div>
              <div className={styles.statValue}>{metrics.memorySavedMB.toFixed(2)} MB</div>
              <div className={styles.statSubtext}>{formatBytes(metrics.memorySavedBytes)}</div>
            </div>

            <div className={styles.statCard}>
              <div className={styles.statLabel}>Active Buffers</div>
              <div className={styles.statValue}>{metrics.activeZeroCopyBuffers}</div>
              <div className={styles.statSubtext}>Peak: {metrics.peakZeroCopyBuffers}</div>
            </div>

            <div className={styles.statCard}>
              <div className={styles.statLabel}>Total Broadcasts</div>
              <div className={styles.statValue}>{metrics.totalBroadcasts.toLocaleString()}</div>
              <div className={styles.statSubtext}>Since server start</div>
            </div>

            <div className={styles.statCard}>
              <div className={styles.statLabel}>Total Data Sent</div>
              <div className={styles.statValue}>{formatBytes(metrics.totalBytesSent)}</div>
              <div className={styles.statSubtext}>Across all clients</div>
            </div>
          </div>
        </div>

        {/* Broadcast Performance */}
        <div className={styles.section}>
          <h2>Broadcast Performance</h2>
          <div className={styles.statsGrid}>
            <div className={styles.statCard}>
              <div className={styles.statLabel}>Average Broadcast Time</div>
              <div className={styles.statValue}>{formatTime(metrics.averageBroadcastTimeMs)}</div>
              <div className={styles.statSubtext}>Per broadcast operation</div>
            </div>

            <div className={styles.statCard}>
              <div className={styles.statLabel}>Peak Broadcast Time</div>
              <div className={styles.statValue}>{formatTime(metrics.peakBroadcastTimeMs)}</div>
              <div className={styles.statSubtext}>Slowest broadcast</div>
            </div>
          </div>
        </div>

        {/* Client/Source Statistics */}
        <div className={styles.section}>
          <h2>Active Connections</h2>
          <div className={styles.statsGrid}>
            <div className={styles.statCard}>
              <div className={styles.statLabel}>Active Clients</div>
              <div className={styles.statValue}>{metrics.activeClients}</div>
              <div className={styles.statSubtext}>Connected clients</div>
            </div>

            <div className={styles.statCard}>
              <div className={styles.statLabel}>Active Sources</div>
              <div className={styles.statValue}>{metrics.activeSources}</div>
              <div className={styles.statSubtext}>Broadcasting sources</div>
            </div>
          </div>
        </div>

        {/* Memory Statistics */}
        <div className={styles.section}>
          <h2>System Memory</h2>
          <div className={styles.statsGrid}>
            <div className={styles.statCard}>
              <div className={styles.statLabel}>Total Memory Usage</div>
              <div className={styles.statValue}>{metrics.totalMemoryUsageMB.toFixed(2)} MB</div>
              <div className={styles.statSubtext}>{formatBytes(metrics.totalMemoryUsageBytes)}</div>
            </div>

            <div className={styles.statCard}>
              <div className={styles.statLabel}>GC Collections</div>
              <div className={styles.statValue}>{metrics.gcCollectionCount.toLocaleString()}</div>
              <div className={styles.statSubtext}>Total garbage collections</div>
            </div>
          </div>
        </div>

        <div className={styles.footer}>
          <p>Last updated: {new Date(metrics.timestamp).toLocaleTimeString()}</p>
          <p className={styles.autoRefresh}>Auto-refreshing every 10 seconds</p>
        </div>
      </div>
    </DashboardLayout>
  );
};

export default PerformancePage;
