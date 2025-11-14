import React, { useState, useEffect } from 'react';
import { DashboardLayout } from '../../components/Layout/DashboardLayout';
import { emailApi, type EmailTriggerSettings } from '../../services/emailApi';
import { smtpApi, type EmailSmtpSettings } from '../../services/smtpApi';
import { telegramApi, type TelegramSettings } from '../../services/telegramApi';
import styles from './NotificationSettingsPage.module.css';

export const NotificationSettingsPage: React.FC = () => {
  // Trigger settings
  const [triggerSettings, setTriggerSettings] = useState<EmailTriggerSettings | null>(null);

  // SMTP settings
  const [smtpSettings, setSmtpSettings] = useState<EmailSmtpSettings | null>(null);
  const [editingSmtp, setEditingSmtp] = useState(false);
  const [smtpForm, setSmtpForm] = useState<Partial<EmailSmtpSettings>>({});

  // UI state
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [success, setSuccess] = useState<string | null>(null);
  const [testEmail, setTestEmail] = useState('');
  const [sendingTest, setSendingTest] = useState(false);
  const [isSaving, setIsSaving] = useState(false);

  useEffect(() => {
    loadAllSettings();
  }, []);

  const loadAllSettings = async () => {
    try {
      setLoading(true);
      setError(null);
      const [triggers, smtp] = await Promise.all([
        emailApi.getEmailSettings(),
        smtpApi.getSettings(),
      ]);
      setTriggerSettings(triggers);
      setSmtpSettings(smtp);
      setSmtpForm(smtp);
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

  const handleSaveTriggers = async () => {
    if (!triggerSettings) return;

    try {
      setIsSaving(true);
      setError(null);
      setSuccess(null);

      await emailApi.updateEmailSettings(triggerSettings);
      setSuccess('Email trigger settings saved successfully!');
      setTimeout(() => setSuccess(null), 3000);
    } catch (err) {
      setError('Failed to save trigger settings');
      console.error(err);
    } finally {
      setIsSaving(false);
    }
  };

  const handleSaveSmtp = async () => {
    if (!smtpForm) return;

    // Validate required fields
    if (!smtpForm.host?.trim()) {
      setError('SMTP Host is required');
      return;
    }

    if (!smtpForm.port || smtpForm.port < 1 || smtpForm.port > 65535) {
      setError('SMTP Port must be between 1 and 65535');
      return;
    }

    if (!smtpForm.fromEmail?.trim()) {
      setError('From Email is required');
      return;
    }

    try {
      setIsSaving(true);
      setError(null);
      setSuccess(null);

      const result = await smtpApi.updateSettings(smtpForm as EmailSmtpSettings);
      setSmtpSettings(result);
      setEditingSmtp(false);
      setSuccess('SMTP settings saved successfully!');
      setTimeout(() => setSuccess(null), 3000);
    } catch (err) {
      setError('Failed to save SMTP settings');
      console.error(err);
    } finally {
      setIsSaving(false);
    }
  };

  const handleTestSmtp = async () => {
    if (!smtpForm?.fromEmail?.trim()) {
      setError('Please enter a From Email address');
      return;
    }

    try {
      setSendingTest(true);
      setError(null);
      const result = await smtpApi.testConnection(smtpForm.fromEmail);
      if (result.success) {
        setSuccess('SMTP connection successful!');
      } else {
        setError(result.message);
      }
      setTimeout(() => setSuccess(null), 5000);
    } catch (err: any) {
      setError(err.response?.data?.message || 'Failed to test SMTP connection');
      console.error(err);
    } finally {
      setSendingTest(false);
    }
  };

  if (loading) {
    return (
      <DashboardLayout>
        <div className={styles.container}>
          <div style={{ textAlign: 'center', padding: '40px' }}>Loading settings...</div>
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

        {/* SMTP Configuration Card */}
        <div className={styles.card}>
          <div className={styles.cardHeader}>
            <h2>🔧 SMTP Configuration</h2>
            {!editingSmtp && (
              <button
                onClick={() => {
                  setEditingSmtp(true);
                  setSmtpForm(smtpSettings || {});
                }}
                className={styles.editButton}
              >
                ✏️ Edit
              </button>
            )}
          </div>

          {editingSmtp ? (
            <div className={styles.formSection}>
              <div className={styles.formRow}>
                <div className={styles.formGroup}>
                  <label htmlFor="host">SMTP Host *</label>
                  <input
                    id="host"
                    type="text"
                    value={smtpForm.host || ''}
                    onChange={(e) =>
                      setSmtpForm({ ...smtpForm, host: e.target.value })
                    }
                    placeholder="e.g., smtp.gmail.com"
                  />
                </div>

                <div className={styles.formGroup}>
                  <label htmlFor="port">SMTP Port *</label>
                  <input
                    id="port"
                    type="number"
                    value={smtpForm.port || 587}
                    onChange={(e) =>
                      setSmtpForm({ ...smtpForm, port: parseInt(e.target.value) })
                    }
                    placeholder="587 (TLS) or 465 (SSL)"
                  />
                </div>
              </div>

              <div className={styles.formRow}>
                <div className={styles.formGroup}>
                  <label htmlFor="username">Username</label>
                  <input
                    id="username"
                    type="text"
                    value={smtpForm.username || ''}
                    onChange={(e) =>
                      setSmtpForm({ ...smtpForm, username: e.target.value })
                    }
                    placeholder="SMTP username (if required)"
                  />
                </div>

                <div className={styles.formGroup}>
                  <label htmlFor="password">Password</label>
                  <input
                    id="password"
                    type="password"
                    value={smtpForm.password || ''}
                    onChange={(e) =>
                      setSmtpForm({ ...smtpForm, password: e.target.value })
                    }
                    placeholder="Leave blank to keep existing password"
                  />
                </div>
              </div>

              <div className={styles.formRow}>
                <div className={styles.formGroup}>
                  <label htmlFor="fromEmail">From Email *</label>
                  <input
                    id="fromEmail"
                    type="email"
                    value={smtpForm.fromEmail || ''}
                    onChange={(e) =>
                      setSmtpForm({ ...smtpForm, fromEmail: e.target.value })
                    }
                    placeholder="e.g., noreply@ntripcaster.local"
                  />
                </div>

                <div className={styles.formGroup}>
                  <label htmlFor="fromName">From Name</label>
                  <input
                    id="fromName"
                    type="text"
                    value={smtpForm.fromName || ''}
                    onChange={(e) =>
                      setSmtpForm({ ...smtpForm, fromName: e.target.value })
                    }
                    placeholder="e.g., NtripCaster"
                  />
                </div>
              </div>

              <div className={styles.checkboxGroup}>
                <label>
                  <input
                    type="checkbox"
                    checked={smtpForm.enableSsl}
                    onChange={(e) =>
                      setSmtpForm({ ...smtpForm, enableSsl: e.target.checked })
                    }
                  />
                  Enable SSL
                </label>
                <label>
                  <input
                    type="checkbox"
                    checked={smtpForm.enableTls}
                    onChange={(e) =>
                      setSmtpForm({ ...smtpForm, enableTls: e.target.checked })
                    }
                  />
                  Enable TLS
                </label>
                <label>
                  <input
                    type="checkbox"
                    checked={smtpForm.isConfigured}
                    onChange={(e) =>
                      setSmtpForm({ ...smtpForm, isConfigured: e.target.checked })
                    }
                  />
                  Configuration Complete
                </label>
              </div>

              <div className={styles.formActions}>
                <button
                  onClick={handleSaveSmtp}
                  disabled={isSaving}
                  className={styles.saveButton}
                >
                  {isSaving ? '💾 Saving...' : '💾 Save SMTP Settings'}
                </button>
                <button
                  onClick={handleTestSmtp}
                  disabled={sendingTest || !smtpForm.fromEmail}
                  className={styles.testButton}
                >
                  {sendingTest ? '⏳ Testing...' : '🧪 Test Connection'}
                </button>
                <button
                  onClick={() => setEditingSmtp(false)}
                  className={styles.cancelButton}
                >
                  Cancel
                </button>
              </div>
            </div>
          ) : (
            <div className={styles.infoSection}>
              <div className={styles.infoRow}>
                <span className={styles.label}>Host:</span>
                <span className={styles.value}>{smtpSettings?.host || 'Not configured'}</span>
              </div>
              <div className={styles.infoRow}>
                <span className={styles.label}>Port:</span>
                <span className={styles.value}>{smtpSettings?.port || '-'}</span>
              </div>
              <div className={styles.infoRow}>
                <span className={styles.label}>From Email:</span>
                <span className={styles.value}>{smtpSettings?.fromEmail || 'Not configured'}</span>
              </div>
              <div className={styles.infoRow}>
                <span className={styles.label}>From Name:</span>
                <span className={styles.value}>{smtpSettings?.fromName || 'NtripCaster'}</span>
              </div>
              <div className={styles.infoRow}>
                <span className={styles.label}>Status:</span>
                <span className={smtpSettings?.isConfigured ? styles.valueSuccess : styles.valueWarning}>
                  {smtpSettings?.isConfigured ? '✅ Configured' : '⚠️ Not verified'}
                </span>
              </div>
            </div>
          )}
        </div>

        {/* Test Email Card */}
        <div className={styles.card}>
          <div className={styles.cardHeader}>
            <h2>🧪 Send Test Email</h2>
          </div>

          <div className={styles.formSection}>
            <div className={styles.formGroup}>
              <label htmlFor="testEmail">Test Email Address</label>
              <input
                id="testEmail"
                type="email"
                value={testEmail}
                onChange={(e) => setTestEmail(e.target.value)}
                placeholder="Enter email to receive test message"
              />
            </div>

            <button
              onClick={handleTestEmail}
              disabled={sendingTest || !testEmail}
              className={styles.testButton}
            >
              {sendingTest ? '⏳ Sending...' : '🧪 Send Test Email'}
            </button>
          </div>
        </div>

        {/* Email Triggers Card */}
        <div className={styles.card}>
          <div className={styles.cardHeader}>
            <h2>🔔 Email Triggers</h2>
          </div>

          {triggerSettings && (
            <div className={styles.formSection}>
              <div className={styles.toggleGrid}>
                <label className={styles.toggle}>
                  <input
                    type="checkbox"
                    checked={triggerSettings.sendVerificationEmail}
                    onChange={(e) =>
                      setTriggerSettings({
                        ...triggerSettings,
                        sendVerificationEmail: e.target.checked,
                      })
                    }
                  />
                  <span className={styles.toggleLabel}>
                    ✉️ Verification Email
                    <small>Send email when user registers</small>
                  </span>
                </label>

                <label className={styles.toggle}>
                  <input
                    type="checkbox"
                    checked={triggerSettings.sendWelcomeEmail}
                    onChange={(e) =>
                      setTriggerSettings({
                        ...triggerSettings,
                        sendWelcomeEmail: e.target.checked,
                      })
                    }
                  />
                  <span className={styles.toggleLabel}>
                    👋 Welcome Email
                    <small>Send email after verification</small>
                  </span>
                </label>

                <label className={styles.toggle}>
                  <input
                    type="checkbox"
                    checked={triggerSettings.sendSourceOfflineEmail}
                    onChange={(e) =>
                      setTriggerSettings({
                        ...triggerSettings,
                        sendSourceOfflineEmail: e.target.checked,
                      })
                    }
                  />
                  <span className={styles.toggleLabel}>
                    🔴 Source Offline
                    <small>Notify when GNSS source goes offline</small>
                  </span>
                </label>

                <label className={styles.toggle}>
                  <input
                    type="checkbox"
                    checked={triggerSettings.sendSourceOnlineEmail}
                    onChange={(e) =>
                      setTriggerSettings({
                        ...triggerSettings,
                        sendSourceOnlineEmail: e.target.checked,
                      })
                    }
                  />
                  <span className={styles.toggleLabel}>
                    🟢 Source Online
                    <small>Notify when GNSS source comes online</small>
                  </span>
                </label>
              </div>

              <div className={styles.formGroup}>
                <label htmlFor="adminEmail">Admin Notification Email</label>
                <input
                  id="adminEmail"
                  type="email"
                  value={triggerSettings.adminEmailForSourceNotifications}
                  onChange={(e) =>
                    setTriggerSettings({
                      ...triggerSettings,
                      adminEmailForSourceNotifications: e.target.value,
                    })
                  }
                  placeholder="admin@ntripcaster.local"
                />
              </div>

              <button
                onClick={handleSaveTriggers}
                disabled={isSaving}
                className={styles.saveButton}
              >
                {isSaving ? '💾 Saving...' : '💾 Save Email Triggers'}
              </button>
            </div>
          )}
        </div>

        {/* Info Box */}
        <div className={styles.infoBox}>
          <h3>ℹ️ Email Configuration Guide</h3>
          <h4>SMTP Settings:</h4>
          <ul>
            <li>
              <strong>Gmail:</strong> smtp.gmail.com, port 587, enable "Less secure app access" or use App Password
            </li>
            <li>
              <strong>Office 365:</strong> smtp.office365.com, port 587, TLS enabled
            </li>
            <li>
              <strong>Custom Server:</strong> Check with your email provider for correct settings
            </li>
          </ul>
          <h4>Email Triggers:</h4>
          <ul>
            <li>Enable/disable specific email notifications</li>
            <li>Source online/offline notifications go to source owner and admin</li>
            <li>Requires valid SMTP configuration to function</li>
          </ul>
        </div>
      </div>
    </DashboardLayout>
  );
};

export default EmailSettingsPage;
