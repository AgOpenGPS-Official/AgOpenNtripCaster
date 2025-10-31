import React, { useState } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import styles from './DatabaseManagementPage.module.css';

export const DatabaseManagementPage: React.FC = () => {
  const [action, setAction] = useState<string | null>(null);
  const [isLoading, setIsLoading] = useState(false);

  const mockDBStats = {
    totalSize: '256 MB',
    tableCount: 12,
    recordCount: 45230,
    lastBackup: new Date(Date.now() - 3600000),
    databaseVersion: '9.0',
  };

  const tables = [
    { name: 'Users', records: 25, size: '2.5 MB' },
    { name: 'Groups', records: 8, size: '0.8 MB' },
    { name: 'MountPoints', records: 15, size: '1.2 MB' },
    { name: 'ClientSessions', records: 125, size: '5.6 MB' },
    { name: 'SourceConnections', records: 45, size: '1.8 MB' },
    { name: 'Activities', records: 12340, size: '45.2 MB' },
  ];

  const handleAction = async (actionName: string) => {
    setIsLoading(true);
    setAction(actionName);

    // Simulate API call
    setTimeout(() => {
      setIsLoading(false);
      alert(`${actionName} completed successfully!`);
    }, 2000);
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
              <div className={styles.statValue}>{mockDBStats.totalSize}</div>
            </div>
          </div>

          <div className={styles.statCard}>
            <div className={styles.statIcon}>📊</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Tables</div>
              <div className={styles.statValue}>{mockDBStats.tableCount}</div>
            </div>
          </div>

          <div className={styles.statCard}>
            <div className={styles.statIcon}>📝</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Total Records</div>
              <div className={styles.statValue}>{mockDBStats.recordCount.toLocaleString()}</div>
            </div>
          </div>

          <div className={styles.statCard}>
            <div className={styles.statIcon}>🕐</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Last Backup</div>
              <div className={styles.statValue}>
                {mockDBStats.lastBackup.toLocaleTimeString()}
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
              <select className={styles.select}>
                <option>30 days</option>
                <option>60 days</option>
                <option>90 days</option>
                <option>180 days</option>
              </select>
              <button className={styles.cleanupButton}>Delete</button>
            </div>

            <div className={styles.cleanupCard}>
              <h3>Remove Orphaned Records</h3>
              <p>Clean up disconnected sessions and sources</p>
              <button className={styles.cleanupButton}>Clean Now</button>
            </div>

            <div className={styles.cleanupCard}>
              <h3>Archive Data</h3>
              <p>Archive old activity records</p>
              <select className={styles.select}>
                <option>Before 6 months ago</option>
                <option>Before 1 year ago</option>
                <option>Before 2 years ago</option>
              </select>
              <button className={styles.cleanupButton}>Archive</button>
            </div>
          </div>
        </div>
      </div>
    </DashboardLayout>
  );
};

export default DatabaseManagementPage;
