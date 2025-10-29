import React, { useState, useEffect } from 'react';
import { NetworkInfoDto, UpdateNetworkInfoRequest } from '../../types';
import { casterNetworkApi } from '../../services/casterNetworkApi';
import { DashboardLayout } from '../../components/layout/DashboardLayout';
import styles from './NetworkConfigPage.module.css';

export const NetworkConfigPage: React.FC = () => {
  const [networkInfo, setNetworkInfo] = useState<NetworkInfoDto | null>(null);
  const [isEditing, setIsEditing] = useState(false);
  const [isSaving, setIsSaving] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [success, setSuccess] = useState<string | null>(null);
  const [loading, setLoading] = useState(true);

  const [formData, setFormData] = useState<UpdateNetworkInfoRequest>({
    identifier: 'NTRIP',
    operator: 'AgOpen',
    authenticationRequired: 'Y',
    feeRequired: 'N',
    website: 'https://github.com/AgOpenGPS',
    email: 'info@agopenrtk.local',
    startDate: new Date().toISOString().split('T')[0],
    endDate: new Date(new Date().setFullYear(new Date().getFullYear() + 1)).toISOString().split('T')[0],
  });

  // Load network info on mount
  useEffect(() => {
    loadNetworkInfo();
  }, []);

  const loadNetworkInfo = async () => {
    try {
      setLoading(true);
      setError(null);
      const data = await casterNetworkApi.getNetworkInfo();
      setNetworkInfo(data);
      setFormData({
        identifier: data.identifier,
        operator: data.operator,
        authenticationRequired: data.authenticationRequired,
        feeRequired: data.feeRequired,
        website: data.website,
        email: data.email,
        startDate: data.startDate.split('T')[0],
        endDate: data.endDate.split('T')[0],
      });
    } catch (err) {
      setError('Failed to load network configuration');
      console.error(err);
    } finally {
      setLoading(false);
    }
  };

  const handleInputChange = (e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement | HTMLSelectElement>) => {
    const { name, value } = e.target;
    setFormData((prev) => ({
      ...prev,
      [name]: value,
    }));
  };

  const handleSave = async () => {
    try {
      setIsSaving(true);
      setError(null);
      const updated = await casterNetworkApi.updateNetworkInfo(formData);
      setNetworkInfo(updated);
      setIsEditing(false);
      setSuccess('Network configuration updated successfully!');
      setTimeout(() => setSuccess(null), 3000);
    } catch (err) {
      setError('Failed to save network configuration');
      console.error(err);
    } finally {
      setIsSaving(false);
    }
  };

  const handleCancel = () => {
    if (networkInfo) {
      setFormData({
        identifier: networkInfo.identifier,
        operator: networkInfo.operator,
        authenticationRequired: networkInfo.authenticationRequired,
        feeRequired: networkInfo.feeRequired,
        website: networkInfo.website,
        email: networkInfo.email,
        startDate: networkInfo.startDate.split('T')[0],
        endDate: networkInfo.endDate.split('T')[0],
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
          <h1>🌐 Network Configuration (NET)</h1>
          <p>Configure network operator information for sourcetable</p>
        </div>

        {error && <div className={styles.errorBanner}>{error}</div>}
        {success && <div className={styles.successBanner}>{success}</div>}

        <div className={styles.card}>
          <div className={styles.cardHeader}>
            <h2>Network Operator Settings</h2>
            {!isEditing && (
              <button className={styles.editButton} onClick={() => setIsEditing(true)}>
                ✏️ Edit
              </button>
            )}
          </div>

          <div className={styles.formSection}>
            <div className={styles.formGroup}>
              <label htmlFor="identifier">Network Identifier</label>
              <input
                id="identifier"
                name="identifier"
                type="text"
                value={formData.identifier}
                onChange={handleInputChange}
                disabled={!isEditing}
                placeholder="e.g., NTRIP, RTK-NET"
              />
              <small>Unique network identifier (used in NET entry)</small>
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
                placeholder="e.g., AgOpen, My RTK Provider"
              />
              <small>Network operator organization name</small>
            </div>

            <div className={styles.formRow}>
              <div className={styles.formGroup}>
                <label htmlFor="authenticationRequired">Authentication Required</label>
                <select
                  id="authenticationRequired"
                  name="authenticationRequired"
                  value={formData.authenticationRequired}
                  onChange={handleInputChange}
                  disabled={!isEditing}
                >
                  <option value="Y">Yes</option>
                  <option value="N">No</option>
                </select>
                <small>Does the service require authentication?</small>
              </div>

              <div className={styles.formGroup}>
                <label htmlFor="feeRequired">Fee Required</label>
                <select
                  id="feeRequired"
                  name="feeRequired"
                  value={formData.feeRequired}
                  onChange={handleInputChange}
                  disabled={!isEditing}
                >
                  <option value="Y">Yes</option>
                  <option value="N">No</option>
                </select>
                <small>Is there a fee to use the service?</small>
              </div>
            </div>

            <div className={styles.formGroup}>
              <label htmlFor="website">Website</label>
              <input
                id="website"
                name="website"
                type="url"
                value={formData.website}
                onChange={handleInputChange}
                disabled={!isEditing}
                placeholder="https://example.com"
              />
              <small>Website URL for more information</small>
            </div>

            <div className={styles.formGroup}>
              <label htmlFor="email">Contact Email</label>
              <input
                id="email"
                name="email"
                type="email"
                value={formData.email}
                onChange={handleInputChange}
                disabled={!isEditing}
                placeholder="info@example.com"
              />
              <small>Contact email address for support</small>
            </div>

            <div className={styles.formRow}>
              <div className={styles.formGroup}>
                <label htmlFor="startDate">Service Start Date</label>
                <input
                  id="startDate"
                  name="startDate"
                  type="date"
                  value={formData.startDate}
                  onChange={handleInputChange}
                  disabled={!isEditing}
                />
                <small>When the network service started</small>
              </div>

              <div className={styles.formGroup}>
                <label htmlFor="endDate">License End Date</label>
                <input
                  id="endDate"
                  name="endDate"
                  type="date"
                  value={formData.endDate}
                  onChange={handleInputChange}
                  disabled={!isEditing}
                />
                <small>Service license or contract expiration date</small>
              </div>
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

          {networkInfo && (
            <div className={styles.metadata}>
              <p>Last updated: {new Date(networkInfo.updatedAt).toLocaleString()}</p>
            </div>
          )}
        </div>

        <div className={styles.infoBox}>
          <h3>ℹ️ About Network Configuration</h3>
          <p>
            The network configuration defines your service operator information. This appears in the sourcetable (NET entry) and helps
            clients understand your service terms and contact information.
          </p>
          <ul>
            <li>
              <strong>Network Identifier:</strong> Unique name for your network (e.g., "NTRIP")
            </li>
            <li>
              <strong>Operator:</strong> Your organization name (e.g., "AgOpen")
            </li>
            <li>
              <strong>Authentication:</strong> Whether users need to login
            </li>
            <li>
              <strong>Fee:</strong> Whether there's a cost for the service
            </li>
            <li>
              <strong>Website & Email:</strong> Contact information for support
            </li>
            <li>
              <strong>Service Dates:</strong> When your service is valid (start to end)
            </li>
          </ul>
        </div>
      </div>
    </DashboardLayout>
  );
};
