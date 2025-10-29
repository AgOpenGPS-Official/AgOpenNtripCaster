import { useState, useEffect } from 'react';
import { mountPointsApi } from '../../services/mountPointsApi';
import type { MountPointDto, CreateMountPointRequest, UpdateMountPointRequest } from '../../types';
import styles from './MountPointsManagement.module.css';

export default function MountPointsManagement() {
  const [mountPoints, setMountPoints] = useState<MountPointDto[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [page, setPage] = useState(1);
  const [totalPages, setTotalPages] = useState(1);
  const [showForm, setShowForm] = useState(false);
  const [editingMountPointId, setEditingMountPointId] = useState<number | null>(null);
  const [deleteConfirm, setDeleteConfirm] = useState<{ show: boolean; mountPointId: number | null }>({
    show: false,
    mountPointId: null,
  });

  const [formData, setFormData] = useState({
    name: '',
    description: '',
    sourcePassword: '',
    requireClientAuthentication: true,
    isActive: true,
  });

  // Load mount points
  useEffect(() => {
    loadMountPoints();
  }, [page]);

  const loadMountPoints = async () => {
    try {
      setLoading(true);
      setError(null);
      const response = await mountPointsApi.getMountPoints(page, 10);
      setMountPoints(response.mountPoints);
      setTotalPages(Math.ceil(response.total / response.pageSize));
    } catch (err) {
      setError(`Failed to load mount points: ${err instanceof Error ? err.message : 'Unknown error'}`);
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
    setEditingMountPointId(null);
    setShowForm(true);
  };

  const handleEditClick = (mountPoint: MountPointDto) => {
    setFormData({
      name: mountPoint.name,
      description: mountPoint.description,
      sourcePassword: '',
      requireClientAuthentication: mountPoint.requireClientAuthentication,
      isActive: mountPoint.isActive,
    });
    setEditingMountPointId(mountPoint.id);
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

      if (editingMountPointId) {
        const updateRequest: UpdateMountPointRequest = {
          description: formData.description,
          sourcePassword: formData.sourcePassword || undefined,
          requireClientAuthentication: formData.requireClientAuthentication,
          isActive: formData.isActive,
        };
        await mountPointsApi.updateMountPoint(editingMountPointId, updateRequest);
      } else {
        const createRequest: CreateMountPointRequest = {
          name: formData.name,
          description: formData.description,
          sourcePassword: formData.sourcePassword,
          requireClientAuthentication: formData.requireClientAuthentication,
          isActive: formData.isActive,
        };
        await mountPointsApi.createMountPoint(createRequest);
      }

      setShowForm(false);
      await loadMountPoints();
    } catch (err) {
      setError(`Failed to save mount point: ${err instanceof Error ? err.message : 'Unknown error'}`);
    }
  };

  const handleDeleteClick = (mountPointId: number) => {
    setDeleteConfirm({ show: true, mountPointId });
  };

  const handleConfirmDelete = async () => {
    if (!deleteConfirm.mountPointId) return;

    try {
      setError(null);
      await mountPointsApi.deleteMountPoint(deleteConfirm.mountPointId);
      setDeleteConfirm({ show: false, mountPointId: null });
      await loadMountPoints();
    } catch (err) {
      setError(`Failed to delete mount point: ${err instanceof Error ? err.message : 'Unknown error'}`);
    }
  };

  return (
    <div className={styles.container}>
      <div className={styles.header}>
        <h1>Mount Points Management</h1>
        <button className={styles.createBtn} onClick={handleCreateClick}>
          + Create Mount Point
        </button>
      </div>

      {error && <div className={styles.error}>{error}</div>}

      {loading ? (
        <div className={styles.loading}>Loading mount points...</div>
      ) : mountPoints.length === 0 ? (
        <div className={styles.empty}>No mount points found. Create one to get started.</div>
      ) : (
        <div className={styles.tableContainer}>
          <table className={styles.table}>
            <thead>
              <tr>
                <th>Name</th>
                <th>Description</th>
                <th>Status</th>
                <th>Sources</th>
                <th>Clients</th>
                <th>Auth Required</th>
                <th>Allowed Groups</th>
                <th>Actions</th>
              </tr>
            </thead>
            <tbody>
              {mountPoints.map((mp) => (
                <tr key={mp.id}>
                  <td className={styles.name}>{mp.name}</td>
                  <td className={styles.description}>{mp.description}</td>
                  <td>
                    <span className={`${styles.status} ${mp.isActive ? styles.active : styles.inactive}`}>
                      {mp.isActive ? 'Active' : 'Inactive'}
                    </span>
                  </td>
                  <td>{mp.activeSourceCount}</td>
                  <td>{mp.activeClientCount}</td>
                  <td>{mp.requireClientAuthentication ? 'Yes' : 'No'}</td>
                  <td>{mp.allowedGroupNames.length > 0 ? mp.allowedGroupNames.join(', ') : 'All'}</td>
                  <td>
                    <div className={styles.actions}>
                      <button
                        className={styles.editBtn}
                        onClick={() => handleEditClick(mp)}
                        title="Edit mount point"
                      >
                        Edit
                      </button>
                      <button
                        className={styles.deleteBtn}
                        onClick={() => handleDeleteClick(mp.id)}
                        title="Delete mount point"
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

      {!loading && mountPoints.length > 0 && (
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

      {/* Create/Edit Form Modal */}
      {showForm && (
        <div className={styles.formContainer}>
          <div className={styles.formContent}>
            <h2>{editingMountPointId ? 'Edit Mount Point' : 'Create Mount Point'}</h2>
            <form onSubmit={handleSubmit}>
              <div className={styles.formGroup}>
                <label htmlFor="name">Mount Point Name *</label>
                <input
                  id="name"
                  type="text"
                  value={formData.name}
                  onChange={(e) => setFormData({ ...formData, name: e.target.value })}
                  disabled={editingMountPointId !== null}
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
                  placeholder="Description of this mount point"
                />
              </div>

              <div className={styles.formGroup}>
                <label htmlFor="sourcePassword">Source Password *</label>
                <input
                  id="sourcePassword"
                  type="password"
                  value={formData.sourcePassword}
                  onChange={(e) => setFormData({ ...formData, sourcePassword: e.target.value })}
                  placeholder="Password for GNSS stations"
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
                  {editingMountPointId ? 'Update' : 'Create'}
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

      {/* Delete Confirmation Modal */}
      {deleteConfirm.show && (
        <div className={styles.modal}>
          <div className={styles.modalContent}>
            <h3>Delete Mount Point</h3>
            <p>Are you sure you want to delete this mount point? This action cannot be undone.</p>
            <div className={styles.modalActions}>
              <button className={styles.deleteConfirmBtn} onClick={handleConfirmDelete}>
                Delete
              </button>
              <button className={styles.cancelBtn} onClick={() => setDeleteConfirm({ show: false, mountPointId: null })}>
                Cancel
              </button>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
