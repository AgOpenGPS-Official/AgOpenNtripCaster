import { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import { myMountPointsApi } from '../../services/myMountPointsApi';
import { mountPointsApi } from '../../services/mountPointsApi';
import { sourcePasswordApi, type SourcePasswordResponse, type GeneratedSourcePasswordResponse } from '../../services/sourcePasswordApi';
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

  // Source password management
  const [sourcePasswordStatus, setSourcePasswordStatus] = useState<SourcePasswordResponse | null>(null);
  const [showGeneratedPassword, setShowGeneratedPassword] = useState(false);
  const [generatedPassword, setGeneratedPassword] = useState<GeneratedSourcePasswordResponse | null>(null);
  const [isGeneratingPassword, setIsGeneratingPassword] = useState(false);
  const [showInstructions, setShowInstructions] = useState(false);

  const [formData, setFormData] = useState({
    name: '',
    description: '',
    sourcePassword: '',
    requireClientAuthentication: true,
    isActive: true,
  });

  useEffect(() => {
    loadSources();
    loadSourcePasswordStatus();
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

  const loadSourcePasswordStatus = async () => {
    try {
      const status = await sourcePasswordApi.getSourcePassword();
      setSourcePasswordStatus(status);
      // If plain password is available, set it as generated password
      if (status.plainPassword) {
        setGeneratedPassword({
          sourcePassword: status.plainPassword,
          message: 'Your current source password'
        });
      }
    } catch (err) {
      console.error('Failed to load source password status:', err);
    }
  };

  const handleGenerateSourcePassword = async () => {
    if (!window.confirm('Are you sure? Your old source password will be replaced.')) {
      return;
    }

    setIsGeneratingPassword(true);
    try {
      setError(null);
      const result = await sourcePasswordApi.resetSourcePassword();
      setGeneratedPassword(result);
      setShowGeneratedPassword(true);
      await loadSourcePasswordStatus();
    } catch (err) {
      setError(`Failed to generate source password: ${err instanceof Error ? err.message : 'Unknown error'}`);
    } finally {
      setIsGeneratingPassword(false);
    }
  };

  const handleCreateClick = () => {
    setFormData({
      name: '',
      description: '',
      sourcePassword: generatedPassword?.sourcePassword || '',
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

    if (!formData.name.trim()) {
      setError('Source name is required');
      return;
    }

    // Ensure password is set (should always be true since it's auto-filled)
    if (!formData.sourcePassword.trim()) {
      setError('Source password is not set. Please go to account settings to generate one.');
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

        {/* Source Credentials Section */}
        <div className={styles.credentialsCard}>
          <div className={styles.credentialsHeader}>
            <h2>🔐 Source Credentials</h2>
            <p>Use these credentials to configure your BaseStations</p>
          </div>

          <div className={styles.credentialsContent}>
            <div className={styles.credentialRow}>
              <div className={styles.credentialLabel}>Current Source Password:</div>
              <div className={styles.credentialValue}>
                {sourcePasswordStatus?.isSet ? (
                  <div className={styles.passwordDisplayRow}>
                    <span className={styles.passwordSet}>
                      <strong>✅ Active</strong>
                    </span>
                    {generatedPassword ? (
                      <>
                        <input
                          type="text"
                          value={generatedPassword.sourcePassword}
                          readOnly
                          className={styles.passwordShowInput}
                        />
                        <button
                          className={styles.copyBtn}
                          onClick={() => {
                            navigator.clipboard.writeText(generatedPassword.sourcePassword);
                            alert('Password copied to clipboard!');
                          }}
                        >
                          📋 Copy
                        </button>
                      </>
                    ) : (
                      <span className={styles.passwordNotVisible}>Generate a new password to see it</span>
                    )}
                  </div>
                ) : (
                  <span className={styles.passwordNotSet}>
                    <strong>Not Set</strong> - Generate one to authenticate your BaseStations
                  </span>
                )}
              </div>
            </div>

            {generatedPassword && showGeneratedPassword && (
              <div className={styles.newPasswordBox}>
                <h3>✅ New Source Password Generated</h3>
                <p>Your new source password is ready. Use it in your BaseStation configuration.</p>
                <div className={styles.passwordDisplay}>
                  <input
                    type="text"
                    value={generatedPassword.sourcePassword}
                    readOnly
                    className={styles.passwordInput}
                  />
                  <button
                    className={styles.copyBtn}
                    onClick={() => {
                      navigator.clipboard.writeText(generatedPassword.sourcePassword);
                      alert('Password copied to clipboard!');
                    }}
                    title="Copy password to clipboard"
                  >
                    📋 Copy
                  </button>
                </div>
                <p className={styles.instructionText}>
                  Use this password in your BaseStation configuration along with the MountPoint name.
                </p>
                <button
                  className={styles.dismissBtn}
                  onClick={() => setShowGeneratedPassword(false)}
                >
                  Done
                </button>
              </div>
            )}

            <div className={styles.actionButtons}>
              <button
                className={styles.generateBtn}
                onClick={handleGenerateSourcePassword}
                disabled={isGeneratingPassword}
              >
                {isGeneratingPassword ? 'Generating...' : '🔄 Generate New Password'}
              </button>
            </div>
          </div>

          {/* Instructions Toggle */}
          <div className={styles.instructionsToggle}>
            <button
              className={styles.instructionsToggleBtn}
              onClick={() => setShowInstructions(!showInstructions)}
            >
              {showInstructions ? '▼ Hide' : '▶ Show'} Setup Instructions
            </button>
          </div>

          {/* Instructions */}
          {showInstructions && (
            <div className={styles.instructionsBox}>
              <h3>📋 How to Configure Your BaseStation</h3>
              <ol>
                <li>Generate a source password using the button above</li>
                <li>Create a new GNSS Source (mount point) in the sources list</li>
                <li>In your BaseStation software, configure:
                  <ul>
                    <li><strong>Server:</strong> Your caster server IP/hostname</li>
                    <li><strong>Port:</strong> 2101</li>
                    <li><strong>MountPoint:</strong> Name of your GNSS Source (e.g., "BaseStationA")</li>
                    <li><strong>Password:</strong> Your generated source password above</li>
                  </ul>
                </li>
                <li>Your BaseStation will authenticate and stream RTCM corrections</li>
              </ol>
            </div>
          )}
        </div>

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
                  <th>Source Connected</th>
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
                    <td>
                      <span className={`${styles.status} ${source.activeSourceCount > 0 ? styles.active : styles.inactive}`}>
                        {source.activeSourceCount > 0 ? 'Connected' : 'Disconnected'}
                      </span>
                    </td>
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
