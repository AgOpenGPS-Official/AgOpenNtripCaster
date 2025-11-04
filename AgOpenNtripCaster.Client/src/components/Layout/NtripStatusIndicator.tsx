import React from 'react';
import { useDashboardStats } from '../../hooks/useDashboardStats';
import styles from './NtripStatusIndicator.module.css';

export const NtripStatusIndicator: React.FC = () => {
  const { stats } = useDashboardStats();

  // Check if there's active upload (sources connected)
  const hasUpload = (stats?.activeSources ?? 0) > 0;

  // Check if there's active download (clients connected)
  const hasDownload = (stats?.activeClients ?? 0) > 0;

  const isRunning = stats?.rtcmListenerActive ?? false;

  return (
    <div className={styles.statusContainer}>
      <div className={`${styles.statusCard} ${isRunning ? styles.active : styles.inactive}`}>
        {/* Upload Arrow (left) - Sources sending data */}
        <svg
          className={`${styles.arrow} ${hasUpload ? styles.filled : styles.outline}`}
          viewBox="0 0 24 24"
          xmlns="http://www.w3.org/2000/svg"
          width="28"
          height="28"
        >
          <path
            d="M12 2L12 20M5 13L12 20L19 13"
            stroke="currentColor"
            strokeWidth="2.5"
            strokeLinecap="round"
            strokeLinejoin="round"
            fill="none"
          />
        </svg>

        {/* Status Text */}
        <div className={styles.statusContent}>
          <div className={styles.statusText}>
            NTRIP (2101)
          </div>
        </div>

        {/* Download Arrow (right) - Clients receiving data */}
        <svg
          className={`${styles.arrow} ${hasDownload ? styles.filled : styles.outline}`}
          viewBox="0 0 24 24"
          xmlns="http://www.w3.org/2000/svg"
          width="28"
          height="28"
        >
          <path
            d="M12 2L12 20M5 13L12 20L19 13"
            stroke="currentColor"
            strokeWidth="2.5"
            strokeLinecap="round"
            strokeLinejoin="round"
            fill="none"
          />
        </svg>
      </div>
    </div>
  );
};

export default NtripStatusIndicator;
