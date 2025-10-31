import api from './api';

export interface SystemLog {
  id: number;
  timestamp: string;
  level: string;
  message: string;
  exception?: string;
}

export interface LogsResponse {
  total: number;
  count: number;
  logs: SystemLog[];
}

export interface LogStatistics {
  totalLogs: number;
  errors: number;
  warnings: number;
  infos: number;
  debugs: number;
  averagePerHour: number;
  lastLogTime: string;
  logFileSize: string;
}

export const logsApi = {
  async getLogs(
    level?: string,
    limit: number = 100,
    offset: number = 0
  ): Promise<LogsResponse> {
    const response = await api.get<LogsResponse>('/admin/logs', {
      params: { level, limit, offset },
    });
    return response.data;
  },

  async getStatistics(): Promise<LogStatistics> {
    const response = await api.get<LogStatistics>('/admin/logs/statistics');
    return response.data;
  },

  async clearOldLogs(olderThanDays: number = 30): Promise<{ message: string; deletedCount: number }> {
    const response = await api.post<{ message: string; deletedCount: number }>(
      '/admin/logs/clear-old',
      null,
      { params: { olderThanDays } }
    );
    return response.data;
  },

  async downloadLogs(level?: string): Promise<Blob> {
    const response = await api.get('/admin/logs/download', {
      params: { level },
      responseType: 'blob',
    });
    return response.data;
  },

  async exportLogsCsv(days: number = 7): Promise<Blob> {
    const response = await api.get('/admin/logs/export-csv', {
      params: { days },
      responseType: 'blob',
    });
    return response.data;
  },
};
