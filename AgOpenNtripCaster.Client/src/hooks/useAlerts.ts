import { useEffect, useState } from 'react';
import { signalRService, AlertSeverity, type SystemAlert } from '../services/signalRService';

// Re-export for consumers
export { AlertSeverity };
export type { SystemAlert };

export interface UseAlertsResult {
  alerts: SystemAlert[];
  clearAlert: (id: string) => void;
  clearAllAlerts: () => void;
}

/**
 * Custom hook for real-time system alerts via SignalR
 *
 * Subscribes to SystemAlert events from backend and maintains
 * a list of active alerts. Automatically removes alerts after
 * a timeout (unless they are Critical severity).
 *
 * Usage:
 * ```tsx
 * const { alerts, clearAlert } = useAlerts();
 *
 * return (
 *   <div>
 *     {alerts.map(alert => (
 *       <Alert
 *         key={alert.id}
 *         alert={alert}
 *         onClose={() => clearAlert(alert.id)}
 *       />
 *     ))}
 *   </div>
 * );
 * ```
 */
export function useAlerts(): UseAlertsResult {
  const [alerts, setAlerts] = useState<SystemAlert[]>([]);

  useEffect(() => {
    let isMounted = true;
    const timeouts: Map<string, ReturnType<typeof setTimeout>> = new Map();

    // Subscribe to system alerts
    const unsubscribeAlerts = signalRService.onSystemAlert((newAlert: SystemAlert) => {
      if (isMounted) {
        setAlerts((prevAlerts) => [...prevAlerts, newAlert]);

        // Auto-dismiss non-critical alerts after 5 seconds
        if (newAlert.severity !== 'Critical') {
          const timeout = setTimeout(() => {
            if (isMounted) {
              setAlerts((current) => current.filter((a) => a.id !== newAlert.id));
              timeouts.delete(newAlert.id);
            }
          }, 5000);

          timeouts.set(newAlert.id, timeout);
        }
      }
    });

    // Cleanup subscription on unmount
    return () => {
      isMounted = false;
      unsubscribeAlerts();
      // Clear all pending timeouts
      timeouts.forEach((timeout) => clearTimeout(timeout));
      timeouts.clear();
    };
  }, []);

  const clearAlert = (id: string) => {
    setAlerts((prevAlerts) => prevAlerts.filter((a) => a.id !== id));
  };

  const clearAllAlerts = () => {
    setAlerts([]);
  };

  return {
    alerts,
    clearAlert,
    clearAllAlerts,
  };
}
