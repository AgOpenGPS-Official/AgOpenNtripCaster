import React, { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import styles from './SystemLogsPage.module.css';
import { logsApi, type SystemLog, type LogStatistics } from '../../services/logsApi';

export const SystemLogsPage: React.FC = () => {
  const [logLevel, setLogLevel] = useState('all');
  const [autoRefresh, setAutoRefresh] = useState(true);
  const [logs, setLogs] = useState<SystemLog[]>([]);
  const [stats, setStats] = useState<LogStatistics | null>(null);

  useEffect(() => {
    const loadLogs = async () => {
      try {
        const [logsData, statsData] = await Promise.all([
          logsApi.getLogs(logLevel === 'all' ? undefined : logLevel),
          logsApi.getStatistics(),
        ]);
        setLogs(logsData.logs);
        setStats(statsData);
      } catch (err) {
        console.error('Failed to load logs:', err);
      }
    };

    loadLogs();

    if (autoRefresh) {
      const interval = setInterval(loadLogs, 5000);
      return () => clearInterval(interval);
    }
  }, [logLevel, autoRefresh]);

  const filteredLogs = logs;

  const getLevelColor = (level: string) => {
    switch (level) {
      case 'ERROR':
        return styles.levelError;
      case 'WARNING':
        return styles.levelWarning;
      case 'INFO':
        return styles.levelInfo;
      case 'DEBUG':
        return styles.levelDebug;
      default:
        return '';
    }
  };

  return (
    <DashboardLayout>
      <div className={styles.container}>
        <div className={styles.header}>
          <h1 className={styles.title}>System Logs</h1>
          <p className={styles.subtitle}>View and monitor server activity logs</p>
        </div>

        {/* Controls */}
        <div className={styles.controlBar}>
          <div className={styles.filterGroup}>
            <label>Log Level:</label>
            <select
              value={logLevel}
              onChange={(e) => setLogLevel(e.target.value)}
              className={styles.select}
            >
              <option value="all">All Levels</option>
              <option value="DEBUG">Debug</option>
              <option value="INFO">Info</option>
              <option value="WARNING">Warning</option>
              <option value="ERROR">Error</option>
            </select>
          </div>

          <div className={styles.filterGroup}>
            <label>
              <input
                type="checkbox"
                checked={autoRefresh}
                onChange={(e) => setAutoRefresh(e.target.checked)}
                className={styles.checkbox}
              />
              Auto-refresh (5s)
            </label>
          </div>

          <button className={styles.downloadButton}>📥 Download Logs</button>
          <button className={styles.clearButton}>🗑️ Clear Logs</button>
        </div>

        {/* Logs Table */}
        <div className={styles.logsSection}>
          <div className={styles.logsHeader}>
            <span className={styles.logCount}>Showing {filteredLogs.length} logs</span>
          </div>

          <div className={styles.logsTable}>
            {filteredLogs.length === 0 ? (
              <div className={styles.emptyState}>
                <p>No logs found</p>
              </div>
            ) : (
              <div className={styles.logsList}>
                {filteredLogs.map((log) => (
                  <div key={log.id} className={styles.logEntry}>
                    <div className={styles.logTime}>
                      {new Date(log.timestamp).toLocaleTimeString()}
                    </div>
                    <div className={`${styles.logLevel} ${getLevelColor(log.level)}`}>
                      {log.level}
                    </div>
                    <div className={styles.logMessage}>{log.message}</div>
                  </div>
                ))}
              </div>
            )}
          </div>
        </div>

        {/* Log Statistics */}
        <div className={styles.statsSection}>
          <h2 className={styles.statsTitle}>📊 Log Statistics</h2>
          <div className={styles.statsGrid}>
            <div className={styles.statItem}>
              <div className={styles.statNumber}>
                {stats?.errors || 0}
              </div>
              <div className={styles.statLabel}>Errors</div>
            </div>
            <div className={styles.statItem}>
              <div className={styles.statNumber}>
                {stats?.warnings || 0}
              </div>
              <div className={styles.statLabel}>Warnings</div>
            </div>
            <div className={styles.statItem}>
              <div className={styles.statNumber}>
                {stats?.infos || 0}
              </div>
              <div className={styles.statLabel}>Info Messages</div>
            </div>
            <div className={styles.statItem}>
              <div className={styles.statNumber}>{stats?.totalLogs || 0}</div>
              <div className={styles.statLabel}>Total Logs</div>
            </div>
          </div>
        </div>
      </div>
    </DashboardLayout>
  );
};

export default SystemLogsPage;
