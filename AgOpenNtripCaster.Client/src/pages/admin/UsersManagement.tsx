import React, { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import type { UserDto, CreateUserRequest, UpdateUserRequest } from '../../types';
import { usersApi } from '../../services/usersApi';
import styles from './UsersManagement.module.css';

export const UsersManagement: React.FC = () => {
  // State
  const [users, setUsers] = useState<UserDto[]>([]);
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
  });

  // Delete confirmation
  const [deleteConfirm, setDeleteConfirm] = useState<{ show: boolean; userId: string | null }>({
    show: false,
    userId: null,
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
  }, []);

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
      };

      await usersApi.createUser(request);
      resetForm();
      setShowForm(false);
      loadUsers(1);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to create user');
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
      };

      await usersApi.updateUser(editingUserId, request);
      resetForm();
      setShowForm(false);
      loadUsers(page);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to update user');
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

  // Open edit form
  const handleEditUser = (user: UserDto) => {
    setEditingUserId(user.id);
    setFormData({
      email: user.email,
      fullName: user.fullName,
      password: '',
      maxConnections: user.maxConnections,
      isActive: user.isActive,
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
    });
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
        <button className={styles.createBtn} onClick={() => setShowForm(true)}>
          + Add New User
        </button>
      </div>

      {error && <div className={styles.error}>{error}</div>}

      {/* Create/Edit Form */}
      {showForm && (
        <div className={styles.formContainer}>
          <div className={styles.formContent}>
            <h2>{editingUserId ? 'Edit User' : 'Create New User'}</h2>

            <div className={styles.formGroup}>
              <label>Email Address</label>
              <input
                type="email"
                name="email"
                value={formData.email}
                onChange={handleInputChange}
                placeholder="user@example.com"
                disabled={!!editingUserId}
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
                />
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
              />
            </div>

            <div className={styles.formGroup}>
              <label>
                <input
                  type="checkbox"
                  name="isActive"
                  checked={formData.isActive}
                  onChange={handleInputChange}
                />
                <span>Active</span>
              </label>
            </div>

            <div className={styles.formActions}>
              <button
                className={styles.submitBtn}
                onClick={editingUserId ? handleUpdateUser : handleCreateUser}
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
                        title="Edit user"
                      >
                        Edit
                      </button>
                      <button
                        className={styles.deleteBtn}
                        onClick={() => setDeleteConfirm({ show: true, userId: user.id })}
                        title="Delete user"
                      >
                        Delete
                      </button>
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
      </div>
    </DashboardLayout>
  );
};

export default UsersManagement;
