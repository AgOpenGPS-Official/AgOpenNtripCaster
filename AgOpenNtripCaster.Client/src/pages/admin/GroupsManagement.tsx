import React, { useState, useEffect } from 'react';
import DashboardLayout from '../../components/Layout/DashboardLayout';
import type { NtripGroupDto, CreateGroupRequest, UpdateGroupRequest } from '../../types';
import { groupsApi } from '../../services/groupsApi';
import styles from './GroupsManagement.module.css';

export const GroupsManagement: React.FC = () => {
  // State
  const [groups, setGroups] = useState<NtripGroupDto[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [page, setPage] = useState(1);
  const [totalPages, setTotalPages] = useState(1);
  const pageSize = 10;

  // Form state
  const [showForm, setShowForm] = useState(false);
  const [editingGroupId, setEditingGroupId] = useState<number | null>(null);
  const [formData, setFormData] = useState({
    name: '',
    description: '',
    isActive: true,
  });

  // Delete confirmation
  const [deleteConfirm, setDeleteConfirm] = useState<{ show: boolean; groupId: number | null }>({
    show: false,
    groupId: null,
  });

  // Load groups
  const loadGroups = async (pageNum: number) => {
    try {
      setLoading(true);
      setError(null);
      const response = await groupsApi.getGroups(pageNum, pageSize);
      setGroups(response.groups);
      setTotalPages(Math.ceil(response.total / pageSize));
      setPage(pageNum);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to load groups');
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadGroups(1);
  }, []);

  // Handle form input
  const handleInputChange = (e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
    const { name, value, type } = e.target;
    setFormData((prev) => ({
      ...prev,
      [name]: type === 'checkbox' ? (e.target as HTMLInputElement).checked : value,
    }));
  };

  // Handle create group
  const handleCreateGroup = async () => {
    if (!formData.name) {
      setError('Group name is required');
      return;
    }

    try {
      setError(null);
      const request: CreateGroupRequest = {
        name: formData.name,
        description: formData.description,
        isActive: formData.isActive,
      };

      await groupsApi.createGroup(request);
      resetForm();
      setShowForm(false);
      loadGroups(1);
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
        setError(err instanceof Error ? err.message : 'Failed to create group');
      }
    }
  };

  // Handle update group
  const handleUpdateGroup = async () => {
    if (!editingGroupId || !formData.name) {
      setError('Group name is required');
      return;
    }

    try {
      setError(null);
      const request: UpdateGroupRequest = {
        name: formData.name,
        description: formData.description,
        isActive: formData.isActive,
      };

      await groupsApi.updateGroup(editingGroupId, request);
      resetForm();
      setShowForm(false);
      loadGroups(page);
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
        setError(err instanceof Error ? err.message : 'Failed to update group');
      }
    }
  };

  // Handle delete group
  const handleDeleteGroup = async (groupId: number) => {
    try {
      setError(null);
      await groupsApi.deleteGroup(groupId);
      setDeleteConfirm({ show: false, groupId: null });
      loadGroups(page);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to delete group');
    }
  };

  // Open edit form
  const handleEditGroup = (group: NtripGroupDto) => {
    setEditingGroupId(group.id);
    setFormData({
      name: group.name,
      description: group.description,
      isActive: group.isActive,
    });
    setShowForm(true);
  };

  // Reset form
  const resetForm = () => {
    setEditingGroupId(null);
    setFormData({
      name: '',
      description: '',
      isActive: true,
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
          <h1>Groups Management</h1>
          <button className={styles.createBtn} onClick={() => setShowForm(true)}>
            + Add New Group
          </button>
        </div>

        {error && !showForm && <div className={styles.error}>{error}</div>}

        {/* Create/Edit Form */}
        {showForm && (
          <div className={styles.formContainer}>
            <div className={styles.formContent}>
              <h2>{editingGroupId ? 'Edit Group' : 'Create New Group'}</h2>

              {error && <div className={styles.error} style={{ whiteSpace: 'pre-line' }}>{error}</div>}

              <div className={styles.formGroup}>
                <label>Group Name</label>
                <input
                  type="text"
                  name="name"
                  value={formData.name}
                  onChange={handleInputChange}
                  placeholder="e.g., Premium Users"
                />
              </div>

              <div className={styles.formGroup}>
                <label>Description</label>
                <textarea
                  name="description"
                  value={formData.description}
                  onChange={handleInputChange}
                  placeholder="Describe this group's purpose"
                  rows={3}
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
                  onClick={editingGroupId ? handleUpdateGroup : handleCreateGroup}
                >
                  {editingGroupId ? 'Update Group' : 'Create Group'}
                </button>
                <button className={styles.cancelBtn} onClick={handleCloseForm}>
                  Cancel
                </button>
              </div>
            </div>
          </div>
        )}

        {/* Groups Table */}
        <div className={styles.tableContainer}>
          {loading ? (
            <div className={styles.loading}>Loading groups...</div>
          ) : groups.length === 0 ? (
            <div className={styles.empty}>No groups found</div>
          ) : (
            <>
              <table className={styles.table}>
                <thead>
                  <tr>
                    <th>Name</th>
                    <th>Description</th>
                    <th>Members</th>
                    <th>Status</th>
                    <th>Created</th>
                    <th>Actions</th>
                  </tr>
                </thead>
                <tbody>
                  {groups.map((group) => (
                    <tr key={group.id}>
                      <td className={styles.name}>{group.name}</td>
                      <td className={styles.description}>{group.description || '-'}</td>
                      <td>{group.memberCount}</td>
                      <td>
                        <span className={group.isActive ? styles.active : styles.inactive}>
                          {group.isActive ? 'Active' : 'Inactive'}
                        </span>
                      </td>
                      <td>{new Date(group.createdAt).toLocaleDateString()}</td>
                      <td className={styles.actions}>
                        <button
                          className={styles.editBtn}
                          onClick={() => handleEditGroup(group)}
                          title="Edit group"
                        >
                          Edit
                        </button>
                        <button
                          className={styles.deleteBtn}
                          onClick={() => setDeleteConfirm({ show: true, groupId: group.id })}
                          title="Delete group"
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
                  onClick={() => loadGroups(page - 1)}
                  disabled={page === 1}
                  className={styles.paginationBtn}
                >
                  Previous
                </button>
                <span className={styles.pageInfo}>
                  Page {page} of {totalPages}
                </span>
                <button
                  onClick={() => loadGroups(page + 1)}
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
              <p>Are you sure you want to delete this group? This action cannot be undone.</p>
              <div className={styles.modalActions}>
                <button
                  className={styles.deleteConfirmBtn}
                  onClick={() => deleteConfirm.groupId && handleDeleteGroup(deleteConfirm.groupId)}
                >
                  Delete
                </button>
                <button
                  className={styles.cancelBtn}
                  onClick={() => setDeleteConfirm({ show: false, groupId: null })}
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

export default GroupsManagement;
