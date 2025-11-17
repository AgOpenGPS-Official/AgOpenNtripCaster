import { useState, useMemo } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import MountPointGroupAccessDialog from '../../components/MountPointGroupAccessDialog';
import { mountPointsApi } from '../../services/mountPointsApi';
import type { MountPointDto, CreateMountPointRequest, UpdateMountPointRequest } from '../../types';
import { useMountPoints } from '../../hooks/useMountPoints';
import { useAuth } from '../../hooks/useAuth';
import styles from './MountPointsManagement.module.css';

export default function MountPointsManagement() {
  // Get real-time mount points with live source/client counts
  const { mountPoints: allMountPoints, loading, error: mountPointsError } = useMountPoints();
  const { user, canWrite } = useAuth();
  const isAdmin = user?.roles?.includes('Admin') ?? false;

  const [error, setError] = useState<string | null>(null);
  const [page, setPage] = useState(1);
  const pageSize = 10;
  const [showForm, setShowForm] = useState(false);
  const [editingMountPointId, setEditingMountPointId] = useState<number | null>(null);
  const [deleteConfirm, setDeleteConfirm] = useState<{ show: boolean; mountPointId: number | null }>({
    show: false,
    mountPointId: null,
  });

  const [groupAccessDialog, setGroupAccessDialog] = useState<{
    show: boolean;
    mountPointId: number | null;
    mountPointName: string;
    allowedGroupNames: string[];
  }>({
    show: false,
    mountPointId: null,
    mountPointName: '',
    allowedGroupNames: [],
  });

  const [formData, setFormData] = useState({
    name: '',
    description: '',
    sourcePassword: '',
    latitude: undefined as number | undefined,
    longitude: undefined as number | undefined,
    // NTRIP 2.0 Sourcetable fields
    identifier: '',
    formatDetails: '',
    carrier: 2,
    navSystem: '',
    network: 'NONE',
    country: 'NLD',
    nmeaRequired: false,
    solution: 0,
    generator: 'sNTRIP',
    compression: 'NONE',
    authentication: 'N',
    feeRequired: false,
    misc: '',
    requireClientAuthentication: true,
    isActive: true,
  });

  // Paginate mount points in memory
  const { mountPoints, totalPages } = useMemo(() => {
    const total = Math.ceil(allMountPoints.length / pageSize);
    const startIndex = (page - 1) * pageSize;
    const endIndex = startIndex + pageSize;
    const paginated = allMountPoints.slice(startIndex, endIndex);

    return {
      mountPoints: paginated,
      totalPages: total,
    };
  }, [allMountPoints, page]);

  const handleCreateClick = () => {
    setFormData({
      name: '',
      description: '',
      sourcePassword: '',
      latitude: undefined,
      longitude: undefined,
      // NTRIP 2.0 Sourcetable fields
      identifier: '',
      formatDetails: '',
      carrier: 2,
      navSystem: '',
      network: 'NONE',
      country: 'NLD',
      nmeaRequired: false,
      solution: 0,
      generator: 'sNTRIP',
      compression: 'NONE',
      authentication: 'N',
      feeRequired: false,
      misc: '',
      requireClientAuthentication: true,
      isActive: true,
    });
    setEditingMountPointId(null);
    setError(null);
    setShowForm(true);
  };

  const handleEditClick = (mountPoint: MountPointDto) => {
    setFormData({
      name: mountPoint.name,
      description: mountPoint.description,
      sourcePassword: '',
      latitude: mountPoint.latitude,
      longitude: mountPoint.longitude,
      // NTRIP 2.0 Sourcetable fields
      identifier: mountPoint.identifier || '',
      formatDetails: mountPoint.formatDetails || '',
      carrier: mountPoint.carrier,
      navSystem: mountPoint.navSystem || '',
      network: mountPoint.network,
      country: mountPoint.country,
      nmeaRequired: mountPoint.nmeaRequired,
      solution: mountPoint.solution,
      generator: mountPoint.generator,
      compression: mountPoint.compression,
      authentication: mountPoint.authentication,
      feeRequired: mountPoint.feeRequired,
      misc: mountPoint.misc || '',
      requireClientAuthentication: mountPoint.requireClientAuthentication,
      isActive: mountPoint.isActive,
    });
    setEditingMountPointId(mountPoint.id);
    setError(null);
    setShowForm(true);
  };

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();

    if (!formData.name.trim()) {
      setError('Mount point name is required');
      return;
    }

    // Only require password for CREATE, not for UPDATE
    if (!editingMountPointId && !formData.sourcePassword.trim()) {
      setError('Source password is required');
      return;
    }

    try {
      setError(null);

      if (editingMountPointId) {
        const updateRequest: UpdateMountPointRequest = {
          description: formData.description,
          sourcePassword: formData.sourcePassword ? formData.sourcePassword : undefined,
          latitude: formData.latitude,
          longitude: formData.longitude,
          // NTRIP 2.0 Sourcetable fields
          identifier: formData.identifier || undefined,
          formatDetails: formData.formatDetails || undefined,
          carrier: formData.carrier,
          navSystem: formData.navSystem || undefined,
          network: formData.network,
          country: formData.country,
          nmeaRequired: formData.nmeaRequired,
          solution: formData.solution,
          generator: formData.generator,
          compression: formData.compression,
          authentication: formData.authentication,
          feeRequired: formData.feeRequired,
          misc: formData.misc || undefined,
          requireClientAuthentication: formData.requireClientAuthentication,
          isActive: formData.isActive,
        };
        await mountPointsApi.updateMountPoint(editingMountPointId, updateRequest);
      } else {
        const createRequest: CreateMountPointRequest = {
          name: formData.name,
          description: formData.description,
          sourcePassword: formData.sourcePassword,
          latitude: formData.latitude,
          longitude: formData.longitude,
          // NTRIP 2.0 Sourcetable fields
          identifier: formData.identifier || undefined,
          formatDetails: formData.formatDetails || undefined,
          carrier: formData.carrier,
          navSystem: formData.navSystem || undefined,
          network: formData.network,
          country: formData.country,
          nmeaRequired: formData.nmeaRequired,
          solution: formData.solution,
          generator: formData.generator,
          compression: formData.compression,
          authentication: formData.authentication,
          feeRequired: formData.feeRequired,
          misc: formData.misc || undefined,
          requireClientAuthentication: formData.requireClientAuthentication,
          isActive: formData.isActive,
        };
        await mountPointsApi.createMountPoint(createRequest);
      }

      setShowForm(false);
      setError(null);
      // Data will update automatically via useMountPoints real-time updates
    } catch (err: any) {
      // Parse backend validation errors
      if (err.response?.data?.errors) {
        const validationErrors = err.response.data.errors;
        const errorMessages = Object.entries(validationErrors)
          .map(([field, messages]) => `${field}: ${(messages as string[]).join(', ')}`)
          .join('\n');
        setError(errorMessages);
      } else if (err.response?.data?.message) {
        setError(err.response.data.message);
      } else {
        setError(`Failed to save mount point: ${err instanceof Error ? err.message : 'Unknown error'}`);
      }
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
      // Data will update automatically via useMountPoints real-time updates
    } catch (err) {
      setError(`Failed to delete mount point: ${err instanceof Error ? err.message : 'Unknown error'}`);
    }
  };

  const handleManageGroupAccess = (mp: MountPointDto) => {
    setGroupAccessDialog({
      show: true,
      mountPointId: mp.id,
      mountPointName: mp.name,
      allowedGroupNames: mp.allowedGroupNames,
    });
  };

  const handleGroupAccessDialogClose = () => {
    setGroupAccessDialog({
      show: false,
      mountPointId: null,
      mountPointName: '',
      allowedGroupNames: [],
    });
  };

  const handleGroupAccessSuccess = () => {
    // Data will update automatically via useMountPoints real-time updates
  };

  return (
    <DashboardLayout>
      <div className={styles.container}>
      <div className={styles.header}>
        <h1>Mount Points Management</h1>
        <button
          className={styles.createBtn}
          onClick={handleCreateClick}
          disabled={!canWrite}
          title={!canWrite ? 'Read-only access - cannot create mount points' : ''}
        >
          + Create Mount Point
        </button>
      </div>

      {(error || mountPointsError) && !showForm && (
        <div className={styles.error}>
          {error || mountPointsError?.message}
        </div>
      )}

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
                <th>Owner</th>
                <th>Description</th>
                <th>Status</th>
                <th>Source Connected</th>
                <th>Connected Clients</th>
                <th>Auth Required</th>
                <th>Allowed Groups</th>
                <th>Actions</th>
              </tr>
            </thead>
            <tbody>
              {mountPoints.map((mp) => (
                <tr key={mp.id}>
                  <td className={styles.name}>{mp.name}</td>
                  <td>{mp.ownerFullName || 'System'}</td>
                  <td className={styles.description}>{mp.description}</td>
                  <td>
                    <span className={`${styles.status} ${mp.isActive ? styles.active : styles.inactive}`}>
                      {mp.isActive ? 'Active' : 'Inactive'}
                    </span>
                  </td>
                  <td>
                    <span className={`${styles.status} ${mp.activeSourceCount > 0 ? styles.active : styles.inactive}`}>
                      {mp.activeSourceCount > 0 ? 'Connected' : 'Disconnected'}
                    </span>
                  </td>
                  <td>{mp.activeClientCount}</td>
                  <td>{mp.requireClientAuthentication ? 'Yes' : 'No'}</td>
                  <td>{mp.allowedGroupNames.length > 0 ? mp.allowedGroupNames.join(', ') : 'All'}</td>
                  <td>
                    <div className={styles.actions}>
                      <button
                        className={styles.editBtn}
                        onClick={() => handleEditClick(mp)}
                        title={!canWrite ? 'Read-only access - cannot edit mount points' : 'Edit mount point'}
                        disabled={!canWrite}
                      >
                        Edit
                      </button>
                      <button
                        className={styles.accessBtn}
                        onClick={() => handleManageGroupAccess(mp)}
                        title={!canWrite ? 'Read-only access - cannot manage access' : 'Manage group access'}
                        disabled={!canWrite}
                      >
                        Access
                      </button>
                      <button
                        className={styles.deleteBtn}
                        onClick={() => handleDeleteClick(mp.id)}
                        title={!canWrite ? 'Read-only access - cannot delete mount points' : 'Delete mount point'}
                        disabled={!canWrite}
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

            {error && <div className={styles.error} style={{ whiteSpace: 'pre-line' }}>{error}</div>}

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
                <label htmlFor="identifier">Identifier (Sourcetable)</label>
                <input
                  id="identifier"
                  type="text"
                  value={formData.identifier}
                  onChange={(e) => setFormData({ ...formData, identifier: e.target.value })}
                  placeholder="e.g., Wachtum (location name for sourcetable)"
                />
                <div style={{ fontSize: '0.75rem', color: '#999', marginTop: '0.25rem' }}>
                  Source identifier shown in NTRIP sourcetable (defaults to description)
                </div>
              </div>

              <div className={styles.formGroup}>
                <label htmlFor="formatDetails">Format Details (Sourcetable)</label>
                <input
                  id="formatDetails"
                  type="text"
                  value={formData.formatDetails}
                  onChange={(e) => setFormData({ ...formData, formatDetails: e.target.value })}
                  placeholder="e.g., 1005(10),1074(1),1084(1),1094(1),1124(1),1230(15)"
                />
                <div style={{ fontSize: '0.75rem', color: '#999', marginTop: '0.25rem' }}>
                  RTCM message types with intervals in seconds: TYPE(interval),TYPE(interval),...
                </div>
              </div>

              {/* Advanced NTRIP 2.0 Configuration - User Fields */}
              <div style={{ marginTop: '1.5rem', padding: '1rem', background: '#f5f5f5', borderRadius: '4px' }}>
                <h3 style={{ marginTop: 0, fontSize: '1rem', marginBottom: '1rem' }}>Advanced Source Configuration</h3>

                <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '1rem' }}>
                  <div className={styles.formGroup}>
                    <label htmlFor="carrier">Carrier Phase</label>
                    <select
                      id="carrier"
                      value={formData.carrier}
                      onChange={(e) => setFormData({ ...formData, carrier: parseInt(e.target.value) })}
                    >
                      <option value="0">0 - No carrier phase</option>
                      <option value="1">1 - L1</option>
                      <option value="2">2 - L1+L2</option>
                    </select>
                  </div>

                  <div className={styles.formGroup}>
                    <label htmlFor="solution">Solution Type</label>
                    <select
                      id="solution"
                      value={formData.solution}
                      onChange={(e) => setFormData({ ...formData, solution: parseInt(e.target.value) })}
                    >
                      <option value="0">0 - Single base station</option>
                      <option value="1">1 - Network</option>
                    </select>
                  </div>

                  <div className={styles.formGroup}>
                    <label htmlFor="navSystem">Navigation Systems</label>
                    <input
                      id="navSystem"
                      type="text"
                      value={formData.navSystem}
                      onChange={(e) => setFormData({ ...formData, navSystem: e.target.value })}
                      placeholder="e.g., GPS+GLO+GAL+BDS (leave empty for auto-detect)"
                    />
                  </div>

                  <div className={styles.formGroup}>
                    <label htmlFor="nmeaRequired">
                      <input
                        id="nmeaRequired"
                        type="checkbox"
                        checked={formData.nmeaRequired}
                        onChange={(e) => setFormData({ ...formData, nmeaRequired: e.target.checked })}
                      />
                      <span>Requires NMEA/GGA Input</span>
                    </label>
                  </div>
                </div>

                <div className={styles.formGroup}>
                  <label htmlFor="misc">Miscellaneous (URL, etc.)</label>
                  <input
                    id="misc"
                    type="text"
                    value={formData.misc}
                    onChange={(e) => setFormData({ ...formData, misc: e.target.value })}
                    placeholder="e.g., http://ntrip.example.com"
                  />
                </div>
              </div>

              {/* Admin-only NTRIP Configuration */}
              {isAdmin && (
                <div style={{ marginTop: '1rem', padding: '1rem', background: '#fff3cd', borderRadius: '4px', border: '1px solid #ffc107' }}>
                  <h3 style={{ marginTop: 0, fontSize: '1rem', marginBottom: '0.5rem', color: '#856404' }}>
                    Admin Only: Caster-wide Defaults Override
                  </h3>
                  <p style={{ fontSize: '0.875rem', color: '#856404', marginBottom: '1rem' }}>
                    These fields use caster-wide defaults. Only change if this source needs different settings.
                  </p>

                  <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '1rem' }}>
                    <div className={styles.formGroup}>
                      <label htmlFor="network">Network Name</label>
                      <input
                        id="network"
                        type="text"
                        value={formData.network}
                        onChange={(e) => setFormData({ ...formData, network: e.target.value })}
                        placeholder="From caster settings"
                      />
                    </div>

                    <div className={styles.formGroup}>
                      <label htmlFor="country">Country Code (ISO 3166)</label>
                      <input
                        id="country"
                        type="text"
                        maxLength={3}
                        value={formData.country}
                        onChange={(e) => setFormData({ ...formData, country: e.target.value.toUpperCase() })}
                        placeholder="From caster settings"
                      />
                    </div>

                    <div className={styles.formGroup}>
                      <label htmlFor="generator">Generator</label>
                      <input
                        id="generator"
                        type="text"
                        value={formData.generator}
                        onChange={(e) => setFormData({ ...formData, generator: e.target.value })}
                        placeholder="From caster settings"
                      />
                    </div>

                    <div className={styles.formGroup}>
                      <label htmlFor="compression">Compression</label>
                      <input
                        id="compression"
                        type="text"
                        value={formData.compression}
                        onChange={(e) => setFormData({ ...formData, compression: e.target.value })}
                        placeholder="From caster settings"
                      />
                    </div>

                    <div className={styles.formGroup}>
                      <label htmlFor="authentication">Authentication</label>
                      <select
                        id="authentication"
                        value={formData.authentication}
                        onChange={(e) => setFormData({ ...formData, authentication: e.target.value })}
                      >
                        <option value="N">N - None</option>
                        <option value="B">B - Basic</option>
                        <option value="D">D - Digest</option>
                        <option value="B,D">B,D - Basic or Digest</option>
                      </select>
                    </div>

                    <div className={styles.formGroup}>
                      <label htmlFor="feeRequired">
                        <input
                          id="feeRequired"
                          type="checkbox"
                          checked={formData.feeRequired}
                          onChange={(e) => setFormData({ ...formData, feeRequired: e.target.checked })}
                        />
                        <span>Fee Required</span>
                      </label>
                    </div>
                  </div>
                </div>
              )}

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

              <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '1rem' }}>
                <div className={styles.formGroup}>
                  <label htmlFor="latitude">Fallback Latitude</label>
                  <input
                    id="latitude"
                    type="number"
                    step="0.000001"
                    value={formData.latitude ?? ''}
                    onChange={(e) => setFormData({ ...formData, latitude: e.target.value ? parseFloat(e.target.value) : undefined })}
                    placeholder="e.g., 52.123456"
                  />
                  <div style={{ fontSize: '0.75rem', color: '#999', marginTop: '0.25rem' }}>
                    Used if RTCM 1005 not received
                  </div>
                </div>

                <div className={styles.formGroup}>
                  <label htmlFor="longitude">Fallback Longitude</label>
                  <input
                    id="longitude"
                    type="number"
                    step="0.000001"
                    value={formData.longitude ?? ''}
                    onChange={(e) => setFormData({ ...formData, longitude: e.target.value ? parseFloat(e.target.value) : undefined })}
                    placeholder="e.g., 5.123456"
                  />
                  <div style={{ fontSize: '0.75rem', color: '#999', marginTop: '0.25rem' }}>
                    Used if RTCM 1005 not received
                  </div>
                </div>
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
                <button
                  type="submit"
                  className={styles.submitBtn}
                  disabled={!canWrite}
                  title={!canWrite ? 'Read-only access - cannot save changes' : ''}
                >
                  {editingMountPointId ? 'Update' : 'Create'}
                </button>
                <button
                  type="button"
                  className={styles.cancelBtn}
                  onClick={() => {
                    setError(null);
                    setShowForm(false);
                  }}
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
              <button
                className={styles.deleteConfirmBtn}
                onClick={handleConfirmDelete}
                disabled={!canWrite}
                title={!canWrite ? 'Read-only access - cannot delete mount points' : ''}
              >
                Delete
              </button>
              <button className={styles.cancelBtn} onClick={() => setDeleteConfirm({ show: false, mountPointId: null })}>
                Cancel
              </button>
            </div>
          </div>
        </div>
      )}

      {/* Group Access Dialog */}
      {groupAccessDialog.show && groupAccessDialog.mountPointId !== null && (
        <MountPointGroupAccessDialog
          mountPointId={groupAccessDialog.mountPointId}
          mountPointName={groupAccessDialog.mountPointName}
          allowedGroupNames={groupAccessDialog.allowedGroupNames}
          onClose={handleGroupAccessDialogClose}
          onSuccess={handleGroupAccessSuccess}
        />
      )}
      </div>
    </DashboardLayout>
  );
}
