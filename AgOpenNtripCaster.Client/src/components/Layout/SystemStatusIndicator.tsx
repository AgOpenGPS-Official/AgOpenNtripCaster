import React from 'react';
import { useDashboardStats } from '../../hooks/useDashboardStats';
import styles from './SystemStatusIndicator.module.css';

export const SystemStatusIndicator: React.FC = () => {
  // Check server connection status via SignalR
  const { connected } = useDashboardStats();

  if (connected) {
    return (
      <div className={styles.statusContainer}>
        <div className={styles.statusOnline}>
          <span className={styles.indicator}>✓</span>
          <span className={styles.text}>System Online</span>
        </div>
      </div>
    );
  }

  return (
    <div className={styles.statusContainer}>
      <div className={`${styles.statusOffline} ${styles.blinking}`}>
        <span className={styles.indicator}>!</span>
        <span className={styles.text}>!!! SYSTEM OFFLINE !!!</span>
      </div>
    </div>
  );
};

export default SystemStatusIndicator;
