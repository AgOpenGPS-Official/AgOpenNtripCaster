import React, { useState, useEffect, useRef } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import UserActivityPanel from '../../components/Dashboard/UserActivityPanel';
import RealTimeMap from '../../components/Dashboard/RealTimeMap';
import styles from './AdminDashboardPage.module.css';
import { useDashboardStats } from '../../hooks/useDashboardStats';
import { useClientPositions } from '../../hooks/useClientPositions';
import { mountPointsApi } from '../../services/mountPointsApi';

interface SourcePosition {
  id: string;
  name: string;
  latitude: number;
  longitude: number;
}

interface ClientPosition {
  id: string;
  name: string;
  username: string;
  serialNumber: number;
  mountPoint: string;
  latitude: number;
  longitude: number;
  accuracy?: number;
  lastUpdate: number;
  isStale: boolean;
}

type SortField = 'username' | 'source' | 'serial' | 'status' | 'lastUpdate';
type SortOrder = 'asc' | 'desc';

export const AdminDashboardPage: React.FC = () => {
  const mapRef = useRef<HTMLDivElement>(null);
  const [sources, setSources] = useState<SourcePosition[]>([]);
  const [mapLoading, setMapLoading] = useState(true);
  const [sortField, setSortField] = useState<SortField>('username');
  const [sortOrder, setSortOrder] = useState<SortOrder>('asc');

  // Get real-time dashboard stats via SignalR
  const { stats: signalRStats, connected: signalRConnected } = useDashboardStats();

  // Get all clients (no username filter for admin)
  const { clients: realtimeClients } = useClientPositions();

  // Transform real-time clients to map component format
  const clients: ClientPosition[] = realtimeClients.map((client) => ({
    id: client.id,
    name: `${client.username} #${client.serialNumber}`,
    username: client.username,
    serialNumber: client.serialNumber,
    mountPoint: client.mountPoint,
    latitude: client.latitude,
    longitude: client.longitude,
    accuracy: client.accuracy,
    lastUpdate: client.lastUpdate,
    isStale: client.isStale,
  }));

  // Sort clients based on current sort field
  const sortedClients = [...clients].sort((a, b) => {
    let aValue: string | number;
    let bValue: string | number;

    switch (sortField) {
      case 'username':
        aValue = a.username.toLowerCase();
        bValue = b.username.toLowerCase();
        break;
      case 'source':
        aValue = a.mountPoint.toLowerCase();
        bValue = b.mountPoint.toLowerCase();
        break;
      case 'serial':
        aValue = a.serialNumber;
        bValue = b.serialNumber;
        break;
      case 'status':
        aValue = a.isStale ? 'stale' : 'fresh';
        bValue = b.isStale ? 'stale' : 'fresh';
        break;
      case 'lastUpdate':
        aValue = a.lastUpdate;
        bValue = b.lastUpdate;
        break;
      default:
        return 0;
    }

    if (typeof aValue === 'string' && typeof bValue === 'string') {
      return sortOrder === 'asc' ? aValue.localeCompare(bValue) : bValue.localeCompare(aValue);
    }

    return sortOrder === 'asc' ? (aValue as number) - (bValue as number) : (bValue as number) - (aValue as number);
  });

  const handleSort = (field: SortField) => {
    if (sortField === field) {
      // Toggle sort order if clicking same column
      setSortOrder(sortOrder === 'asc' ? 'desc' : 'asc');
    } else {
      // Set new sort field with ascending order
      setSortField(field);
      setSortOrder('asc');
    }
  };

  const getSortIndicator = (field: SortField) => {
    if (sortField !== field) return '';
    return sortOrder === 'asc' ? ' ↑' : ' ↓';
  };

  // Load source data
  useEffect(() => {
    const loadSources = async () => {
      try {
        const mountPointsResponse = await mountPointsApi.getMountPoints(1, 100);
        const mountPoints = mountPointsResponse.mountPoints || [];

        // Filter for sources with active connections and coordinates
        const sourcesWithCoords = mountPoints
          .filter((mp: any) => mp.activeSourceCount > 0)
          .filter((mp: any) => (mp.rtcmLatitude && mp.rtcmLongitude) || (mp.latitude && mp.longitude))
          .map((mp: any) => ({
            id: `source-${mp.id}`,
            name: mp.name,
            latitude: (mp.rtcmLatitude && mp.rtcmLatitude >= -90 && mp.rtcmLatitude <= 90)
              ? mp.rtcmLatitude
              : mp.latitude,
            longitude: (mp.rtcmLongitude && mp.rtcmLongitude >= -180 && mp.rtcmLongitude <= 180)
              ? mp.rtcmLongitude
              : mp.longitude,
          }));

        setSources(sourcesWithCoords);
        setMapLoading(false);
      } catch (error) {
        console.error('Failed to load sources:', error);
        setSources([]);
        setMapLoading(false);
      }
    };

    loadSources();
  }, []);

  // Fallback stats when SignalR not available
  const stats = signalRStats ? {
    activeClients: signalRStats.activeClients,
    activeSources: signalRStats.activeSources,
    totalBytesReceived: signalRStats.totalBytesReceived,
    totalBytesSent: signalRStats.totalBytesSent,
    totalBytesTransferred: signalRStats.totalBytesTransferred,
    uptimeFormatted: signalRConnected ? signalRStats.uptimeFormatted : 'Server Offline',
  } : {
    activeClients: 0,
    activeSources: 0,
    totalBytesReceived: 0,
    totalBytesSent: 0,
    totalBytesTransferred: 0,
    uptimeFormatted: 'Waiting for server...',
  };

  return (
    <DashboardLayout>
      <div className={styles.container}>
        <div className={styles.header}>
          <h1 className={styles.title}>Admin Dashboard</h1>
          <p className={styles.subtitle}>System overview and management center</p>
        </div>

        {/* Stats Cards */}
        <div className={styles.statsGrid}>
          <a
            onClick={(e) => {
              e.preventDefault();
              mapRef.current?.scrollIntoView({ behavior: 'smooth' });
            }}
            href="#map"
            className={styles.statCard}
          >
            <div className={styles.statIcon}>🛰️</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Connected Rovers</div>
              <div className={styles.statValue}>{stats?.activeClients ?? 0}</div>
              <div className={styles.statSubtext}>NTRIP clients → View Map</div>
            </div>
          </a>

          <a href="/admin/mountpoints" className={styles.statCard}>
            <div className={styles.statIcon}>📡</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Base Stations</div>
              <div className={styles.statValue}>{stats?.activeSources ?? 0}</div>
              <div className={styles.statSubtext}>Data sources → Manage</div>
            </div>
          </a>

          <a href="/admin/analytics" className={styles.statCard}>
            <div className={styles.statIcon}>📊</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Data Transferred</div>
              <div className={styles.statValue}>{stats?.totalBytesTransferred.toFixed(2) ?? 0} MB</div>
              <div className={styles.statSubtext}>Total session traffic → Analytics</div>
            </div>
          </a>

          <a href="/admin/system-settings" className={styles.statCard}>
            <div className={styles.statIcon}>⏱️</div>
            <div className={styles.statContent}>
              <div className={styles.statLabel}>Server Uptime</div>
              <div className={styles.statValue}>{stats?.uptimeFormatted ?? 'N/A'}</div>
              <div className={styles.statSubtext}>Since last restart → Settings</div>
            </div>
          </a>
        </div>

        {/* Connected Rovers Table */}
        <div className={styles.clientListSection}>
          <h2 className={styles.clientListTitle}>Connected Rovers ({clients.length})</h2>
          <div className={styles.clientList}>
            {clients.length === 0 ? (
              <div className={styles.emptyState}>
                <p>No rovers currently connected</p>
              </div>
            ) : (
              <table className={styles.roversTable}>
                <thead>
                  <tr>
                    <th className={styles.sortableHeader} onClick={() => handleSort('username')}>
                      Username{getSortIndicator('username')}
                    </th>
                    <th className={styles.sortableHeader} onClick={() => handleSort('serial')}>
                      Serial #{getSortIndicator('serial')}
                    </th>
                    <th className={styles.sortableHeader} onClick={() => handleSort('source')}>
                      Source{getSortIndicator('source')}
                    </th>
                    <th>Latitude</th>
                    <th>Longitude</th>
                    <th>Accuracy</th>
                    <th className={styles.sortableHeader} onClick={() => handleSort('status')}>
                      Status{getSortIndicator('status')}
                    </th>
                    <th className={styles.sortableHeader} onClick={() => handleSort('lastUpdate')}>
                      Last Update{getSortIndicator('lastUpdate')}
                    </th>
                  </tr>
                </thead>
                <tbody>
                  {sortedClients.map((client) => (
                    <tr key={client.id} className={client.isStale ? styles.staleRow : ''}>
                      <td>{client.username}</td>
                      <td>{client.serialNumber}</td>
                      <td>{client.mountPoint}</td>
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

        {/* Real-time Map Section */}
        <div ref={mapRef} style={{ marginTop: '40px', paddingTop: '20px', borderTop: '1px solid #e5e7eb' }}>
          <h2 style={{ marginBottom: '16px', fontSize: '18px', fontWeight: '600' }}>
            📍 Real-time Client & Source Positions
          </h2>
          <div style={{
            background: '#fff',
            borderRadius: '8px',
            padding: '16px',
            boxShadow: '0 1px 3px rgba(0, 0, 0, 0.1)',
            height: '600px'
          }}>
            {mapLoading ? (
              <div style={{ textAlign: 'center', padding: '40px', color: '#666' }}>
                Loading map data...
              </div>
            ) : (
              <div style={{ width: '100%', height: '100%' }}>
                <RealTimeMap clients={clients} sources={sources} />
              </div>
            )}
          </div>
        </div>

        {/* User Activity Events */}
        <UserActivityPanel />

        {/* Admin Sections Grid */}
        <div className={styles.sectionsGrid}>
          <div className={styles.section}>
            <h2 className={styles.sectionTitle}>👥 User & Group Management</h2>
            <div className={styles.actionButtons}>
              <a href="/admin/users" className={styles.actionButton}>
                Manage Users
              </a>
              <a href="/admin/groups" className={styles.actionButton}>
                Manage Groups
              </a>
            </div>
          </div>

          <div className={styles.section}>
            <h2 className={styles.sectionTitle}>🔌 Network Configuration</h2>
            <div className={styles.actionButtons}>
              <a href="/admin/mountpoints" className={styles.actionButton}>
                Mount Points
              </a>
              <a href="/admin/caster-config" className={styles.actionButton}>
                Caster Config
              </a>
              <a href="/admin/network-config" className={styles.actionButton}>
                Network Config
              </a>
            </div>
          </div>

          <div className={styles.section}>
            <h2 className={styles.sectionTitle}>📊 Monitoring & Analytics</h2>
            <div className={styles.actionButtons}>
              <a href="/admin/realtime-map" className={styles.actionButton}>
                Real-time Map
              </a>
              <a href="/admin/analytics" className={styles.actionButton}>
                Analytics & Reports
              </a>
              <a href="/admin/activity-log" className={styles.actionButton}>
                Activity Log
              </a>
            </div>
          </div>

          <div className={styles.section}>
            <h2 className={styles.sectionTitle}>⚙️ System Settings</h2>
            <div className={styles.actionButtons}>
              <a href="/admin/system-settings" className={styles.actionButton}>
                System Settings
              </a>
              <a href="/admin/security" className={styles.actionButton}>
                Security Settings
              </a>
              <a href="/admin/logs" className={styles.actionButton}>
                System Logs
              </a>
            </div>
          </div>

          <div className={styles.section}>
            <h2 className={styles.sectionTitle}>🗄️ Database Management</h2>
            <div className={styles.actionButtons}>
              <a href="/admin/database" className={styles.actionButton}>
                Database Tools
              </a>
            </div>
          </div>
        </div>

      </div>
    </DashboardLayout>
  );
};

export default AdminDashboardPage;
