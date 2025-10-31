import React, { useState } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import styles from './SystemSettingsPage.module.css';

export const SystemSettingsPage: React.FC = () => {
  const [emailConfig, setEmailConfig] = useState({
    smtpServer: 'smtp.gmail.com',
    smtpPort: 587,
    senderEmail: 'ntrip@example.com',
    senderPassword: '••••••••',
    useTls: true,
  });

  const [loggingConfig, setLoggingConfig] = useState({
    logLevel: 'Information',
    maxLogSize: 100,
    retentionDays: 30,
    enableConsoleLogging: true,
    enableFileLogging: true,
  });

  const [saved, setSaved] = useState(false);

  const handleEmailChange = (field: string, value: any) => {
    setEmailConfig((prev) => ({ ...prev, [field]: value }));
    setSaved(false);
  };

  const handleLoggingChange = (field: string, value: any) => {
    setLoggingConfig((prev) => ({ ...prev, [field]: value }));
    setSaved(false);
  };

  const handleSaveEmail = async () => {
    try {
      // API call would go here
      console.log('Saving email config:', emailConfig);
      setSaved(true);
      setTimeout(() => setSaved(false), 3000);
    } catch (err) {
      console.error('Failed to save email config:', err);
    }
  };

  const handleSaveLogging = async () => {
    try {
      // API call would go here
      console.log('Saving logging config:', loggingConfig);
      setSaved(true);
      setTimeout(() => setSaved(false), 3000);
    } catch (err) {
      console.error('Failed to save logging config:', err);
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

        {/* Email Configuration */}
        <div className={styles.settingsSection}>
          <h2 className={styles.sectionTitle}>📧 Email Configuration</h2>
          <p className={styles.sectionDescription}>Configure SMTP settings for system notifications and alerts</p>

          <div className={styles.settingsForm}>
            <div className={styles.formGroup}>
              <label className={styles.label}>SMTP Server</label>
              <input
                type="text"
                value={emailConfig.smtpServer}
                onChange={(e) => handleEmailChange('smtpServer', e.target.value)}
                className={styles.input}
              />
            </div>

            <div className={styles.formRow}>
              <div className={styles.formGroup}>
                <label className={styles.label}>SMTP Port</label>
                <input
                  type="number"
                  value={emailConfig.smtpPort}
                  onChange={(e) => handleEmailChange('smtpPort', parseInt(e.target.value))}
                  className={styles.input}
                />
              </div>
              <div className={styles.formGroup}>
                <label className={styles.label}>
                  <input
                    type="checkbox"
                    checked={emailConfig.useTls}
                    onChange={(e) => handleEmailChange('useTls', e.target.checked)}
                    className={styles.checkbox}
                  />
                  Use TLS/SSL
                </label>
              </div>
            </div>

            <div className={styles.formGroup}>
              <label className={styles.label}>Sender Email</label>
              <input
                type="email"
                value={emailConfig.senderEmail}
                onChange={(e) => handleEmailChange('senderEmail', e.target.value)}
                className={styles.input}
              />
            </div>

            <div className={styles.formGroup}>
              <label className={styles.label}>Sender Password</label>
              <input
                type="password"
                value={emailConfig.senderPassword}
                onChange={(e) => handleEmailChange('senderPassword', e.target.value)}
                className={styles.input}
              />
            </div>

            <button className={styles.saveButton} onClick={handleSaveEmail}>
              Save Email Settings
            </button>
          </div>
        </div>

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

            <button className={styles.saveButton} onClick={handleSaveLogging}>
              Save Logging Settings
            </button>
          </div>
        </div>

        {/* Backup & Maintenance */}
        <div className={styles.settingsSection}>
          <h2 className={styles.sectionTitle}>💾 Backup & Maintenance</h2>
          <p className={styles.sectionDescription}>Database backup and system maintenance tools</p>

          <div className={styles.actionButtons}>
            <button className={styles.actionButton}>
              📥 Create Database Backup
            </button>
            <button className={styles.actionButton}>
              🗑️ Clean Old Logs
            </button>
            <button className={styles.actionButton}>
              🔄 Restart Server
            </button>
          </div>
        </div>
      </div>
    </DashboardLayout>
  );
};

export default SystemSettingsPage;
