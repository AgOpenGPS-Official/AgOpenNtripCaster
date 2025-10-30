import { useState, useEffect } from 'react';
import { groupsApi } from '../services/groupsApi';
import { mountPointsApi } from '../services/mountPointsApi';
import type { NtripGroupDto } from '../types';
import styles from './MountPointGroupAccessDialog.module.css';

interface MountPointGroupAccessDialogProps {
  mountPointId: number;
  mountPointName: string;
  allowedGroupNames: string[];
  onClose: () => void;
  onSuccess: () => void;
}

export default function MountPointGroupAccessDialog({
  mountPointId,
  mountPointName,
  allowedGroupNames,
  onClose,
  onSuccess,
}: MountPointGroupAccessDialogProps) {
  const [groups, setGroups] = useState<NtripGroupDto[]>([]);
  const [selectedGroupIds, setSelectedGroupIds] = useState<Set<number>>(new Set());
  const [loading, setLoading] = useState(true);
  const [saving, setSaving] = useState(false);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    loadGroups();
  }, []);

  const loadGroups = async () => {
    try {
      setLoading(true);
      setError(null);
      // Load all groups (we'll fetch them all, not paginated for simplicity)
      const response = await groupsApi.getGroups(1, 1000);
      setGroups(response.groups);

      // Mark groups that are currently allowed
      const allowedGroupNameSet = new Set(allowedGroupNames);
      const selectedIds = new Set<number>();
      response.groups.forEach((group) => {
        if (allowedGroupNameSet.has(group.name)) {
          selectedIds.add(group.id);
        }
      });
      setSelectedGroupIds(selectedIds);
    } catch (err) {
      setError(`Failed to load groups: ${err instanceof Error ? err.message : 'Unknown error'}`);
    } finally {
      setLoading(false);
    }
  };

  const handleGroupToggle = (groupId: number) => {
    const newSelected = new Set(selectedGroupIds);
    if (newSelected.has(groupId)) {
      newSelected.delete(groupId);
    } else {
      newSelected.add(groupId);
    }
    setSelectedGroupIds(newSelected);
  };

  const handleSave = async () => {
    try {
      setSaving(true);
      setError(null);

      // Find which groups to add and remove
      const currentGroupIds = new Set(
        groups
          .filter((g) => allowedGroupNames.includes(g.name))
          .map((g) => g.id)
      );

      // Groups to allow (new selections)
      const groupsToAllow = Array.from(selectedGroupIds).filter((id) => !currentGroupIds.has(id));

      // Groups to deny (removed selections)
      const groupsToDeny = Array.from(currentGroupIds).filter((id) => !selectedGroupIds.has(id));

      // Execute all allow operations
      for (const groupId of groupsToAllow) {
        await mountPointsApi.allowGroup(mountPointId, { groupId });
      }

      // Execute all deny operations
      for (const groupId of groupsToDeny) {
        await mountPointsApi.denyGroup(mountPointId, { groupId });
      }

      onSuccess();
      onClose();
    } catch (err) {
      setError(`Failed to update group access: ${err instanceof Error ? err.message : 'Unknown error'}`);
    } finally {
      setSaving(false);
    }
  };

  return (
    <div className={styles.modal}>
      <div className={styles.modalContent}>
        <h2>Manage Group Access</h2>
        <p className={styles.subtitle}>Mount Point: <strong>{mountPointName}</strong></p>

        {error && <div className={styles.error}>{error}</div>}

        <div className={styles.groupsContainer}>
          {loading ? (
            <div className={styles.loading}>Loading groups...</div>
          ) : groups.length === 0 ? (
            <div className={styles.empty}>No groups available. Create groups first.</div>
          ) : (
            <div className={styles.groupsList}>
              {groups.map((group) => (
                <label key={group.id} className={styles.groupItem}>
                  <input
                    type="checkbox"
                    checked={selectedGroupIds.has(group.id)}
                    onChange={() => handleGroupToggle(group.id)}
                    disabled={saving}
                  />
                  <div className={styles.groupInfo}>
                    <div className={styles.groupName}>{group.name}</div>
                    {group.description && (
                      <div className={styles.groupDescription}>{group.description}</div>
                    )}
                    <div className={styles.groupMeta}>
                      {group.memberCount} member{group.memberCount !== 1 ? 's' : ''}
                    </div>
                  </div>
                </label>
              ))}
            </div>
          )}
        </div>

        <div className={styles.actions}>
          <button
            className={styles.saveBtn}
            onClick={handleSave}
            disabled={loading || saving}
          >
            {saving ? 'Saving...' : 'Save Changes'}
          </button>
          <button
            className={styles.cancelBtn}
            onClick={onClose}
            disabled={saving}
          >
            Cancel
          </button>
        </div>
      </div>
    </div>
  );
}
