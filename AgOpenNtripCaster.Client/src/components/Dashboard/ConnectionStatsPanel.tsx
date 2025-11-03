import React from 'react';
import { useConnectionStats } from '../../hooks/useConnectionStats';
import StatsCard from './StatsCard';
import styles from './ConnectionStatsPanel.module.css';

/**
 * ConnectionStatsPanel displays real-time connection statistics
 * including throughput, latency, CPU and memory usage via SignalR
 */
export const ConnectionStatsPanel: React.FC = () => {
  const { stats, isLoading } = useConnectionStats();

  if (isLoading) {
    return (
      <div className={styles.container}>
        <h2 className={styles.title}>Connection Statistics</h2>
        <div className={styles.loadingMessage}>Loading statistics...</div>
      </div>
    );
  }

  if (!stats) {
    return (
      <div className={styles.container}>
        <h2 className={styles.title}>Connection Statistics</h2>
        <div className={styles.errorMessage}>Unable to load connection statistics</div>
      </div>
    );
  }

  // Format throughput as Mbps
  const throughputDisplay = stats.throughputMbps > 0
    ? `${stats.throughputMbps.toFixed(2)} Mbps`
    : 'N/A';

  // Format latency
  const latencyDisplay = stats.averageLatencyMs > 0
    ? `${stats.averageLatencyMs.toFixed(0)} ms`
    : 'N/A';

  return (
    <div className={styles.container}>
      <h2 className={styles.title}>Connection Statistics</h2>

      <div className={styles.statsGrid}>
        <StatsCard
          title="Total Connections"
          value={stats.totalConnections}
          icon="🔗"
          color="blue"
        />

        <StatsCard
          title="Active Clients"
          value={stats.activeClientCount}
          icon="👥"
          color="green"
        />

        <StatsCard
          title="Active Sources"
          value={stats.activeSourceCount}
          icon="📡"
          color="orange"
        />

        <StatsCard
          title="Throughput"
          value={throughputDisplay}
          icon="⚡"
          color="blue"
        />

        <StatsCard
          title="Download"
          value={`${stats.downloadMbps.toFixed(2)} Mbps`}
          icon="📥"
          color="green"
        />

        <StatsCard
          title="Upload"
          value={`${stats.uploadMbps.toFixed(2)} Mbps`}
          icon="📤"
          color="orange"
        />

        <StatsCard
          title="Average Latency"
          value={latencyDisplay}
          icon="⏱️"
          color="blue"
        />

        <StatsCard
          title="CPU Usage"
          value={`${stats.cpuUsagePercent.toFixed(1)}%`}
          icon="⚙️"
          trend={stats.cpuUsagePercent > 70 ? 'up' : 'stable'}
          color={stats.cpuUsagePercent > 80 ? 'red' : stats.cpuUsagePercent > 60 ? 'orange' : 'green'}
        />

        <StatsCard
          title="Memory Usage"
          value={`${stats.memoryUsagePercent.toFixed(1)}%`}
          icon="💾"
          trend={stats.memoryUsagePercent > 70 ? 'up' : 'stable'}
          color={stats.memoryUsagePercent > 80 ? 'red' : stats.memoryUsagePercent > 60 ? 'orange' : 'green'}
        />
      </div>

      <div className={styles.footer}>
        <small>Updated: {new Date(stats.collectedAt).toLocaleTimeString()}</small>
      </div>
    </div>
  );
};

export default ConnectionStatsPanel;
