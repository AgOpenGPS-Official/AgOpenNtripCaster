import { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import { mountPointsApi } from '../../services/mountPointsApi';
import type { MountPointDto } from '../../types';
import styles from './AvailableSourcesPage.module.css';

export default function AvailableSourcesPage() {
  const [sources, setSources] = useState<MountPointDto[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [page, setPage] = useState(1);
  const [totalPages, setTotalPages] = useState(1);

  useEffect(() => {
    loadSources();
  }, [page]);

  const loadSources = async () => {
    try {
      setLoading(true);
      setError(null);
      const response = await mountPointsApi.getMountPoints(page, 10);
      setSources(response.mountPoints);
      setTotalPages(Math.ceil(response.total / response.pageSize));
    } catch (err) {
      setError(`Failed to load sources: ${err instanceof Error ? err.message : 'Unknown error'}`);
    } finally {
      setLoading(false);
    }
  };

  return (
    <DashboardLayout>
      <div className={styles.container}>
        <div className={styles.header}>
          <h1>Available GNSS Sources (Sourcetable)</h1>
          <p className={styles.subtitle}>All available GNSS sources you can connect to</p>
        </div>

        {error && <div className={styles.error}>{error}</div>}

        {loading ? (
          <div className={styles.loading}>Loading sources...</div>
        ) : sources.length === 0 ? (
          <div className={styles.empty}>No sources available yet.</div>
        ) : (
          <div className={styles.tableContainer}>
            <table className={styles.table}>
              <thead>
                <tr>
                  <th>Name</th>
                  <th>Description</th>
                  <th>Status</th>
                  <th>Source Connected</th>
                  <th>Connected Clients</th>
                  <th>Auth Required</th>
                  <th>Allowed Groups</th>
                </tr>
              </thead>
              <tbody>
                {sources.map((source) => (
                  <tr key={source.id}>
                    <td className={styles.name}>{source.name}</td>
                    <td className={styles.description}>{source.description}</td>
                    <td>
                      <span className={`${styles.status} ${source.isActive ? styles.active : styles.inactive}`}>
                        {source.isActive ? 'Active' : 'Inactive'}
                      </span>
                    </td>
                    <td>
                      <span className={`${styles.status} ${source.activeSourceCount > 0 ? styles.active : styles.inactive}`}>
                        {source.activeSourceCount > 0 ? 'Connected' : 'Disconnected'}
                      </span>
                    </td>
                    <td>{source.activeClientCount}</td>
                    <td>{source.requireClientAuthentication ? 'Yes' : 'No'}</td>
                    <td>{source.allowedGroupNames.length > 0 ? source.allowedGroupNames.join(', ') : 'All'}</td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        )}

        {!loading && sources.length > 0 && (
          <div className={styles.pagination}>
            <button
              className={styles.paginationBtn}
              onClick={() => setPage(Math.max(1, page - 1))}
              disabled={page === 1}
            >
              Previous
            </button>
            <span className={styles.pageInfo}>
              Page {page} of {totalPages}
            </span>
            <button
              className={styles.paginationBtn}
              onClick={() => setPage(Math.min(totalPages, page + 1))}
              disabled={page === totalPages}
            >
              Next
            </button>
          </div>
        )}

        <div className={styles.info}>
          <h3>How to connect</h3>
          <p>To connect to any of these sources:</p>
          <ul>
            <li>Use the source name as the mount point identifier</li>
            <li>Contact the source owner for the source password</li>
            <li>Connect your GNSS base station or RTK client via port 2101</li>
            <li>Send your client credentials (username/password) to authenticate</li>
          </ul>
        </div>
      </div>
    </DashboardLayout>
  );
}
