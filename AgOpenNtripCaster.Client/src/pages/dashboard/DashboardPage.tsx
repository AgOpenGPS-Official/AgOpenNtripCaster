import React, { useMemo } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import StatsCard from '../../components/Dashboard/StatsCard';
import RealTimeMap from '../../components/Dashboard/RealTimeMap';
import { useClientPositions } from '../../hooks/useClientPositions';
import { useAuth } from '../../hooks/useAuth';
import { useDashboardStats } from '../../hooks/useDashboardStats';
import { useActivityFeed } from '../../hooks/useActivityFeed';
import { useMountPoints } from '../../hooks/useMountPoints';
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
  const { stats: signalRStats, connected: signalRConnected } = useDashboardStats();

  // Fallback stats structure when SignalR data not available
  const stats = signalRStats
    ? {
        activeClients: signalRStats.activeClients,
        activeSources: signalRStats.activeSources,
        totalBytesReceived: signalRStats.totalBytesReceived,
        totalBytesSent: signalRStats.totalBytesSent,
        totalBytesTransferred: signalRStats.totalBytesTransferred,
        uptimeFormatted: signalRConnected ? signalRStats.uptimeFormatted : 'Server Offline',
      }
    : {
        activeClients: 0,
        activeSources: 0,
        totalBytesReceived: 0,
        totalBytesSent: 0,
        totalBytesTransferred: 0,
        uptimeFormatted: 'Waiting for server...',
      };

  // Use real-time mount points with live source/client counts
  const { mountPoints, loading, error: mountPointsError } = useMountPoints();

  // Use real-time client positions filtered by current user
  const { clients: realtimeClients } = useClientPositions(user?.userName);

  // Use real-time activity feed via SignalR, filtered for current user only (max 8 items)
  const { activities } = useActivityFeed(user?.id, 8);

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

  // Transform mount points to sources for the map (real-time updates)
  const sources: SourcePosition[] = useMemo(() => {
    return mountPoints
      .filter((mp) => {
        const hasActiveSource = mp.activeSourceCount > 0;
        const hasRtcm = mp.rtcmLatitude && mp.rtcmLongitude;
        const hasFallback = mp.latitude && mp.longitude;
        return hasActiveSource && (hasRtcm || hasFallback);
      })
      .map((mp) => {
        const rtcmValid = mp.rtcmLatitude && mp.rtcmLatitude >= -90 && mp.rtcmLatitude <= 90;
        const lonValid = mp.rtcmLongitude && mp.rtcmLongitude >= -180 && mp.rtcmLongitude <= 180;
        const lat = rtcmValid ? mp.rtcmLatitude! : mp.latitude!;
        const lon = lonValid ? mp.rtcmLongitude! : mp.longitude!;
        return {
          id: `source-${mp.id}`,
          name: mp.name,
          latitude: lat,
          longitude: lon,
        };
      });
  }, [mountPoints]);

  const error = mountPointsError?.message || null;

  return (
    <DashboardLayout>
      <div className={styles.container}>
        <h1 className={styles.title}>User Dashboard</h1>
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
                value={realtimeClients.length}
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
