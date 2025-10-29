import { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import { myMountPointsApi } from '../../services/myMountPointsApi';
import { mountPointsApi } from '../../services/mountPointsApi';
import type { MountPointDto, CreateMountPointRequest, UpdateMountPointRequest } from '../../types';
import styles from './MySourcesPage.module.css';

export default function MySourcesPage() {
  const [sources, setSources] = useState<MountPointDto[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [page, setPage] = useState(1);
  const [totalPages, setTotalPages] = useState(1);
  const [showForm, setShowForm] = useState(false);
  const [editingSourceId, setEditingSourceId] = useState<number | null>(null);
  const [deleteConfirm, setDeleteConfirm] = useState<{ show: boolean; sourceId: number | null }>({
    show: false,
    sourceId: null,
  });

  const [formData, setFormData] = useState({
    name: '',
    description: '',
    sourcePassword: '',
    requireClientAuthentication: true,
    isActive: true,
  });

  useEffect(() => {
    loadSources();
  }, [page]);

  const loadSources = async () => {
    try {
      setLoading(true);
      setError(null);
      const response = await myMountPointsApi.getMyMountPoints(page, 10);
      setSources(response.mountPoints);
      setTotalPages(Math.ceil(response.total / response.pageSize));
    } catch (err) {
      setError(`Failed to load sources: ${err instanceof Error ? err.message : 'Unknown error'}`);
    } finally {
      setLoading(false);
    }
  };

  const handleCreateClick = () => {
    setFormData({
      name: '',
      description: '',
      sourcePassword: '',
      requireClientAuthentication: true,
      isActive: true,
    });
    setEditingSourceId(null);
    setShowForm(true);
  };

  const handleEditClick = (source: MountPointDto) => {
    setFormData({
      name: source.name,
      description: source.description,
      sourcePassword: '',
      requireClientAuthentication: source.requireClientAuthentication,
      isActive: source.isActive,
    });
    setEditingSourceId(source.id);
    setShowForm(true);
  };

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();

    if (!formData.name.trim() || !formData.sourcePassword.trim()) {
      setError('Name and source password are required');
      return;
    }

    try {
      setError(null);

      if (editingSourceId) {
        const updateRequest: UpdateMountPointRequest = {
          description: formData.description,
          sourcePassword: formData.sourcePassword || undefined,
          requireClientAuthentication: formData.requireClientAuthentication,
          isActive: formData.isActive,
        };
        await mountPointsApi.updateMountPoint(editingSourceId, updateRequest);
      } else {
        const createRequest: CreateMountPointRequest = {
          name: formData.name,
          description: formData.description,
          sourcePassword: formData.sourcePassword,
          requireClientAuthentication: formData.requireClientAuthentication,
          isActive: formData.isActive,
        };
        await myMountPointsApi.createMountPoint(createRequest);
      }

      setShowForm(false);
      await loadSources();
    } catch (err) {
      setError(`Failed to save source: ${err instanceof Error ? err.message : 'Unknown error'}`);
    }
  };

  const handleDeleteClick = (sourceId: number) => {
    setDeleteConfirm({ show: true, sourceId });
  };

  const handleConfirmDelete = async () => {
    if (!deleteConfirm.sourceId) return;

    try {
      setError(null);
      await myMountPointsApi.deleteMountPoint(deleteConfirm.sourceId);
      setDeleteConfirm({ show: false, sourceId: null });
      await loadSources();
    } catch (err) {
      setError(`Failed to delete source: ${err instanceof Error ? err.message : 'Unknown error'}`);
    }
  };

  return (
    <DashboardLayout>
      <div className={styles.container}>
        <div className={styles.header}>
          <h1>My GNSS Sources</h1>
          <button className={styles.createBtn} onClick={handleCreateClick}>
            + Create Source
          </button>
        </div>

        {error && <div className={styles.error}>{error}</div>}

        {loading ? (
          <div className={styles.loading}>Loading sources...</div>
        ) : sources.length === 0 ? (
          <div className={styles.empty}>No sources created. Create one to get started.</div>
        ) : (
          <div className={styles.tableContainer}>
            <table className={styles.table}>
              <thead>
                <tr>
                  <th>Name</th>
                  <th>Description</th>
                  <th>Status</th>
                  <th>Connected Sources</th>
                  <th>Connected Clients</th>
                  <th>Auth Required</th>
                  <th>Actions</th>
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
                    <td>{source.activeSourceCount}</td>
                    <td>{source.activeClientCount}</td>
                    <td>{source.requireClientAuthentication ? 'Yes' : 'No'}</td>
                    <td>
                      <div className={styles.actions}>
                        <button
                          className={styles.editBtn}
                          onClick={() => handleEditClick(source)}
                          title="Edit source"
                        >
                          Edit
                        </button>
                        <button
                          className={styles.deleteBtn}
                          onClick={() => handleDeleteClick(source.id)}
                          title="Delete source"
                        >
                          Delete
                        </button>
                      </div>
                    </td>
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

        {showForm && (
          <div className={styles.formContainer}>
            <div className={styles.formContent}>
              <h2>{editingSourceId ? 'Edit Source' : 'Create GNSS Source'}</h2>
              <form onSubmit={handleSubmit}>
                <div className={styles.formGroup}>
                  <label htmlFor="name">Source Name (Mount Point) *</label>
                  <input
                    id="name"
                    type="text"
                    value={formData.name}
                    onChange={(e) => setFormData({ ...formData, name: e.target.value })}
                    disabled={editingSourceId !== null}
                    placeholder="e.g., STATION_A"
                    required
                  />
                </div>

                <div className={styles.formGroup}>
                  <label htmlFor="description">Description</label>
                  <textarea
                    id="description"
                    value={formData.description}
                    onChange={(e) => setFormData({ ...formData, description: e.target.value })}
                    placeholder="Description of this GNSS source"
                  />
                </div>

                <div className={styles.formGroup}>
                  <label htmlFor="sourcePassword">Source Password *</label>
                  <input
                    id="sourcePassword"
                    type="password"
                    value={formData.sourcePassword}
                    onChange={(e) => setFormData({ ...formData, sourcePassword: e.target.value })}
                    placeholder="Password for GNSS base station connection"
                    required
                  />
                </div>

                <div className={styles.formGroup}>
                  <label htmlFor="requireClientAuth">
                    <input
                      id="requireClientAuth"
                      type="checkbox"
                      checked={formData.requireClientAuthentication}
                      onChange={(e) =>
                        setFormData({ ...formData, requireClientAuthentication: e.target.checked })
                      }
                    />
                    <span>Require Client Authentication</span>
                  </label>
                </div>

                <div className={styles.formGroup}>
                  <label htmlFor="isActive">
                    <input
                      id="isActive"
                      type="checkbox"
                      checked={formData.isActive}
                      onChange={(e) => setFormData({ ...formData, isActive: e.target.checked })}
                    />
                    <span>Active</span>
                  </label>
                </div>

                <div className={styles.formActions}>
                  <button type="submit" className={styles.submitBtn}>
                    {editingSourceId ? 'Update' : 'Create'}
                  </button>
                  <button
                    type="button"
                    className={styles.cancelBtn}
                    onClick={() => setShowForm(false)}
                  >
                    Cancel
                  </button>
                </div>
              </form>
            </div>
          </div>
        )}

        {deleteConfirm.show && (
          <div className={styles.modal}>
            <div className={styles.modalContent}>
              <h3>Delete Source</h3>
              <p>Are you sure you want to delete this source? This action cannot be undone.</p>
              <div className={styles.modalActions}>
                <button className={styles.deleteConfirmBtn} onClick={handleConfirmDelete}>
                  Delete
                </button>
                <button className={styles.cancelBtn} onClick={() => setDeleteConfirm({ show: false, sourceId: null })}>
                  Cancel
                </button>
              </div>
            </div>
          </div>
        )}
      </div>
    </DashboardLayout>
  );
}
