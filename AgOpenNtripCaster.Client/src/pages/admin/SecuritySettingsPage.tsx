import React, { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import styles from './SecuritySettingsPage.module.css';
import { securityApi } from '../../services/securityApi';

export const SecuritySettingsPage: React.FC = () => {
  const [securityConfig, setSecurityConfig] = useState<any>(null);

  const [saved, setSaved] = useState(false);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const loadPolicies = async () => {
      try {
        const policies = await securityApi.getPolicies();
        setSecurityConfig(policies);
      } catch (err) {
        console.error('Failed to load security policies:', err);
      } finally {
        setLoading(false);
      }
    };

    loadPolicies();
  }, []);

  const handleChange = (field: string, value: any) => {
    setSecurityConfig((prev: any) => ({ ...prev, [field]: value }));
    setSaved(false);
  };

  const handleSave = async () => {
    try {
      await securityApi.updatePolicies(securityConfig);
      setSaved(true);
      setTimeout(() => setSaved(false), 3000);
    } catch (err) {
      console.error('Failed to save security config:', err);
      alert('Failed to save security settings');
    }
  };

  if (loading) {
    return (
      <DashboardLayout>
        <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', height: '400px' }}>
          <p>Loading security settings...</p>
        </div>
      </DashboardLayout>
    );
  }

  if (!securityConfig) {
    return (
      <DashboardLayout>
        <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', height: '400px' }}>
          <p>Failed to load security settings</p>
        </div>
      </DashboardLayout>
    );
  }

  return (
    <DashboardLayout>
      <div className={styles.container}>
        <div className={styles.header}>
          <h1 className={styles.title}>Security Settings</h1>
          <p className={styles.subtitle}>Configure authentication and security policies</p>
        </div>

        {saved && <div className={styles.successMessage}>Security settings saved successfully!</div>}

        {/* Authentication Policies */}
        <div className={styles.section}>
          <h2 className={styles.sectionTitle}>🔐 Authentication Policies</h2>

          <div className={styles.settingItem}>
            <div className={styles.settingLabel}>
              <input
                type="checkbox"
                checked={securityConfig.requireMfa}
                onChange={(e) => handleChange('requireMfa', e.target.checked)}
                className={styles.checkbox}
              />
              <label>Require Multi-Factor Authentication (MFA)</label>
            </div>
            <p className={styles.description}>Force all admins to use MFA for account access</p>
          </div>

          <div className={styles.formRow}>
            <div className={styles.formGroup}>
              <label>Minimum Password Length</label>
              <input
                type="number"
                value={securityConfig.passwordMinLength}
                onChange={(e) => handleChange('passwordMinLength', parseInt(e.target.value))}
                className={styles.input}
                min="6"
                max="32"
              />
            </div>
            <div className={styles.formGroup}>
              <label>Password Expiration (Days)</label>
              <input
                type="number"
                value={securityConfig.passwordExpireDays}
                onChange={(e) => handleChange('passwordExpireDays', parseInt(e.target.value))}
                className={styles.input}
                min="0"
              />
            </div>
          </div>
        </div>

        {/* Account Lockout */}
        <div className={styles.section}>
          <h2 className={styles.sectionTitle}>🔒 Account Lockout Policy</h2>

          <div className={styles.formRow}>
            <div className={styles.formGroup}>
              <label>Max Failed Login Attempts</label>
              <input
                type="number"
                value={securityConfig.maxLoginAttempts}
                onChange={(e) => handleChange('maxLoginAttempts', parseInt(e.target.value))}
                className={styles.input}
              />
            </div>
            <div className={styles.formGroup}>
              <label>Lockout Duration (Minutes)</label>
              <input
                type="number"
                value={securityConfig.lockoutDurationMinutes}
                onChange={(e) => handleChange('lockoutDurationMinutes', parseInt(e.target.value))}
                className={styles.input}
              />
            </div>
          </div>

          <div className={styles.formGroup}>
            <label>Session Timeout (Minutes)</label>
            <input
              type="number"
              value={securityConfig.sessionTimeoutMinutes}
              onChange={(e) => handleChange('sessionTimeoutMinutes', parseInt(e.target.value))}
              className={styles.input}
            />
            <p className={styles.description}>Time before user is automatically logged out</p>
          </div>
        </div>

        {/* IP Whitelist */}
        <div className={styles.section}>
          <h2 className={styles.sectionTitle}>🌐 IP Whitelist</h2>

          <div className={styles.settingItem}>
            <div className={styles.settingLabel}>
              <input
                type="checkbox"
                checked={securityConfig.ipWhitelistEnabled}
                onChange={(e) => handleChange('ipWhitelistEnabled', e.target.checked)}
                className={styles.checkbox}
              />
              <label>Enable IP Whitelist</label>
            </div>
            <p className={styles.description}>Only allow connections from specific IP addresses</p>
          </div>

          <div className={styles.formGroup}>
            <label>Whitelisted IPs (one per line)</label>
            <textarea
              value={securityConfig.ipWhitelist}
              onChange={(e) => handleChange('ipWhitelist', e.target.value)}
              className={styles.textarea}
              placeholder="192.168.1.1&#10;10.0.0.0/8"
              rows={5}
            />
            <p className={styles.description}>Supports single IPs and CIDR ranges</p>
          </div>
        </div>

        {/* Transport Security */}
        <div className={styles.section}>
          <h2 className={styles.sectionTitle}>🔗 Transport Security</h2>

          <div className={styles.settingItem}>
            <div className={styles.settingLabel}>
              <input
                type="checkbox"
                checked={securityConfig.tlsEnabled}
                onChange={(e) => handleChange('tlsEnabled', e.target.checked)}
                className={styles.checkbox}
              />
              <label>Require TLS/SSL Encryption</label>
            </div>
            <p className={styles.description}>Force encrypted connections for all clients</p>
          </div>
        </div>

        {/* Action Buttons */}
        <div className={styles.actionBar}>
          <button className={styles.saveButton} onClick={handleSave}>
            Save Security Settings
          </button>
          <button className={styles.resetButton}>Reset to Defaults</button>
        </div>
      </div>
    </DashboardLayout>
  );
};

export default SecuritySettingsPage;
