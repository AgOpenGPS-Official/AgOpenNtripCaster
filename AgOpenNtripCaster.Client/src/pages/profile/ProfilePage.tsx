import React, { useState, useEffect } from 'react';
import { useNavigate } from 'react-router-dom';
import { useAuth } from '../../hooks/useAuth';
import { DashboardLayout } from '../../components/Layout/DashboardLayout';
import { usersApi } from '../../services/usersApi';
import { authApi } from '../../services/auth';
import styles from './ProfilePage.module.css';

interface UpdateProfileForm {
  fullName: string;
  email: string;
}

interface ChangePasswordForm {
  currentPassword: string;
  newPassword: string;
  confirmPassword: string;
}

export const ProfilePage: React.FC = () => {
  const { user, logout } = useAuth();
  const navigate = useNavigate();
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [success, setSuccess] = useState<string | null>(null);

  // Profile edit state
  const [isEditingProfile, setIsEditingProfile] = useState(false);
  const [profileForm, setProfileForm] = useState<UpdateProfileForm>({
    fullName: user?.fullName || '',
    email: user?.email || '',
  });

  // Password change state
  const [isChangingPassword, setIsChangingPassword] = useState(false);
  const [passwordForm, setPasswordForm] = useState<ChangePasswordForm>({
    currentPassword: '',
    newPassword: '',
    confirmPassword: '',
  });

  // Delete profile state
  const [showDeleteConfirm, setShowDeleteConfirm] = useState(false);
  const [deletePassword, setDeletePassword] = useState('');

  useEffect(() => {
    if (user) {
      setProfileForm({
        fullName: user.fullName || '',
        email: user.email || '',
      });
    }
  }, [user]);

  const handleSaveProfile = async () => {
    if (!user?.id) return;

    if (!profileForm.fullName.trim()) {
      setError('Full name is required');
      return;
    }

    if (!profileForm.email.trim()) {
      setError('Email is required');
      return;
    }

    try {
      setLoading(true);
      setError(null);
      setSuccess(null);

      await usersApi.updateUser(user.id, {
        fullName: profileForm.fullName,
        email: profileForm.email,
      });

      setSuccess('Profile updated successfully!');
      setIsEditingProfile(false);
      setTimeout(() => setSuccess(null), 3000);
    } catch (err) {
      setError('Failed to update profile');
      console.error(err);
    } finally {
      setLoading(false);
    }
  };

  const handleChangePassword = async () => {
    if (!passwordForm.currentPassword) {
      setError('Current password is required');
      return;
    }

    if (!passwordForm.newPassword) {
      setError('New password is required');
      return;
    }

    if (passwordForm.newPassword.length < 8) {
      setError('Password must be at least 8 characters long');
      return;
    }

    if (passwordForm.newPassword !== passwordForm.confirmPassword) {
      setError('New passwords do not match');
      return;
    }

    try {
      setLoading(true);
      setError(null);
      setSuccess(null);

      await authApi.changePassword({
        currentPassword: passwordForm.currentPassword,
        newPassword: passwordForm.newPassword,
      });

      setSuccess('Password changed successfully!');
      setIsChangingPassword(false);
      setPasswordForm({
        currentPassword: '',
        newPassword: '',
        confirmPassword: '',
      });
      setTimeout(() => setSuccess(null), 3000);
    } catch (err: any) {
      setError(err.response?.data?.message || 'Failed to change password');
      console.error(err);
    } finally {
      setLoading(false);
    }
  };

  const handleDeleteProfile = async () => {
    if (!user?.id) return;

    if (!deletePassword) {
      setError('Password is required to delete profile');
      return;
    }

    try {
      setLoading(true);
      setError(null);
      setSuccess(null);

      // First verify password by attempting to change it with same password
      // This is a security check - in production, could have dedicated endpoint
      try {
        await authApi.changePassword({
          currentPassword: deletePassword,
          newPassword: deletePassword,
        });
      } catch (err) {
        setError('Incorrect password. Profile not deleted.');
        setLoading(false);
        return;
      }

      // Delete user and all their sources (cascade delete handles sources)
      await usersApi.deleteUser(user.id);

      setSuccess('Profile deleted successfully');
      setTimeout(() => {
        logout();
        navigate('/login');
      }, 2000);
    } catch (err: any) {
      setError(err.response?.data?.message || 'Failed to delete profile');
      console.error(err);
    } finally {
      setLoading(false);
    }
  };

  return (
    <DashboardLayout>
      <div className={styles.container}>
        <div className={styles.header}>
          <h1>👤 My Profile</h1>
          <p>Manage your account settings and profile information</p>
        </div>

        {error && <div className={styles.errorBanner}>{error}</div>}
        {success && <div className={styles.successBanner}>{success}</div>}

        {/* Profile Information Card */}
        <div className={styles.card}>
          <div className={styles.cardHeader}>
            <h2>Profile Information</h2>
            {!isEditingProfile && (
              <button
                onClick={() => setIsEditingProfile(true)}
                className={styles.editButton}
              >
                ✏️ Edit
              </button>
            )}
          </div>

          {isEditingProfile ? (
            <div className={styles.formSection}>
              <div className={styles.formGroup}>
                <label htmlFor="fullName">Full Name</label>
                <input
                  id="fullName"
                  type="text"
                  value={profileForm.fullName}
                  onChange={(e) =>
                    setProfileForm({ ...profileForm, fullName: e.target.value })
                  }
                  placeholder="Enter your full name"
                />
              </div>

              <div className={styles.formGroup}>
                <label htmlFor="email">Email Address</label>
                <input
                  id="email"
                  type="email"
                  value={profileForm.email}
                  onChange={(e) =>
                    setProfileForm({ ...profileForm, email: e.target.value })
                  }
                  placeholder="Enter your email"
                />
              </div>

              <div className={styles.formActions}>
                <button
                  onClick={handleSaveProfile}
                  disabled={loading}
                  className={styles.saveButton}
                >
                  {loading ? '💾 Saving...' : '💾 Save Changes'}
                </button>
                <button
                  onClick={() => setIsEditingProfile(false)}
                  className={styles.cancelButton}
                >
                  Cancel
                </button>
              </div>
            </div>
          ) : (
            <div className={styles.infoSection}>
              <div className={styles.infoRow}>
                <span className={styles.label}>Full Name:</span>
                <span className={styles.value}>{user?.fullName || 'N/A'}</span>
              </div>
              <div className={styles.infoRow}>
                <span className={styles.label}>Email:</span>
                <span className={styles.value}>{user?.email || 'N/A'}</span>
              </div>
              <div className={styles.infoRow}>
                <span className={styles.label}>User ID:</span>
                <span className={styles.value}>{user?.id || 'N/A'}</span>
              </div>
              <div className={styles.infoRow}>
                <span className={styles.label}>Roles:</span>
                <span className={styles.value}>
                  {user?.roles?.join(', ') || 'N/A'}
                </span>
              </div>
            </div>
          )}
        </div>

        {/* Change Password Card */}
        <div className={styles.card}>
          <div className={styles.cardHeader}>
            <h2>🔐 Change Password</h2>
            {!isChangingPassword && (
              <button
                onClick={() => setIsChangingPassword(true)}
                className={styles.editButton}
              >
                ✏️ Change
              </button>
            )}
          </div>

          {isChangingPassword ? (
            <div className={styles.formSection}>
              <div className={styles.formGroup}>
                <label htmlFor="currentPassword">Current Password</label>
                <input
                  id="currentPassword"
                  type="password"
                  value={passwordForm.currentPassword}
                  onChange={(e) =>
                    setPasswordForm({
                      ...passwordForm,
                      currentPassword: e.target.value,
                    })
                  }
                  placeholder="Enter current password"
                />
              </div>

              <div className={styles.formGroup}>
                <label htmlFor="newPassword">New Password</label>
                <input
                  id="newPassword"
                  type="password"
                  value={passwordForm.newPassword}
                  onChange={(e) =>
                    setPasswordForm({
                      ...passwordForm,
                      newPassword: e.target.value,
                    })
                  }
                  placeholder="Enter new password (min. 8 characters)"
                />
              </div>

              <div className={styles.formGroup}>
                <label htmlFor="confirmPassword">Confirm New Password</label>
                <input
                  id="confirmPassword"
                  type="password"
                  value={passwordForm.confirmPassword}
                  onChange={(e) =>
                    setPasswordForm({
                      ...passwordForm,
                      confirmPassword: e.target.value,
                    })
                  }
                  placeholder="Confirm new password"
                />
              </div>

              <div className={styles.formActions}>
                <button
                  onClick={handleChangePassword}
                  disabled={loading}
                  className={styles.saveButton}
                >
                  {loading ? '🔒 Updating...' : '🔒 Update Password'}
                </button>
                <button
                  onClick={() => setIsChangingPassword(false)}
                  className={styles.cancelButton}
                >
                  Cancel
                </button>
              </div>
            </div>
          ) : (
            <div className={styles.infoSection}>
              <p>Keep your account secure with a strong password.</p>
              <small>
                We recommend changing your password regularly and using a unique,
                complex password.
              </small>
            </div>
          )}
        </div>

        {/* Delete Account Card */}
        <div className={`${styles.card} ${styles.dangerCard}`}>
          <div className={styles.cardHeader}>
            <h2>🗑️ Delete Account</h2>
          </div>

          {!showDeleteConfirm ? (
            <div className={styles.infoSection}>
              <p>
                <strong>Warning:</strong> Deleting your account is permanent and
                cannot be undone.
              </p>
              <ul className={styles.warningList}>
                <li>All your personal data will be deleted</li>
                <li>All GNSS sources you created will be deleted</li>
                <li>This action cannot be reversed</li>
              </ul>
              <button
                onClick={() => setShowDeleteConfirm(true)}
                className={styles.deleteButton}
              >
                🗑️ Delete My Account
              </button>
            </div>
          ) : (
            <div className={styles.formSection}>
              <div className={styles.warningBox}>
                <p>
                  <strong>Are you sure you want to delete your account?</strong>
                </p>
                <p>
                  This will delete your account and all associated data including:
                </p>
                <ul>
                  <li>Your profile information</li>
                  <li>All GNSS sources you created</li>
                  <li>All your data in the system</li>
                </ul>
                <p>This action cannot be undone.</p>
              </div>

              <div className={styles.formGroup}>
                <label htmlFor="deletePassword">
                  Enter your password to confirm deletion:
                </label>
                <input
                  id="deletePassword"
                  type="password"
                  value={deletePassword}
                  onChange={(e) => setDeletePassword(e.target.value)}
                  placeholder="Enter your password"
                />
              </div>

              <div className={styles.formActions}>
                <button
                  onClick={handleDeleteProfile}
                  disabled={loading || !deletePassword}
                  className={styles.deleteButton}
                >
                  {loading ? '⏳ Deleting...' : '🗑️ Permanently Delete'}
                </button>
                <button
                  onClick={() => {
                    setShowDeleteConfirm(false);
                    setDeletePassword('');
                  }}
                  className={styles.cancelButton}
                >
                  Cancel
                </button>
              </div>
            </div>
          )}
        </div>

        {/* Info Box */}
        <div className={styles.infoBox}>
          <h3>ℹ️ Profile Management</h3>
          <p>
            Use this page to manage your account settings, change your password,
            and delete your account if needed.
          </p>
          <h4>Profile Information:</h4>
          <ul>
            <li>Update your full name and email address</li>
            <li>View your user ID and assigned roles</li>
          </ul>
          <h4>Security:</h4>
          <ul>
            <li>Change your password regularly for better security</li>
            <li>Use a strong password with mixed characters</li>
          </ul>
          <h4>Account Deletion:</h4>
          <ul>
            <li>This action is permanent and irreversible</li>
            <li>All your GNSS sources will also be deleted</li>
            <li>Requires password confirmation for security</li>
          </ul>
        </div>
      </div>
    </DashboardLayout>
  );
};

export default ProfilePage;
