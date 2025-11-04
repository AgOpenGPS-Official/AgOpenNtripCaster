import React, { useState, useEffect } from 'react';
import type { CasterInfoDto, UpdateCasterInfoRequest } from '../../types';
import { casterNetworkApi } from '../../services/casterNetworkApi';
import { DashboardLayout } from '../../components/Layout/DashboardLayout';
import { useDashboardStats } from '../../hooks/useDashboardStats';
import styles from './CasterConfigPage.module.css';

export const CasterConfigPage: React.FC = () => {
  const [casterInfo, setCasterInfo] = useState<CasterInfoDto | null>(null);
  const [isEditing, setIsEditing] = useState(false);
  const [isSaving, setIsSaving] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [success, setSuccess] = useState<string | null>(null);
  const [loading, setLoading] = useState(true);
  const [isConfigCreated, setIsConfigCreated] = useState(true);

  // Get RTCM listener status from dashboard stats
  const { stats } = useDashboardStats();

  const [formData, setFormData] = useState<UpdateCasterInfoRequest>({
    identifier: 'agopencast',
    operator: 'AgOpen NtripCaster',
    nmeaSupport: 0,
    country: 'NL',
    latitude: 52.0,
    longitude: 5.0,
    fallbackHost: '',
    port: 2101,
    description: 'AgOpen GNSS RTK Server',
  });

  // Load caster info on mount
  useEffect(() => {
    loadCasterInfo();
  }, []);

  const loadCasterInfo = async () => {
    try {
      setLoading(true);
      setError(null);
      const data = await casterNetworkApi.getCasterInfo();

      if (data === null) {
        // Configuration not created yet
        setIsConfigCreated(false);
        setCasterInfo(null);
      } else {
        // Configuration exists
        setIsConfigCreated(true);
        setCasterInfo(data);
        setFormData({
          identifier: data.identifier,
          operator: data.operator,
          nmeaSupport: data.nmeaSupport,
          country: data.country,
          latitude: data.latitude,
          longitude: data.longitude,
          fallbackHost: data.fallbackHost,
          port: data.port,
          description: data.description,
        });
      }
    } catch (err) {
      setError('Failed to load caster configuration. Server error.');
      console.error(err);
    } finally {
      setLoading(false);
    }
  };

  const handleInputChange = (e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
    const { name, value } = e.target;
    setFormData((prev) => ({
      ...prev,
      [name]: name === 'port' || name === 'nmeaSupport' ? parseInt(value, 10) : name.includes('latitude') || name.includes('longitude') ? parseFloat(value) : value,
    }));
  };

  const handleSave = async () => {
    try {
      setIsSaving(true);
      setError(null);
      const updated = await casterNetworkApi.updateCasterInfo(formData);
      setCasterInfo(updated);
      setIsConfigCreated(true);
      setIsEditing(false);
      setSuccess('Caster configuration saved successfully!');
      setTimeout(() => setSuccess(null), 3000);
    } catch (err) {
      setError('Failed to save caster configuration');
      console.error(err);
    } finally {
      setIsSaving(false);
    }
  };

  const handleCancel = () => {
    if (casterInfo) {
      setFormData({
        identifier: casterInfo.identifier,
        operator: casterInfo.operator,
        nmeaSupport: casterInfo.nmeaSupport,
        country: casterInfo.country,
        latitude: casterInfo.latitude,
        longitude: casterInfo.longitude,
        fallbackHost: casterInfo.fallbackHost,
        port: casterInfo.port,
        description: casterInfo.description,
      });
    }
    setIsEditing(false);
  };

  if (loading) {
    return (
      <DashboardLayout>
        <div className={styles.container}>
          <div className={styles.loadingSpinner}>Loading...</div>
        </div>
      </DashboardLayout>
    );
  }

  return (
    <DashboardLayout>
      <div className={styles.container}>
        <div className={styles.header}>
          <h1>🗺️ Caster Configuration (CAS)</h1>
          <p>Configure NTRIP Caster server information for sourcetable</p>
        </div>

        {error && <div className={styles.errorBanner}>{error}</div>}
        {success && <div className={styles.successBanner}>{success}</div>}

        {!isConfigCreated && !isEditing && (
          <div className={styles.card} style={{ backgroundColor: '#f0f9ff', borderLeft: '4px solid #3b82f6' }}>
            <div className={styles.cardHeader}>
              <h2>No Configuration Created Yet</h2>
            </div>
            <p style={{ marginBottom: '1rem', color: '#1f2937' }}>
              You haven't created a Caster configuration yet. This configuration is required to identify your NTRIP server to clients
              and will appear in the sourcetable CAS entry.
            </p>
            <button
              className={styles.editButton}
              onClick={() => setIsEditing(true)}
              style={{ marginBottom: '1rem' }}
            >
              ➕ Create Configuration
            </button>
          </div>
        )}

        {(isConfigCreated || isEditing) && (
        <div className={styles.card}>
          <div className={styles.cardHeader}>
            <h2>Caster Server Settings</h2>
            {!isEditing && (
              <button className={styles.editButton} onClick={() => setIsEditing(true)}>
                ✏️ Edit
              </button>
            )}
          </div>

          <div className={styles.formSection}>
            <div className={styles.formGroup}>
              <label htmlFor="identifier">Identifier</label>
              <input
                id="identifier"
                name="identifier"
                type="text"
                value={formData.identifier}
                onChange={handleInputChange}
                disabled={!isEditing}
                placeholder="e.g., agopencast"
              />
              <small>Unique identifier for this caster (used in CAS entry)</small>
            </div>

            <div className={styles.formGroup}>
              <label htmlFor="operator">Operator Name</label>
              <input
                id="operator"
                name="operator"
                type="text"
                value={formData.operator}
                onChange={handleInputChange}
                disabled={!isEditing}
                placeholder="e.g., AgOpen NtripCaster"
              />
              <small>Organization or operator name</small>
            </div>

            <div className={styles.formGroup}>
              <label htmlFor="country">Country Code</label>
              <input
                id="country"
                name="country"
                type="text"
                value={formData.country}
                onChange={handleInputChange}
                disabled={!isEditing}
                placeholder="e.g., NL, DE, US"
                maxLength={2}
              />
              <small>2-letter ISO country code</small>
            </div>

            <div className={styles.formRow}>
              <div className={styles.formGroup}>
                <label htmlFor="latitude">Latitude</label>
                <input
                  id="latitude"
                  name="latitude"
                  type="number"
                  step="0.000001"
                  value={formData.latitude}
                  onChange={handleInputChange}
                  disabled={!isEditing}
                  placeholder="52.0"
                />
                <small>Server location latitude (-90 to 90)</small>
              </div>

              <div className={styles.formGroup}>
                <label htmlFor="longitude">Longitude</label>
                <input
                  id="longitude"
                  name="longitude"
                  type="number"
                  step="0.000001"
                  value={formData.longitude}
                  onChange={handleInputChange}
                  disabled={!isEditing}
                  placeholder="5.0"
                />
                <small>Server location longitude (-180 to 180)</small>
              </div>
            </div>

            <div className={styles.formRow}>
              <div className={styles.formGroup}>
                <div style={{ display: 'flex', alignItems: 'center', gap: '8px', marginBottom: '6px' }}>
                  <label htmlFor="port">Port</label>
                  {stats?.rtcmListenerActive ? (
                    <span style={{
                      display: 'inline-block',
                      padding: '2px 8px',
                      backgroundColor: 'var(--color-success-bg)',
                      color: 'var(--color-success)',
                      borderRadius: '4px',
                      fontSize: '0.85rem',
                      fontWeight: '500'
                    }}>
                      🟢 Running
                    </span>
                  ) : (
                    <span style={{
                      display: 'inline-block',
                      padding: '2px 8px',
                      backgroundColor: 'var(--color-danger-bg)',
                      color: 'var(--color-danger)',
                      borderRadius: '4px',
                      fontSize: '0.85rem',
                      fontWeight: '500'
                    }}>
                      🔴 Stopped
                    </span>
                  )}
                </div>
                <input
                  id="port"
                  name="port"
                  type="number"
                  value={formData.port}
                  onChange={handleInputChange}
                  disabled={!isEditing}
                  placeholder="2101"
                  min="1"
                  max="65535"
                />
                <small>NTRIP server port (default 2101)</small>
              </div>

              <div className={styles.formGroup}>
                <label htmlFor="nmeaSupport">NMEA Support</label>
                <select
                  id="nmeaSupport"
                  name="nmeaSupport"
                  value={formData.nmeaSupport}
                  onChange={(e) =>
                    setFormData((prev) => ({
                      ...prev,
                      nmeaSupport: parseInt(e.target.value, 10),
                    }))
                  }
                  disabled={!isEditing}
                >
                  <option value={0}>No (0)</option>
                  <option value={1}>Yes (1)</option>
                </select>
                <small>NMEA data support (0=no, 1=yes)</small>
              </div>
            </div>

            <div className={styles.formGroup}>
              <label htmlFor="fallbackHost">Fallback Host</label>
              <input
                id="fallbackHost"
                name="fallbackHost"
                type="text"
                value={formData.fallbackHost || ''}
                onChange={handleInputChange}
                disabled={!isEditing}
                placeholder="Optional: backup server address"
              />
              <small>Optional fallback/backup caster host</small>
            </div>

            <div className={styles.formGroup}>
              <label htmlFor="description">Description</label>
              <textarea
                id="description"
                name="description"
                value={formData.description}
                onChange={handleInputChange}
                disabled={!isEditing}
                placeholder="Brief description of this caster"
                rows={3}
              />
              <small>Short description for sourcetable (max 255 chars)</small>
            </div>
          </div>

          {isEditing && (
            <div className={styles.actionButtons}>
              <button className={styles.saveButton} onClick={handleSave} disabled={isSaving}>
                {isSaving ? '💾 Saving...' : '💾 Save Changes'}
              </button>
              <button className={styles.cancelButton} onClick={handleCancel} disabled={isSaving}>
                ❌ Cancel
              </button>
            </div>
          )}

          {casterInfo && (
            <div className={styles.metadata}>
              <p>Last updated: {new Date(casterInfo.updatedAt).toLocaleString()}</p>
            </div>
          )}
        </div>
        )}

        <div className={styles.infoBox}>
          <h3>ℹ️ About Caster Configuration</h3>
          <p>
            The caster configuration defines how your NTRIP server identifies itself to clients. This information appears in the sourcetable
            (CAS entry) that clients download to discover your service.
          </p>
          <ul>
            <li>
              <strong>Identifier:</strong> Unique name for this caster instance
            </li>
            <li>
              <strong>Operator:</strong> Your organization name
            </li>
            <li>
              <strong>Location:</strong> Latitude/Longitude of your server
            </li>
            <li>
              <strong>Port:</strong> NTRIP listening port (usually 2101)
            </li>
            <li>
              <strong>Description:</strong> Brief description clients will see
            </li>
          </ul>
        </div>
      </div>
    </DashboardLayout>
  );
};
