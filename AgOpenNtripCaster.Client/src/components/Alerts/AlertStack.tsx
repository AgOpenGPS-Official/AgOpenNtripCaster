import React from 'react';
import { useAlerts, type SystemAlert, type AlertSeverity } from '../../hooks/useAlerts';
import styles from './AlertStack.module.css';

/**
 * Alert component - displays a single alert
 */
const Alert: React.FC<{ alert: SystemAlert; onClose: () => void }> = ({ alert, onClose }) => {
  const getSeverityIcon = (severity: AlertSeverity): string => {
    switch (severity) {
      case 'Info':
        return 'ℹ️';
      case 'Warning':
        return '⚠️';
      case 'Error':
        return '❌';
      case 'Critical':
        return '🚨';
      default:
        return 'ℹ️';
    }
  };

  const getSeverityClass = (severity: AlertSeverity): string => {
    return severity.toLowerCase();
  };

  return (
    <div className={`${styles.alert} ${styles[getSeverityClass(alert.severity)]}`}>
      <div className={styles.icon}>{getSeverityIcon(alert.severity)}</div>
      <div className={styles.content}>
        <div className={styles.title}>{alert.title}</div>
        <div className={styles.message}>{alert.message}</div>
        {alert.code && <div className={styles.code}>Code: {alert.code}</div>}
      </div>
      <button className={styles.closeBtn} onClick={onClose}>
        ✕
      </button>
    </div>
  );
};

/**
 * AlertStack component - displays all active alerts
 * Place this in your App.tsx or main layout to show alerts globally
 */
export const AlertStack: React.FC = () => {
  const { alerts, clearAlert } = useAlerts();

  if (alerts.length === 0) {
    return null;
  }

  return (
    <div className={styles.stack}>
      {alerts.map((alert) => (
        <Alert key={alert.id} alert={alert} onClose={() => clearAlert(alert.id)} />
      ))}
    </div>
  );
};

export default AlertStack;
