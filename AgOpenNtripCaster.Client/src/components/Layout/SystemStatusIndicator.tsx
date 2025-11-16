import React, { useEffect, useState } from 'react';
import { signalRService } from '../../services/signalRService';
import styles from './SystemStatusIndicator.module.css';

export const SystemStatusIndicator: React.FC = () => {
  // Track connection status directly from SignalR service
  const [connected, setConnected] = useState(() => signalRService.isConnected());

  useEffect(() => {
    // Subscribe to connection status changes
    const unsubscribe = signalRService.onConnectionStatusChange((isConnected: boolean) => {
      setConnected(isConnected);
    });

    // Also poll the connection state every 2 seconds to catch missed events
    // This is a safety net for edge cases where reconnection events might be missed
    const pollInterval = setInterval(() => {
      const currentStatus = signalRService.isConnected();
      setConnected((prevStatus) => {
        if (prevStatus !== currentStatus) {
          return currentStatus;
        }
        return prevStatus;
      });
    }, 2000);

    return () => {
      unsubscribe();
      clearInterval(pollInterval);
    };
  }, []);

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
