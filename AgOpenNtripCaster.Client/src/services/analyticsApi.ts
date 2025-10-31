import api from './api';

export interface Analytics {
  totalConnections: number;
  totalDataTransferred: number;
  averageSessionDuration: string;
  peakConnectionTime: string;
}

export interface ConnectionTrend {
  timestamp: string;
  clientCount: number;
  sourceCount: number;
}

export interface DataTransferStats {
  timestamp: string;
  bytesSent: number;
  bytesReceived: number;
}

export interface UserActivityReport {
  totalUsers: number;
  activeUsers: number;
  newUsers: number;
  avgSessionsPerUser: number;
  usersByGroup: Array<{
    groupName: string;
    users: number;
    sessions: number;
  }>;
}

export interface PerformanceMetrics {
  avgResponseTime: string;
  maxResponseTime: string;
  minResponseTime: string;
  cpuUsage: string;
  memoryUsage: string;
  databaseQueries: number;
  avgQueryTime: string;
  slowQueries: number;
}

export const analyticsApi = {
  async getOverview(dateRange: string = '7d'): Promise<Analytics> {
    const response = await api.get<Analytics>('/admin/analytics/overview', {
      params: { dateRange },
    });
    return response.data;
  },

  async getConnectionTrends(dateRange: string = '7d'): Promise<ConnectionTrend[]> {
    const response = await api.get<ConnectionTrend[]>('/admin/analytics/connection-trends', {
      params: { dateRange },
    });
    return response.data;
  },

  async getDataTransferStats(dateRange: string = '7d'): Promise<DataTransferStats[]> {
    const response = await api.get<DataTransferStats[]>('/admin/analytics/data-transfer', {
      params: { dateRange },
    });
    return response.data;
  },

  async getUserActivityReport(dateRange: string = '7d'): Promise<UserActivityReport> {
    const response = await api.get<UserActivityReport>('/admin/analytics/user-activity', {
      params: { dateRange },
    });
    return response.data;
  },

  async getPerformanceMetrics(): Promise<PerformanceMetrics> {
    const response = await api.get<PerformanceMetrics>('/admin/analytics/performance');
    return response.data;
  },

  async exportCsv(dateRange: string = '7d'): Promise<Blob> {
    const response = await api.get('/admin/analytics/export-csv', {
      params: { dateRange },
      responseType: 'blob',
    });
    return response.data;
  },

  async exportPdf(dateRange: string = '7d'): Promise<Blob> {
    const response = await api.get('/admin/analytics/export-pdf', {
      params: { dateRange },
      responseType: 'blob',
    });
    return response.data;
  },
};
