import api from './api';

export interface DatabaseStats {
  totalSize: string;
  tableCount: number;
  recordCount: number;
  lastBackup: string;
  databaseVersion: string;
}

export interface TableInfo {
  name: string;
  records: number;
  size: string;
}

export interface MaintenanceResult {
  success: boolean;
  message: string;
  executionTimeMs: number;
}

export const databaseApi = {
  async getStatistics(): Promise<DatabaseStats> {
    const response = await api.get<DatabaseStats>('/admin/database/statistics');
    return response.data;
  },

  async getTableInformation(): Promise<TableInfo[]> {
    const response = await api.get<TableInfo[]>('/admin/database/tables');
    return response.data;
  },

  async backup(): Promise<MaintenanceResult> {
    const response = await api.post<MaintenanceResult>('/admin/database/backup');
    return response.data;
  },

  async optimize(): Promise<MaintenanceResult> {
    const response = await api.post<MaintenanceResult>('/admin/database/optimize');
    return response.data;
  },

  async repair(): Promise<MaintenanceResult> {
    const response = await api.post<MaintenanceResult>('/admin/database/repair');
    return response.data;
  },

  async vacuum(): Promise<MaintenanceResult> {
    const response = await api.post<MaintenanceResult>('/admin/database/vacuum');
    return response.data;
  },

  async reindex(): Promise<MaintenanceResult> {
    const response = await api.post<MaintenanceResult>('/admin/database/reindex');
    return response.data;
  },

  async rebuildStatistics(): Promise<MaintenanceResult> {
    const response = await api.post<MaintenanceResult>('/admin/database/rebuild-statistics');
    return response.data;
  },

  async cleanupOldLogs(olderThanDays: number = 30): Promise<MaintenanceResult> {
    const response = await api.post<MaintenanceResult>('/admin/database/cleanup-old-logs', null, {
      params: { olderThanDays },
    });
    return response.data;
  },

  async cleanupOrphanedRecords(): Promise<MaintenanceResult> {
    const response = await api.post<MaintenanceResult>('/admin/database/cleanup-orphaned');
    return response.data;
  },
};
