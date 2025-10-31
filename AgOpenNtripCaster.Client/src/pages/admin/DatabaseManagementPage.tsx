import React, { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import styles from './DatabaseManagementPage.module.css';
import { databaseApi, type DatabaseStats, type TableInfo } from '../../services/databaseApi';

export const DatabaseManagementPage: React.FC = () => {
  const [action, setAction] = useState<string | null>(null);
  const [isLoading, setIsLoading] = useState(false);
  const [dbStats, setDbStats] = useState<DatabaseStats | null>(null);
  const [tables, setTables] = useState<TableInfo[]>([]);
  const [cleanupDays, setCleanupDays] = useState('30');

  useEffect(() => {
    const loadDatabaseInfo = async () => {
      try {
        const [stats, tablesData] = await Promise.all([
          databaseApi.getStatistics(),
          databaseApi.getTableInformation(),
        ]);
        setDbStats(stats);
        setTables(tablesData);
      } catch (err) {
        console.error('Failed to load database information:', err);
      }
    };

    loadDatabaseInfo();
  }, []);

  const handleAction = async (actionName: string) => {
    setIsLoading(true);
    setAction(actionName);

    try {
      switch (actionName) {
        case 'Backup Database':
          await databaseApi.backup();
          break;
        case 'Optimize Tables':
          await databaseApi.optimize();
          break;
        case 'Repair Database':
          await databaseApi.repair();
          break;
        case 'Vacuum Database':
          await databaseApi.vacuum();
          break;
        case 'Reindex Database':
          await databaseApi.reindex();
          break;
        case 'Rebuild Statistics':
          await databaseApi.rebuildStatistics();
          break;
      }
      alert(`${actionName} completed successfully!`);
      // Reload stats after action
      const stats = await databaseApi.getStatistics();
      setDbStats(stats);
    } catch (err) {
      console.error(`Failed to execute ${actionName}:`, err);
      alert(`Failed to execute ${actionName}. Please try again.`);
    } finally {
      setIsLoading(false);
      setAction(null);
    }
  };

  const handleCleanupOldLogs = async () => {
    setIsLoading(true);
    try {
      await databaseApi.cleanupOldLogs(parseInt(cleanupDays));
      alert('Old logs cleaned up successfully!');
      // Reload stats
      const stats = await databaseApi.getStatistics();
      setDbStats(stats);
    } catch (err) {
      console.error('Failed to cleanup old logs:', err);
      alert('Failed to cleanup old logs. Please try again.');
    } finally {
      setIsLoading(false);
    }
  };

  const handleCleanupOrphaned = async () => {
    setIsLoading(true);
    try {
      await databaseApi.cleanupOrphanedRecords();
      alert('Orphaned records cleaned up successfully!');
      // Reload stats and tables
      const [stats, tablesData] = await Promise.all([
        databaseApi.getStatistics(),
        databaseApi.getTableInformation(),
      ]);
      setDbStats(stats);
      setTables(tablesData);
    } catch (err) {
      console.error('Failed to cleanup orphaned records:', err);
      alert('Failed to cleanup orphaned records. Please try again.');
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <DashboardLayout>
      <div className={styles.container}>
        <div className={styles.header}>
          <h1 className={styles.title}>Database Management</h1>
          <p className={styles.subtitle}>Monitor and maintain database integrity</p>
        </div>

        {/* Database Statistics */}
        <div className={styles.statsGrid}>
          <div className={styles.statCard}>
            <div className={styles.statIcon}>💾</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Total Size</div>
              <div className={styles.statValue}>{dbStats?.totalSize || 'N/A'}</div>
            </div>
          </div>

          <div className={styles.statCard}>
            <div className={styles.statIcon}>📊</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Tables</div>
              <div className={styles.statValue}>{dbStats?.tableCount || 0}</div>
            </div>
          </div>

          <div className={styles.statCard}>
            <div className={styles.statIcon}>📝</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Total Records</div>
              <div className={styles.statValue}>{dbStats?.recordCount.toLocaleString() || '0'}</div>
            </div>
          </div>

          <div className={styles.statCard}>
            <div className={styles.statIcon}>🕐</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Last Backup</div>
              <div className={styles.statValue}>
                {dbStats?.lastBackup || 'Never'}
              </div>
            </div>
          </div>
        </div>

        {/* Maintenance Actions */}
        <div className={styles.maintenanceSection}>
          <h2 className={styles.sectionTitle}>🔧 Maintenance Actions</h2>

          <div className={styles.actionGrid}>
            <button
              className={styles.actionCard}
              onClick={() => handleAction('Backup Database')}
              disabled={isLoading}
            >
              <div className={styles.actionIcon}>💾</div>
              <div className={styles.actionName}>Backup Database</div>
              <div className={styles.actionDescription}>Create a complete database backup</div>
              {isLoading && action === 'Backup Database' && <div className={styles.spinner}></div>}
            </button>

            <button
              className={styles.actionCard}
              onClick={() => handleAction('Optimize Tables')}
              disabled={isLoading}
            >
              <div className={styles.actionIcon}>⚡</div>
              <div className={styles.actionName}>Optimize Tables</div>
              <div className={styles.actionDescription}>Optimize and defragment tables</div>
              {isLoading && action === 'Optimize Tables' && <div className={styles.spinner}></div>}
            </button>

            <button
              className={styles.actionCard}
              onClick={() => handleAction('Repair Database')}
              disabled={isLoading}
            >
              <div className={styles.actionIcon}>🔨</div>
              <div className={styles.actionName}>Repair Database</div>
              <div className={styles.actionDescription}>Check and repair database integrity</div>
              {isLoading && action === 'Repair Database' && <div className={styles.spinner}></div>}
            </button>

            <button
              className={styles.actionCard}
              onClick={() => handleAction('Vacuum Database')}
              disabled={isLoading}
            >
              <div className={styles.actionIcon}>🗑️</div>
              <div className={styles.actionName}>Vacuum Database</div>
              <div className={styles.actionDescription}>Reclaim unused space</div>
              {isLoading && action === 'Vacuum Database' && <div className={styles.spinner}></div>}
            </button>

            <button
              className={styles.actionCard}
              onClick={() => handleAction('Reindex Database')}
              disabled={isLoading}
            >
              <div className={styles.actionIcon}>📇</div>
              <div className={styles.actionName}>Reindex Database</div>
              <div className={styles.actionDescription}>Rebuild all database indexes</div>
              {isLoading && action === 'Reindex Database' && <div className={styles.spinner}></div>}
            </button>

            <button
              className={styles.actionCard}
              onClick={() => handleAction('Rebuild Statistics')}
              disabled={isLoading}
            >
              <div className={styles.actionIcon}>📈</div>
              <div className={styles.actionName}>Rebuild Statistics</div>
              <div className={styles.actionDescription}>Recalculate table statistics</div>
              {isLoading && action === 'Rebuild Statistics' && <div className={styles.spinner}></div>}
            </button>
          </div>
        </div>

        {/* Table Information */}
        <div className={styles.tablesSection}>
          <h2 className={styles.sectionTitle}>📋 Table Information</h2>

          <div className={styles.tableWrapper}>
            <table className={styles.table}>
              <thead>
                <tr>
                  <th>Table Name</th>
                  <th>Records</th>
                  <th>Size</th>
                </tr>
              </thead>
              <tbody>
                {tables.map((table) => (
                  <tr key={table.name}>
                    <td className={styles.nameCell}>{table.name}</td>
                    <td className={styles.numberCell}>{table.records.toLocaleString()}</td>
                    <td className={styles.sizeCell}>{table.size}</td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </div>

        {/* Cleanup Options */}
        <div className={styles.cleanupSection}>
          <h2 className={styles.sectionTitle}>🧹 Cleanup Options</h2>

          <div className={styles.cleanupOptions}>
            <div className={styles.cleanupCard}>
              <h3>Remove Old Logs</h3>
              <p>Delete activity logs older than:</p>
              <select
                className={styles.select}
                value={cleanupDays}
                onChange={(e) => setCleanupDays(e.target.value)}
                disabled={isLoading}
              >
                <option value="30">30 days</option>
                <option value="60">60 days</option>
                <option value="90">90 days</option>
                <option value="180">180 days</option>
              </select>
              <button
                className={styles.cleanupButton}
                onClick={handleCleanupOldLogs}
                disabled={isLoading}
              >
                Delete
              </button>
            </div>

            <div className={styles.cleanupCard}>
              <h3>Remove Orphaned Records</h3>
              <p>Clean up disconnected sessions and sources</p>
              <button
                className={styles.cleanupButton}
                onClick={handleCleanupOrphaned}
                disabled={isLoading}
              >
                Clean Now
              </button>
            </div>
          </div>
        </div>
      </div>
    </DashboardLayout>
  );
};

export default DatabaseManagementPage;
