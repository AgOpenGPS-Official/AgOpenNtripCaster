import React, { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import RealTimeMap from '../../components/Dashboard/RealTimeMap';
import { useClientPositions } from '../../hooks/useClientPositions';
import { mountPointsApi } from '../../services/mountPointsApi';
import { activityApi, type ActivityDto } from '../../services/activityApi';
import styles from './AdminRealtimeMapPage.module.css';

interface SourcePosition {
  id: string;
  name: string;
  latitude: number;
  longitude: number;
}

interface ClientPosition {
  id: string;
  name: string;
  latitude: number;
  longitude: number;
  accuracy?: number;
  lastUpdate: number;
  isStale: boolean;
}

export const AdminRealtimeMapPage: React.FC = () => {
  const [sources, setSources] = useState<SourcePosition[]>([]);
  const [activities, setActivities] = useState<ActivityDto[]>([]);
  const [loading, setLoading] = useState(true);

  // Get all clients (no username filter for admin)
  const { clients: realtimeClients, isConnected, clientCount } = useClientPositions();

  // Transform real-time clients to map component format
  const clients: ClientPosition[] = realtimeClients.map((client) => ({
    id: client.id,
    name: `${client.username} #${client.serialNumber}`,
    latitude: client.latitude,
    longitude: client.longitude,
    accuracy: client.accuracy,
    lastUpdate: client.lastUpdate,
    isStale: client.isStale,
  }));

  // Load source/base station data and activities with auto-refresh
  useEffect(() => {
    const loadData = async (isInitialLoad = false) => {
      try {
        // Fetch all mount points (sources/base stations)
        const mountPointsResponse = await mountPointsApi.getMountPoints(1, 100);
        const mountPoints = mountPointsResponse.mountPoints || [];

        // Filter for sources that are actually connected (activeSourceCount > 0) with coordinates (RTCM or fallback)
        const sourcesWithCoords = mountPoints
          .filter((mp: any) => mp.activeSourceCount > 0)
          .filter((mp: any) => (mp.rtcmLatitude && mp.rtcmLongitude) || (mp.latitude && mp.longitude))
          .map((mp: any) => ({
            id: `source-${mp.id}`,
            name: mp.name,
            // Use RTCM-extracted coordinates if valid, fallback to manual coordinates
            // RTCM valid: latitude between -90 and 90, longitude between -180 and 180
            latitude: (mp.rtcmLatitude && mp.rtcmLatitude >= -90 && mp.rtcmLatitude <= 90)
              ? mp.rtcmLatitude
              : mp.latitude,
            longitude: (mp.rtcmLongitude && mp.rtcmLongitude >= -180 && mp.rtcmLongitude <= 180)
              ? mp.rtcmLongitude
              : mp.longitude,
          }));

        setSources(sourcesWithCoords);

        // Fetch recent activities
        try {
          const fetchedActivities = await activityApi.getRecentActivities(20);
          setActivities(fetchedActivities);
        } catch (err) {
          console.error('Failed to load activities:', err);
        }

        if (isInitialLoad) setLoading(false);
      } catch (error) {
        console.error('Failed to load data:', error);
        setSources([]);
        if (isInitialLoad) setLoading(false);
      }
    };

    // Initial load
    loadData(true);

    // Refresh data in background every 5 seconds to catch RTCM1005 updates
    // Only refresh if page is visible
    const interval = setInterval(() => {
      if (!document.hidden) {
        loadData(false);
      }
    }, 5000);

    const handleVisibilityChange = () => {
      if (!document.hidden) {
        loadData(false);
      }
    };

    document.addEventListener('visibilitychange', handleVisibilityChange);

    return () => {
      clearInterval(interval);
      document.removeEventListener('visibilitychange', handleVisibilityChange);
    };
  }, []);

  return (
    <DashboardLayout>
      <div className={styles.container}>
        <div className={styles.header}>
          <h1 className={styles.title}>Real-Time Network Monitoring</h1>
          <p className={styles.subtitle}>Live view of all connected base stations and rovers</p>
        </div>

        {loading ? (
          <div className={styles.loading}>
            <div className={styles.spinner}></div>
            <p>Loading network data...</p>
          </div>
        ) : (
          <>
            {/* Stats Overview */}
            <div className={styles.statsOverview}>
              <div className={styles.stat}>
                <div className={styles.statValue}>{clientCount}</div>
                <div className={styles.statLabel}>Connected Rovers</div>
              </div>
              <div className={styles.stat}>
                <div className={styles.statValue}>{sources.length}</div>
                <div className={styles.statLabel}>Base Stations</div>
              </div>
              <div className={styles.stat}>
                <div style={{
                  width: '12px',
                  height: '12px',
                  borderRadius: '50%',
                  backgroundColor: isConnected ? '#10b981' : '#ef4444',
                  marginBottom: '8px',
                }} />
                <div className={styles.statLabel}>
                  {isConnected ? 'Live Stream Active' : 'Connecting...'}
                </div>
              </div>
            </div>

            {/* Real-Time Map */}
            <div className={styles.mapSection}>
              <h2>Global Network Positions</h2>
              <div className={styles.mapContainer}>
                <RealTimeMap clients={clients} sources={sources} />
              </div>
            </div>

            {/* Recent Activity Section */}
            <div className={styles.clientListSection}>
              <h2>Recent Activity</h2>
              <div className={styles.clientList}>
                {activities.length === 0 ? (
                  <div className={styles.emptyState}>
                    <p>No activity seen yet</p>
                  </div>
                ) : (
                  <table className={styles.activityTable}>
                    <tbody>
                      {activities.map((activity) => (
                        <tr key={activity.id} className={styles.activityRow}>
                          <td className={styles.activityTime}>
                            {new Date(activity.createdAt).toLocaleTimeString()}
                          </td>
                          <td className={styles.activityType}>
                            <span className={`${styles.badge} ${styles[activity.type.toLowerCase()]}`}>
                              {activity.type === 'SourceConnected' && '📡 Base station'}
                              {activity.type === 'SourceDisconnected' && '📡 Base station'}
                              {activity.type === 'ClientConnected' && '🛰️ Rover'}
                              {activity.type === 'ClientDisconnected' && '🛰️ Rover'}
                            </span>
                          </td>
                          <td className={styles.activityDescription}>{activity.description}</td>
                        </tr>
                      ))}
                    </tbody>
                  </table>
                )}
              </div>
            </div>
          </>
        )}
      </div>
    </DashboardLayout>
  );
};

export default AdminRealtimeMapPage;
