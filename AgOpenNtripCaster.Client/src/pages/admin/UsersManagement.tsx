import React, { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import type { UserDto, CreateUserRequest, UpdateUserRequest, NtripGroupDto } from '../../types';
import { usersApi } from '../../services/usersApi';
import { groupsApi } from '../../services/groupsApi';
import { useAuth } from '../../hooks/useAuth';
import styles from './UsersManagement.module.css';

export const UsersManagement: React.FC = () => {
  const { canWrite } = useAuth();
  // State
  const [users, setUsers] = useState<UserDto[]>([]);
  const [availableGroups, setAvailableGroups] = useState<NtripGroupDto[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [page, setPage] = useState(1);
  const [totalPages, setTotalPages] = useState(1);
  const pageSize = 10;

  // Form state
  const [showForm, setShowForm] = useState(false);
  const [editingUserId, setEditingUserId] = useState<string | null>(null);
  const [formData, setFormData] = useState({
    email: '',
    fullName: '',
    password: '',
    maxConnections: 5,
    isActive: true,
    isAdmin: false,
    groupIds: [] as number[],
  });

  // Delete confirmation
  const [deleteConfirm, setDeleteConfirm] = useState<{ show: boolean; userId: string | null }>({
    show: false,
    userId: null,
  });

  // Group management modal
  const [manageGroupsModal, setManageGroupsModal] = useState<{
    show: boolean;
    user: UserDto | null;
  }>({
    show: false,
    user: null,
  });

  // Load users
  const loadUsers = async (pageNum: number) => {
    try {
      setLoading(true);
      setError(null);
      const response = await usersApi.getUsers(pageNum, pageSize);
      setUsers(response.users);
      setTotalPages(Math.ceil(response.total / pageSize));
      setPage(pageNum);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to load users');
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadUsers(1);
    loadGroups();
  }, []);

  // Load available groups for the selector
  const loadGroups = async () => {
    try {
      const response = await groupsApi.getGroups(1, 100); // Get first 100 groups
      setAvailableGroups(response.groups);
    } catch (err) {
      console.error('Failed to load groups:', err);
    }
  };

  // Check if user is the System Administrator
  const isSystemAdmin = (user: UserDto): boolean => {
    return user.email === 'admin@ntripcaster.local' || user.fullName === 'System Administrator';
  };

  // Handle form input
  const handleInputChange = (e: React.ChangeEvent<HTMLInputElement | HTMLSelectElement>) => {
    const { name, value, type } = e.target;
    setFormData((prev) => ({
      ...prev,
      [name]: type === 'checkbox' ? (e.target as HTMLInputElement).checked : value,
    }));
  };

  // Handle create user
  const handleCreateUser = async () => {
    if (!formData.email || !formData.fullName || !formData.password) {
      setError('Email, Full Name, and Password are required');
      return;
    }

    try {
      setError(null);
      const request: CreateUserRequest = {
        email: formData.email,
        fullName: formData.fullName,
        password: formData.password,
        maxConnections: formData.maxConnections,
        isActive: formData.isActive,
        isAdmin: formData.isAdmin,
      };

      await usersApi.createUser(request);
      resetForm();
      setShowForm(false);
      loadUsers(1);
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
        setError(err instanceof Error ? err.message : 'Failed to create user');
      }
    }
  };

  // Handle update user
  const handleUpdateUser = async () => {
    if (!editingUserId || !formData.fullName) {
      setError('Full Name is required');
      return;
    }

    try {
      setError(null);
      const request: UpdateUserRequest = {
        fullName: formData.fullName,
        maxConnections: formData.maxConnections,
        isActive: formData.isActive,
        isAdmin: formData.isAdmin,
      };

      await usersApi.updateUser(editingUserId, request);
      resetForm();
      setShowForm(false);
      loadUsers(page);
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
        setError(err instanceof Error ? err.message : 'Failed to update user');
      }
    }
  };

  // Handle delete user
  const handleDeleteUser = async (userId: string) => {
    try {
      setError(null);
      await usersApi.deleteUser(userId);
      setDeleteConfirm({ show: false, userId: null });
      loadUsers(page);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to delete user');
    }
  };

  // Handle add user to group
  const handleAddToGroup = async (groupId: number) => {
    if (!manageGroupsModal.user) return;

    try {
      await groupsApi.addUserToGroup(groupId, { userId: manageGroupsModal.user.id });
      // Reload users to update the groups display
      await loadUsers(page);
      // Update modal user data
      const updatedUser = users.find(u => u.id === manageGroupsModal.user?.id);
      if (updatedUser) {
        setManageGroupsModal({ show: true, user: updatedUser });
      }
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to add user to group');
    }
  };

  // Handle remove user from group
  const handleRemoveFromGroup = async (groupId: number) => {
    if (!manageGroupsModal.user) return;

    try {
      await groupsApi.removeUserFromGroup(groupId, { userId: manageGroupsModal.user.id });
      // Reload users to update the groups display
      await loadUsers(page);
      // Update modal user data
      const updatedUser = users.find(u => u.id === manageGroupsModal.user?.id);
      if (updatedUser) {
        setManageGroupsModal({ show: true, user: updatedUser });
      }
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to remove user from group');
    }
  };

  // Open edit form
  const handleEditUser = (user: UserDto) => {
    setEditingUserId(user.id);
    setFormData({
      email: user.email,
      fullName: user.fullName,
      password: '',
      maxConnections: user.maxConnections,
      isActive: user.isActive,
      isAdmin: user.roles?.includes('Admin') ?? false,
      groupIds: [],
    });
    setShowForm(true);
  };

  // Reset form
  const resetForm = () => {
    setEditingUserId(null);
    setFormData({
      email: '',
      fullName: '',
      password: '',
      maxConnections: 5,
      isActive: true,
      isAdmin: false,
      groupIds: [],
    });
    setError(null);
  };

  const handleCloseForm = () => {
    resetForm();
    setShowForm(false);
  };

  return (
    <DashboardLayout>
      <div className={styles.container}>
      <div className={styles.header}>
        <h1>Users Management</h1>
        <button
          className={styles.createBtn}
          onClick={() => setShowForm(true)}
          disabled={!canWrite}
          title={!canWrite ? 'Read-only access - cannot create users' : ''}
        >
          + Add New User
        </button>
      </div>

      {error && !showForm && <div className={styles.error}>{error}</div>}

      {/* Create/Edit Form */}
      {showForm && (
        <div className={styles.formContainer}>
          <div className={styles.formContent}>
            <h2>{editingUserId ? 'Edit User' : 'Create New User'}</h2>

            {error && <div className={styles.error} style={{ whiteSpace: 'pre-line' }}>{error}</div>}

            <div className={styles.formGroup}>
              <label>Email Address</label>
              <input
                type="email"
                name="email"
                value={formData.email}
                onChange={handleInputChange}
                placeholder="user@example.com"
                disabled={!!editingUserId || !canWrite}
              />
            </div>

            <div className={styles.formGroup}>
              <label>Full Name</label>
              <input
                type="text"
                name="fullName"
                value={formData.fullName}
                onChange={handleInputChange}
                placeholder="John Doe"
                disabled={!canWrite}
              />
            </div>

            {!editingUserId && (
              <div className={styles.formGroup}>
                <label>Password</label>
                <input
                  type="password"
                  name="password"
                  value={formData.password}
                  onChange={handleInputChange}
                  placeholder="Enter password"
                  disabled={!canWrite}
                />
                <small style={{ color: '#666', fontSize: '0.875rem', marginTop: '0.25rem', display: 'block' }}>
                  Password must contain at least 8 characters, including uppercase, lowercase, digit, and special character
                </small>
              </div>
            )}

            <div className={styles.formGroup}>
              <label>Max Connections</label>
              <input
                type="number"
                name="maxConnections"
                value={formData.maxConnections}
                onChange={handleInputChange}
                min="1"
                max="100"
                disabled={!canWrite}
              />
            </div>

            <div className={styles.formGroup}>
              <label>
                <input
                  type="checkbox"
                  name="isActive"
                  checked={formData.isActive}
                  onChange={handleInputChange}
                  disabled={!canWrite}
                />
                <span>Active</span>
              </label>
            </div>

            <div className={styles.formGroup}>
              <label>
                <input
                  type="checkbox"
                  name="isAdmin"
                  checked={formData.isAdmin}
                  onChange={handleInputChange}
                  disabled={!canWrite}
                />
                <span>Administrator</span>
              </label>
            </div>

            <div className={styles.formActions}>
              <button
                className={styles.submitBtn}
                onClick={editingUserId ? handleUpdateUser : handleCreateUser}
                disabled={!canWrite}
                title={!canWrite ? 'Read-only access - cannot save changes' : ''}
              >
                {editingUserId ? 'Update User' : 'Create User'}
              </button>
              <button className={styles.cancelBtn} onClick={handleCloseForm}>
                Cancel
              </button>
            </div>
          </div>
        </div>
      )}

      {/* Users Table */}
      <div className={styles.tableContainer}>
        {loading ? (
          <div className={styles.loading}>Loading users...</div>
        ) : users.length === 0 ? (
          <div className={styles.empty}>No users found</div>
        ) : (
          <>
            <table className={styles.table}>
              <thead>
                <tr>
                  <th>Email</th>
                  <th>Full Name</th>
                  <th>Role</th>
                  <th>Groups</th>
                  <th>Connections</th>
                  <th>Status</th>
                  <th>Created</th>
                  <th>Actions</th>
                </tr>
              </thead>
              <tbody>
                {users.map((user) => (
                  <tr key={user.id}>
                    <td>{user.email}</td>
                    <td>{user.fullName}</td>
                    <td>
                      {user.roles?.includes('Admin') ? (
                        <span style={{ color: '#e74c3c', fontWeight: 'bold' }}>Admin</span>
                      ) : (
                        <span style={{ color: '#95a5a6' }}>User</span>
                      )}
                    </td>
                    <td>
                      {user.groups && user.groups.length > 0 ? (
                        <span style={{ fontSize: '0.85rem', color: '#7f8c8d' }}>
                          {user.groups.join(', ')}
                        </span>
                      ) : (
                        <span style={{ fontSize: '0.85rem', color: '#bdc3c7' }}>None</span>
                      )}
                    </td>
                    <td>{user.maxConnections}</td>
                    <td>
                      <span className={user.isActive ? styles.active : styles.inactive}>
                        {user.isActive ? 'Active' : 'Inactive'}
                      </span>
                    </td>
                    <td>{new Date(user.createdAt).toLocaleDateString()}</td>
                    <td className={styles.actions}>
                      <button
                        className={styles.editBtn}
                        onClick={() => handleEditUser(user)}
                        title={!canWrite ? 'Read-only access - cannot edit users' : 'Edit user'}
                        disabled={!canWrite}
                      >
                        Edit
                      </button>
                      <button
                        className={styles.editBtn}
                        onClick={() => setManageGroupsModal({ show: true, user })}
                        title={!canWrite ? 'Read-only access - cannot manage groups' : 'Manage groups'}
                        disabled={!canWrite}
                      >
                        Groups
                      </button>
                      {!isSystemAdmin(user) && (
                        <button
                          className={styles.deleteBtn}
                          onClick={() => setDeleteConfirm({ show: true, userId: user.id })}
                          title={!canWrite ? 'Read-only access - cannot delete users' : 'Delete user'}
                          disabled={!canWrite}
                        >
                          Delete
                        </button>
                      )}
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>

            {/* Pagination */}
            <div className={styles.pagination}>
              <button
                onClick={() => loadUsers(page - 1)}
                disabled={page === 1}
                className={styles.paginationBtn}
              >
                Previous
              </button>
              <span className={styles.pageInfo}>
                Page {page} of {totalPages}
              </span>
              <button
                onClick={() => loadUsers(page + 1)}
                disabled={page >= totalPages}
                className={styles.paginationBtn}
              >
                Next
              </button>
            </div>
          </>
        )}
      </div>

      {/* Delete Confirmation Dialog */}
      {deleteConfirm.show && (
        <div className={styles.modal}>
          <div className={styles.modalContent}>
            <h3>Confirm Delete</h3>
            <p>Are you sure you want to delete this user? This action cannot be undone.</p>
            <div className={styles.modalActions}>
              <button
                className={styles.deleteConfirmBtn}
                onClick={() => deleteConfirm.userId && handleDeleteUser(deleteConfirm.userId)}
                disabled={!canWrite}
                title={!canWrite ? 'Read-only access - cannot delete users' : ''}
              >
                Delete
              </button>
              <button
                className={styles.cancelBtn}
                onClick={() => setDeleteConfirm({ show: false, userId: null })}
              >
                Cancel
              </button>
            </div>
          </div>
        </div>
      )}

      {/* Manage Groups Modal */}
      {manageGroupsModal.show && manageGroupsModal.user && (
        <div className={styles.modal}>
          <div className={styles.modalContentWide}>
            <h3>Manage Groups for {manageGroupsModal.user.fullName}</h3>

            {/* Current Groups */}
            <div className={styles.groupSection}>
              <h4>Current Groups:</h4>
              {manageGroupsModal.user.groups && manageGroupsModal.user.groups.length > 0 ? (
                <div className={styles.groupList}>
                  {manageGroupsModal.user.groups.map((groupName) => {
                    const group = availableGroups.find(g => g.name === groupName);
                    if (!group) return null;
                    return (
                      <div key={group.id} className={styles.groupItem}>
                        <span className={styles.groupName}>{groupName}</span>
                        <button
                          className={`${styles.groupItemBtn} ${styles.deleteBtn}`}
                          onClick={() => handleRemoveFromGroup(group.id)}
                          disabled={!canWrite}
                          title={!canWrite ? 'Read-only access - cannot remove from group' : ''}
                        >
                          Remove
                        </button>
                      </div>
                    );
                  })}
                </div>
              ) : (
                <p className={styles.emptyMessage}>Not in any groups</p>
              )}
            </div>

            {/* Available Groups to Add */}
            <div className={styles.groupSection}>
              <h4>Add to Group:</h4>
              {availableGroups.filter(g => !manageGroupsModal.user?.groups?.includes(g.name)).length > 0 ? (
                <div className={styles.groupList}>
                  {availableGroups
                    .filter(g => !manageGroupsModal.user?.groups?.includes(g.name))
                    .map((group) => (
                      <div key={group.id} className={styles.groupItem}>
                        <span className={styles.groupName}>{group.name}</span>
                        <button
                          className={`${styles.groupItemBtn} ${styles.editBtn}`}
                          onClick={() => handleAddToGroup(group.id)}
                          disabled={!canWrite}
                          title={!canWrite ? 'Read-only access - cannot add to group' : ''}
                        >
                          Add
                        </button>
                      </div>
                    ))}
                </div>
              ) : (
                <p className={styles.emptyMessage}>Already in all groups</p>
              )}
            </div>

            <div className={styles.modalActions}>
              <button
                className={styles.cancelBtn}
                onClick={() => setManageGroupsModal({ show: false, user: null })}
              >
                Close
              </button>
            </div>
          </div>
        </div>
      )}
      </div>
    </DashboardLayout>
  );
};

export default UsersManagement;
