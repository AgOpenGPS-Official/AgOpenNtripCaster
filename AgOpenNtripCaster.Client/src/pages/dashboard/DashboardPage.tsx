import React, { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import StatsCard from '../../components/Dashboard/StatsCard';
import RealTimeMap from '../../components/Dashboard/RealTimeMap';
import { useClientPositions } from '../../hooks/useClientPositions';
import { useAuth } from '../../hooks/useAuth';
import { useDashboardStats } from '../../hooks/useDashboardStats';
import { mountPointsApi } from '../../services/mountPointsApi';
import { activityApi, type ActivityDto } from '../../services/activityApi';
import styles from './DashboardPage.module.css';

interface ClientPosition {
  id: string;
  name: string;
  latitude: number;
  longitude: number;
  accuracy?: number;
  lastUpdate: number;
  isStale: boolean;
}

interface SourcePosition {
  id: string;
  name: string;
  latitude: number;
  longitude: number;
}

export const DashboardPage: React.FC = () => {
  const { user } = useAuth();

  // Get real-time dashboard stats via SignalR
  const { stats: signalRStats } = useDashboardStats();

  // Fallback stats structure when SignalR data not available
  const stats = signalRStats
    ? {
        activeClients: signalRStats.activeClients,
        activeSources: signalRStats.activeSources,
        totalBytesReceived: signalRStats.totalBytesReceived,
        totalBytesSent: signalRStats.totalBytesSent,
        totalBytesTransferred: signalRStats.totalBytesTransferred,
        uptimeFormatted: signalRStats.uptimeFormatted,
      }
    : {
        activeClients: 0,
        activeSources: 0,
        totalBytesReceived: 0,
        totalBytesSent: 0,
        totalBytesTransferred: 0,
        uptimeFormatted: 'Waiting for server...',
      };

  const [sources, setSources] = useState<SourcePosition[]>([]);
  const [activities, setActivities] = useState<ActivityDto[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  // Use real-time client positions filtered by current user
  const { clients: realtimeClients } = useClientPositions(user?.userName);

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

  // Load initial mount points and activities data
  useEffect(() => {
    const loadDashboardData = async () => {
      setLoading(true);
      try {
        setError(null);
        let hasError = false;

        // Try to fetch mount points (sources/base stations)
        let sourcesWithCoords: SourcePosition[] = [];
        try {
          const mountPointsResponse = await mountPointsApi.getMountPoints(1, 100);
          const mountPoints = mountPointsResponse.mountPoints || [];

          // Filter for sources that are actually connected (activeSourceCount > 0) and have coordinates
          sourcesWithCoords = mountPoints
            .filter((mp: any) => {
              const hasActiveSource = mp.activeSourceCount > 0;
              const hasRtcm = mp.rtcmLatitude && mp.rtcmLongitude;
              const hasFallback = mp.latitude && mp.longitude;
              return hasActiveSource && (hasRtcm || hasFallback);
            })
            .map((mp: any) => {
              const rtcmValid = mp.rtcmLatitude && mp.rtcmLatitude >= -90 && mp.rtcmLatitude <= 90;
              const lonValid = mp.rtcmLongitude && mp.rtcmLongitude >= -180 && mp.rtcmLongitude <= 180;
              const lat = rtcmValid ? mp.rtcmLatitude : mp.latitude;
              const lon = lonValid ? mp.rtcmLongitude : mp.longitude;
              return {
                id: `source-${mp.id}`,
                name: mp.name,
                latitude: lat,
                longitude: lon,
              };
            });
        } catch (err) {
          console.error('Failed to load mount points:', err);
          hasError = true;
        }

        // Try to fetch recent activities
        try {
          const fetchedActivities = await activityApi.getRecentActivities(20);
          setActivities(fetchedActivities);
        } catch (err) {
          console.error('Failed to load activities:', err);
          // Don't set error for activities - it's not critical
        }

        // Update state
        setSources(sourcesWithCoords);

        if (hasError && sourcesWithCoords.length === 0) {
          setError('Unable to load dashboard data. Please check if the backend server is running.');
        }

        setLoading(false);
      } catch (error) {
        console.error('Unexpected error loading dashboard data:', error);
        setError('An unexpected error occurred. Please refresh the page.');
        setSources([]);
        setLoading(false);
      }
    };

    // Initial load
    loadDashboardData();

    // Note: Dashboard stats are now provided in real-time via SignalR hook
    // Activities will be optimized with real-time updates in Phase 1.2
  }, []);

  return (
    <DashboardLayout>
      <div className={styles.container}>
        <h1 className={styles.title}>Dashboard</h1>
        <p className={styles.subtitle}>Real-time NTRIP Caster monitoring and statistics</p>

        {/* Error Banner */}
        {error && (
          <div style={{
            padding: '12px 16px',
            marginBottom: '20px',
            backgroundColor: '#fee2e2',
            border: '1px solid #fecaca',
            borderRadius: '8px',
            color: '#991b1b',
            fontSize: '14px',
          }}>
            ⚠️ {error}
          </div>
        )}

        {loading ? (
          <div className={styles.loading}>
            <div className={styles.spinner}></div>
            <p>Loading dashboard...</p>
          </div>
        ) : (
          <>
            {/* Statistics Cards */}
            <div className={styles.statsGrid}>
              <StatsCard
                title="Active Clients"
                value={stats.activeClients}
                icon="👥"
                subtitle="Connected rovers"
                color="blue"
                trend="up"
                trendValue="Live"
              />
              <StatsCard
                title="Active Sources"
                value={stats.activeSources}
                icon="📡"
                subtitle="Base stations"
                color="green"
                trend="stable"
                trendValue="Active"
              />
              <StatsCard
                title="Data Transferred"
                value={`${stats.totalBytesTransferred.toFixed(2)} MB`}
                icon="📊"
                subtitle={`↓ ${stats.totalBytesReceived.toFixed(2)} MB | ↑ ${stats.totalBytesSent.toFixed(2)} MB`}
                color="orange"
                trend="up"
                trendValue="Real-time"
              />
              <StatsCard
                title="System Uptime"
                value={stats.uptimeFormatted}
                icon="⏱️"
                subtitle="Since server start"
                color={stats.uptimeFormatted === 'Server offline' ? 'red' : 'green'}
                trend={stats.uptimeFormatted === 'Server offline' ? 'down' : 'up'}
                trendValue={stats.uptimeFormatted === 'Server offline' ? 'Offline' : 'Running'}
              />
            </div>

            {/* Main Content: Map and Activity */}
            <div className={styles.mainContent}>
              {/* Real-time Map */}
              <div className={styles.mapSection}>
                <h2>Real-time Client & Source Positions</h2>
                <div className={styles.mapContainer}>
                  <RealTimeMap clients={clients} sources={sources} />
                </div>
              </div>

              {/* Activity Section */}
              <div className={styles.activitySection}>
                <h2>Recent Activity</h2>
                <div className={styles.activityList}>
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
            </div>
          </>
        )}
      </div>
    </DashboardLayout>
  );
};

export default DashboardPage;
