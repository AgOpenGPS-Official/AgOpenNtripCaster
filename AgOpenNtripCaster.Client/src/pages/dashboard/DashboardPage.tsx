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

  // Use real-time client positions filtered by current user
  const { clients: realtimeClients, isConnected } = useClientPositions(user?.userName);

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
      try {
        // Fetch all mount points (sources/base stations)
        const mountPointsResponse = await mountPointsApi.getMountPoints(1, 100);
        const mountPoints = mountPointsResponse.mountPoints || [];

        // Fetch dashboard statistics
        const statsData = await dashboardStatsApi.getDashboardStats();

        // Filter for active mount points with coordinates
        const sourcesWithCoords = mountPoints
          .filter((mp: any) => mp.isActive && mp.latitude && mp.longitude)
          .map((mp: any) => ({
            id: `source-${mp.id}`,
            name: mp.name,
            latitude: mp.latitude,
            longitude: mp.longitude,
          }));

        // Update stats with real data
        setStats({
          activeClients: statsData.activeClients,
          activeSources: statsData.activeSources,
          totalBytesReceived: statsData.totalBytesReceived,
          totalBytesSent: statsData.totalBytesSent,
          totalBytesTransferred: statsData.totalBytesTransferred,
          uptimeFormatted: statsData.uptimeFormatted,
        });

        setSources(sourcesWithCoords);
        setLoading(false);
      } catch (error) {
        console.error('Failed to load dashboard data:', error);
        // Still show the page even if loading fails
        setSources([]);
        setStats((prev) => ({
          ...prev,
          activeClients: realtimeClients.length,
        }));
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
                subtitle="Connected clients"
                color="blue"
                trend="up"
                trendValue="+3 today"
              />
              <StatsCard
                title="Active Sources"
                value={stats.activeSources}
                icon="📡"
                subtitle="GNSS stations"
                color="green"
                trend="stable"
                trendValue="No change"
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
                color="green"
                trend="up"
                trendValue="Running"
              />
            </div>

            {/* Main Content: Map and Activity */}
            <div className={styles.mainContent}>
              {/* Real-time Map */}
              <div className={styles.mapSection}>
                <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                  <h2>Real-time Client & Source Positions</h2>
                  <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                    <div style={{
                      width: '12px',
                      height: '12px',
                      borderRadius: '50%',
                      backgroundColor: isConnected ? '#10b981' : '#ef4444',
                    }} />
                    <span style={{ fontSize: '14px', color: isConnected ? '#10b981' : '#ef4444' }}>
                      {isConnected ? 'Live' : 'Connecting...'}
                    </span>
                  </div>
                </div>
                <div className={styles.mapContainer}>
                  <RealTimeMap clients={clients} sources={sources} />
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
