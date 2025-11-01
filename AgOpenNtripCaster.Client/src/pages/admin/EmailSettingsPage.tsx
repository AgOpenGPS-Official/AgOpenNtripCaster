import React, { useState, useEffect } from 'react';
import { DashboardLayout } from '../../components/Layout/DashboardLayout';
import { emailApi, type EmailTriggerSettings } from '../../services/emailApi';
import styles from './EmailSettingsPage.module.css';

export const EmailSettingsPage: React.FC = () => {
  const [settings, setSettings] = useState<EmailTriggerSettings | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [success, setSuccess] = useState<string | null>(null);
  const [testEmail, setTestEmail] = useState('');
  const [sendingTest, setSendingTest] = useState(false);
  const [isSaving, setIsSaving] = useState(false);

  useEffect(() => {
    loadSettings();
  }, []);

  const loadSettings = async () => {
    try {
      setLoading(true);
      setError(null);
      const data = await emailApi.getEmailSettings();
      setSettings(data);
    } catch (err) {
      setError('Failed to load email settings');
      console.error(err);
    } finally {
      setLoading(false);
    }
  };

  const handleTestEmail = async () => {
    if (!testEmail) {
      setError('Please enter an email address for the test');
      return;
    }

    try {
      setSendingTest(true);
      setError(null);
      await emailApi.sendTestEmail(testEmail);
      setSuccess(`Test email sent to ${testEmail}! Check your inbox.`);
      setTimeout(() => setSuccess(null), 5000);
    } catch (err) {
      setError('Failed to send test email. Check your SMTP configuration.');
      console.error(err);
    } finally {
      setSendingTest(false);
    }
  };

  const handleToggle = (key: keyof EmailTriggerSettings) => {
    if (!settings) return;
    setSettings({
      ...settings,
      [key]: !settings[key],
    });
  };

  const handleEmailChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    if (!settings) return;
    setSettings({
      ...settings,
      adminEmailForSourceNotifications: e.target.value,
    });
  };

  const handleSave = async () => {
    if (!settings) return;

    try {
      setIsSaving(true);
      setError(null);
      await emailApi.updateEmailSettings(settings);
      setSuccess('Email settings saved successfully!');
      setTimeout(() => setSuccess(null), 3000);
    } catch (err) {
      setError('Failed to save email settings');
      console.error(err);
    } finally {
      setIsSaving(false);
    }
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

  if (!settings) {
    return (
      <DashboardLayout>
        <div className={styles.container}>
          <div className={styles.errorBanner}>Failed to load email settings</div>
        </div>
      </DashboardLayout>
    );
  }

  return (
    <DashboardLayout>
      <div className={styles.container}>
        <div className={styles.header}>
          <h1>📧 Email Settings</h1>
          <p>Configure email notifications and SMTP settings</p>
        </div>

        {error && <div className={styles.errorBanner}>{error}</div>}
        {success && <div className={styles.successBanner}>{success}</div>}

        {/* Test Email Section */}
        <div className={styles.card}>
          <div className={styles.cardHeader}>
            <h2>🧪 Send Test Email</h2>
          </div>
          <div className={styles.formSection}>
            <p>Send a test email to verify your SMTP configuration is working correctly.</p>
            <div className={styles.testEmailForm}>
              <input
                type="email"
                value={testEmail}
                onChange={(e) => setTestEmail(e.target.value)}
                placeholder="Enter email address to test"
                className={styles.input}
              />
              <button
                onClick={handleTestEmail}
                disabled={sendingTest || !testEmail}
                className={styles.testButton}
              >
                {sendingTest ? '📤 Sending...' : '📤 Send Test Email'}
              </button>
            </div>
          </div>
        </div>

        {/* Email Triggers Section */}
        <div className={styles.card}>
          <div className={styles.cardHeader}>
            <h2>🔔 Email Triggers</h2>
          </div>
          <div className={styles.formSection}>
            <p>Enable or disable automatic email notifications for different events.</p>

            <div className={styles.triggerGroup}>
              <div className={styles.triggerItem}>
                <div className={styles.triggerLabel}>
                  <h3>Verification Email</h3>
                  <p>Send email verification link when new users register</p>
                </div>
                <label className={styles.toggle}>
                  <input
                    type="checkbox"
                    checked={settings.sendVerificationEmail}
                    onChange={() => handleToggle('sendVerificationEmail')}
                  />
                  <span className={styles.toggleSlider}></span>
                </label>
              </div>

              <div className={styles.triggerItem}>
                <div className={styles.triggerLabel}>
                  <h3>Welcome Email</h3>
                  <p>Send welcome email after successful email verification</p>
                </div>
                <label className={styles.toggle}>
                  <input
                    type="checkbox"
                    checked={settings.sendWelcomeEmail}
                    onChange={() => handleToggle('sendWelcomeEmail')}
                  />
                  <span className={styles.toggleSlider}></span>
                </label>
              </div>

              <div className={styles.triggerItem}>
                <div className={styles.triggerLabel}>
                  <h3>Source Offline Notification</h3>
                  <p>Send email when a GNSS source goes offline</p>
                </div>
                <label className={styles.toggle}>
                  <input
                    type="checkbox"
                    checked={settings.sendSourceOfflineEmail}
                    onChange={() => handleToggle('sendSourceOfflineEmail')}
                  />
                  <span className={styles.toggleSlider}></span>
                </label>
              </div>

              <div className={styles.triggerItem}>
                <div className={styles.triggerLabel}>
                  <h3>Source Online Notification</h3>
                  <p>Send email when a GNSS source comes back online</p>
                </div>
                <label className={styles.toggle}>
                  <input
                    type="checkbox"
                    checked={settings.sendSourceOnlineEmail}
                    onChange={() => handleToggle('sendSourceOnlineEmail')}
                  />
                  <span className={styles.toggleSlider}></span>
                </label>
              </div>
            </div>
          </div>
        </div>

        {/* Admin Notification Email Section */}
        <div className={styles.card}>
          <div className={styles.cardHeader}>
            <h2>👤 Admin Notifications</h2>
          </div>
          <div className={styles.formSection}>
            <div className={styles.formGroup}>
              <label htmlFor="adminEmail">Admin Email for Source Notifications</label>
              <input
                id="adminEmail"
                type="email"
                value={settings.adminEmailForSourceNotifications}
                onChange={handleEmailChange}
                placeholder="admin@example.com"
              />
              <small>Email address to notify when GNSS sources go offline/online</small>
            </div>
          </div>
        </div>

        {/* Save Button */}
        <div className={styles.actions}>
          <button
            onClick={handleSave}
            disabled={isSaving}
            className={styles.saveButton}
          >
            {isSaving ? '💾 Saving...' : '💾 Save Settings'}
          </button>
        </div>

        {/* Info Box */}
        <div className={styles.infoBox}>
          <h3>ℹ️ About Email Configuration</h3>
          <p>
            These settings control which email notifications are automatically sent by the system. Before using email features, make sure
            you have configured your SMTP server in the application settings.
          </p>
          <h4>Email Triggers:</h4>
          <ul>
            <li><strong>Verification Email:</strong> Sent when a new user registers (if enabled)</li>
            <li><strong>Welcome Email:</strong> Sent after user verifies their email address</li>
            <li><strong>Source Notifications:</strong> Sent when GNSS sources go offline or come back online</li>
          </ul>
        </div>
      </div>
    </DashboardLayout>
  );
};
