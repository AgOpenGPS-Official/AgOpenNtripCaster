import React, { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import { useAuth } from '../../hooks/useAuth';
import styles from './SystemSettingsPage.module.css';
import { systemSettingsApi } from '../../services/systemSettingsApi';

export const SystemSettingsPage: React.FC = () => {
  const { canWrite } = useAuth();
  const [loggingConfig, setLoggingConfig] = useState({
    logLevel: 'Information',
    maxLogSize: 100,
    retentionDays: 30,
    enableConsoleLogging: true,
    enableFileLogging: true,
  });

  const [saved, setSaved] = useState(false);

  useEffect(() => {
    const loadSettings = async () => {
      try {
        const settings = await systemSettingsApi.getSettings();
        setLoggingConfig(settings.loggingConfig);
      } catch (err) {
        console.error('Failed to load settings:', err);
      }
    };

    loadSettings();
  }, []);

  const handleLoggingChange = (field: string, value: any) => {
    setLoggingConfig((prev) => ({ ...prev, [field]: value }));
    setSaved(false);
  };

  const handleSaveLogging = async () => {
    try {
      await systemSettingsApi.updateLoggingSettings(loggingConfig);
      setSaved(true);
      setTimeout(() => setSaved(false), 3000);
    } catch (err) {
      console.error('Failed to save logging config:', err);
      alert('Failed to save logging settings');
    }
  };

  return (
    <DashboardLayout>
      <div className={styles.container}>
        <div className={styles.header}>
          <h1 className={styles.title}>System Settings</h1>
          <p className={styles.subtitle}>Configure system-wide settings</p>
        </div>

        {saved && <div className={styles.successMessage}>Settings saved successfully!</div>}

        {/* Logging Configuration */}
        <div className={styles.settingsSection}>
          <h2 className={styles.sectionTitle}>📝 Logging Configuration</h2>
          <p className={styles.sectionDescription}>Configure logging levels and retention policies</p>

          <div className={styles.settingsForm}>
            <div className={styles.formGroup}>
              <label className={styles.label}>Log Level</label>
              <select
                value={loggingConfig.logLevel}
                onChange={(e) => handleLoggingChange('logLevel', e.target.value)}
                className={styles.input}
              >
                <option value="Debug">Debug</option>
                <option value="Information">Information</option>
                <option value="Warning">Warning</option>
                <option value="Error">Error</option>
                <option value="Critical">Critical</option>
              </select>
            </div>

            <div className={styles.formRow}>
              <div className={styles.formGroup}>
                <label className={styles.label}>Max Log Size (MB)</label>
                <input
                  type="number"
                  value={loggingConfig.maxLogSize}
                  onChange={(e) => handleLoggingChange('maxLogSize', parseInt(e.target.value))}
                  className={styles.input}
                />
              </div>
              <div className={styles.formGroup}>
                <label className={styles.label}>Retention (Days)</label>
                <input
                  type="number"
                  value={loggingConfig.retentionDays}
                  onChange={(e) => handleLoggingChange('retentionDays', parseInt(e.target.value))}
                  className={styles.input}
                />
              </div>
            </div>

            <div className={styles.checkboxGroup}>
              <label className={styles.label}>
                <input
                  type="checkbox"
                  checked={loggingConfig.enableConsoleLogging}
                  onChange={(e) => handleLoggingChange('enableConsoleLogging', e.target.checked)}
                  className={styles.checkbox}
                />
                Enable Console Logging
              </label>
              <label className={styles.label}>
                <input
                  type="checkbox"
                  checked={loggingConfig.enableFileLogging}
                  onChange={(e) => handleLoggingChange('enableFileLogging', e.target.checked)}
                  className={styles.checkbox}
                />
                Enable File Logging
              </label>
            </div>

            <button
              className={styles.saveButton}
              onClick={handleSaveLogging}
              disabled={!canWrite}
              title={!canWrite ? 'Read-only access - cannot save settings' : ''}
            >
              Save Logging Settings
            </button>
          </div>
        </div>

        {/* Backup & Maintenance */}
        <div className={styles.settingsSection}>
          <h2 className={styles.sectionTitle}>💾 Backup & Maintenance</h2>
          <p className={styles.sectionDescription}>Database backup and system maintenance tools</p>

          <div className={styles.actionButtons}>
            <button
              className={styles.actionButton}
              disabled={!canWrite}
              title={!canWrite ? 'Read-only access - cannot create backup' : ''}
            >
              📥 Create Database Backup
            </button>
            <button
              className={styles.actionButton}
              disabled={!canWrite}
              title={!canWrite ? 'Read-only access - cannot clean logs' : ''}
            >
              🗑️ Clean Old Logs
            </button>
            <button
              className={styles.actionButton}
              disabled={!canWrite}
              title={!canWrite ? 'Read-only access - cannot restart server' : ''}
            >
              🔄 Restart Server
            </button>
          </div>
        </div>
      </div>
    </DashboardLayout>
  );
};

export default SystemSettingsPage;
