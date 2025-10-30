import React, { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import RealTimeMap from '../../components/Dashboard/RealTimeMap';
import { useClientPositions } from '../../hooks/useClientPositions';
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

  // Load source/base station data
  useEffect(() => {
    const loadSources = async () => {
      try {
        // In a real application, fetch from API
        // For now, use mock data
        await new Promise((resolve) => setTimeout(resolve, 300));

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
        console.error('Failed to load sources:', error);
        setLoading(false);
      }
    };

    loadSources();
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

            {/* Client List */}
            <div className={styles.clientListSection}>
              <h2>Connected Rovers ({clientCount})</h2>
              <div className={styles.clientList}>
                {clients.length === 0 ? (
                  <div className={styles.emptyState}>
                    <p>No rovers currently connected</p>
                  </div>
                ) : (
                  <table className={styles.table}>
                    <thead>
                      <tr>
                        <th>Username</th>
                        <th>Serial #</th>
                        <th>Latitude</th>
                        <th>Longitude</th>
                        <th>Accuracy</th>
                        <th>Status</th>
                        <th>Last Update</th>
                      </tr>
                    </thead>
                    <tbody>
                      {clients.map((client) => (
                        <tr key={client.id} className={client.isStale ? styles.staleRow : ''}>
                          <td>{client.name.split(' #')[0]}</td>
                          <td>{client.name.split(' #')[1]}</td>
                          <td>{client.latitude.toFixed(6)}</td>
                          <td>{client.longitude.toFixed(6)}</td>
                          <td>{client.accuracy?.toFixed(2) ?? 'N/A'}</td>
                          <td>
                            <span className={client.isStale ? styles.staleBadge : styles.freshBadge}>
                              {client.isStale ? 'Stale' : 'Fresh'}
                            </span>
                          </td>
                          <td>
                            {new Date(client.lastUpdate).toLocaleTimeString()}
                          </td>
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
