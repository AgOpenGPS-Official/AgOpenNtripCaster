import api from './api';

export interface DashboardStatsDto {
  activeClients: number;
  activeSources: number;
  totalBytesReceived: number;
  totalBytesSent: number;
  totalBytesTransferred: number;
  serverStartTime: string;
  currentTime: string;
  uptimeFormatted: string;
}

export const dashboardStatsApi = {
  // Get real-time dashboard statistics
  async getDashboardStats(): Promise<DashboardStatsDto> {
    const response = await api.get<DashboardStatsDto>('/admin/config/stats');
    return response.data;
  },
};
