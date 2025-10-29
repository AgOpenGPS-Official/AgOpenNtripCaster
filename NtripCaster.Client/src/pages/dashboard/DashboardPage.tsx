import React, { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import StatsCard from '../../components/Dashboard/StatsCard';
import RealTimeMap from '../../components/Dashboard/RealTimeMap';
import styles from './DashboardPage.module.css';

interface DashboardStats {
  activeClients: number;
  activeSources: number;
  totalDataTransferred: string;
  uptime: string;
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
  const [stats, setStats] = useState<DashboardStats>({
    activeClients: 0,
    activeSources: 0,
    totalDataTransferred: '0 MB',
    uptime: '0h 0m',
  });

  const [clients, setClients] = useState<ClientPosition[]>([]);
  const [sources, setSources] = useState<SourcePosition[]>([]);
  const [loading, setLoading] = useState(true);

  // Simulate loading dashboard data
  useEffect(() => {
    // In a real application, this would fetch from the backend
    const loadDashboardData = async () => {
      try {
        // Simulate API call delay
        await new Promise((resolve) => setTimeout(resolve, 500));

        // Mock data for demonstration
        setStats({
          activeClients: 12,
          activeSources: 3,
          totalDataTransferred: '234.5 MB',
          uptime: '5d 14h 32m',
        });

        setClients([
          {
            id: '1',
            name: 'Client A',
            latitude: 40.7128,
            longitude: -74.006,
            accuracy: 2.5,
            lastUpdate: Date.now(),
            isStale: false,
          },
          {
            id: '2',
            name: 'Client B',
            latitude: 34.0522,
            longitude: -118.2437,
            accuracy: 1.8,
            lastUpdate: Date.now(),
            isStale: false,
          },
          {
            id: '3',
            name: 'Client C',
            latitude: 41.8781,
            longitude: -87.6298,
            accuracy: 3.2,
            lastUpdate: Date.now(),
            isStale: false,
          },
        ]);

        setSources([
          {
            id: 's1',
            name: 'Base Station 1',
            latitude: 40.712,
            longitude: -74.0047,
          },
          {
            id: 's2',
            name: 'Base Station 2',
            latitude: 34.0537,
            longitude: -118.2453,
          },
          {
            id: 's3',
            name: 'Base Station 3',
            latitude: 41.878,
            longitude: -87.6298,
          },
        ]);

        setLoading(false);
      } catch (error) {
        console.error('Failed to load dashboard data:', error);
        setLoading(false);
      }
    };

    loadDashboardData();
  }, []);

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
                value={stats.totalDataTransferred}
                icon="📊"
                subtitle="Total since restart"
                color="orange"
                trend="up"
                trendValue="+12.3 MB/hr"
              />
              <StatsCard
                title="System Uptime"
                value={stats.uptime}
                icon="⏱️"
                subtitle="Since last restart"
                color="green"
                trend="up"
                trendValue="Stable"
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

              {/* Recent Activity */}
              <div className={styles.activitySection}>
                <h2>Recent Activity</h2>
                <div className={styles.activityList}>
                  <div className={styles.activityItem}>
                    <span className={styles.icon}>✓</span>
                    <div className={styles.details}>
                      <p className={styles.message}>Client A connected</p>
                      <span className={styles.time}>5 minutes ago</span>
                    </div>
                  </div>
                  <div className={styles.activityItem}>
                    <span className={styles.icon}>🔄</span>
                    <div className={styles.details}>
                      <p className={styles.message}>System restart completed</p>
                      <span className={styles.time}>5 days ago</span>
                    </div>
                  </div>
                  <div className={styles.activityItem}>
                    <span className={styles.icon}>⚠️</span>
                    <div className={styles.details}>
                      <p className={styles.message}>High latency detected on source 2</p>
                      <span className={styles.time}>2 hours ago</span>
                    </div>
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
