import React, { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import StatsCard from '../../components/Dashboard/StatsCard';
import RealTimeMap from '../../components/Dashboard/RealTimeMap';
import { useClientPositions } from '../../hooks/useClientPositions';
import { useAuth } from '../../hooks/useAuth';
import { mountPointsApi } from '../../services/mountPointsApi';
import { dashboardStatsApi } from '../../services/dashboardStatsApi';
import styles from './DashboardPage.module.css';

interface DashboardStats {
  activeClients: number;
  activeSources: number;
  totalBytesReceived: number;
  totalBytesSent: number;
  totalBytesTransferred: number;
  uptimeFormatted: string;
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

interface SourcePosition {
  id: string;
  name: string;
  latitude: number;
  longitude: number;
}

export const DashboardPage: React.FC = () => {
  const { user } = useAuth();
  const [stats, setStats] = useState<DashboardStats>({
    activeClients: 0,
    activeSources: 0,
    totalBytesReceived: 0,
    totalBytesSent: 0,
    totalBytesTransferred: 0,
    uptimeFormatted: '0m',
  });

  const [sources, setSources] = useState<SourcePosition[]>([]);
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

  // Load dashboard data (sources and stats)
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

          sourcesWithCoords = mountPoints
            .filter((mp: any) => mp.isActive && mp.latitude && mp.longitude)
            .map((mp: any) => ({
              id: `source-${mp.id}`,
              name: mp.name,
              latitude: mp.latitude,
              longitude: mp.longitude,
            }));
        } catch (err) {
          console.error('Failed to load mount points:', err);
          hasError = true;
        }

        // Try to fetch dashboard statistics
        let statsData: DashboardStats = {
          activeClients: realtimeClients.length,
          activeSources: 0,
          totalBytesReceived: 0,
          totalBytesSent: 0,
          totalBytesTransferred: 0,
          uptimeFormatted: 'Server offline',
        };

        try {
          const fetchedStats = await dashboardStatsApi.getDashboardStats();
          statsData = {
            activeClients: fetchedStats.activeClients,
            activeSources: fetchedStats.activeSources,
            totalBytesReceived: fetchedStats.totalBytesReceived,
            totalBytesSent: fetchedStats.totalBytesSent,
            totalBytesTransferred: fetchedStats.totalBytesTransferred,
            uptimeFormatted: fetchedStats.uptimeFormatted,
          };
        } catch (err) {
          console.error('Failed to load statistics:', err);
          hasError = true;
          setError('Unable to fetch server statistics. Backend may be offline.');
        }

        // Update state - always show content
        setStats(statsData);
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

    loadDashboardData();
  }, [realtimeClients.length]);

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
                  <div className={styles.emptyState}>
                    <p>No activity seen yet</p>
                  </div>
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
