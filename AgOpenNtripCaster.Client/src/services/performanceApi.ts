import api from './api';

export interface PerformanceMetricsDto {
  timestamp: string;

  // Zero-copy metrics
  totalBytesSent: number;
  memorySavedBytes: number;
  memorySavedMB: number;
  activeZeroCopyBuffers: number;
  peakZeroCopyBuffers: number;

  // Broadcast performance
  averageBroadcastTimeMs: number;
  peakBroadcastTimeMs: number;
  totalBroadcasts: number;

  // Client/Source statistics
  activeClients: number;
  activeSources: number;
  totalMountPoints: number;

  // Memory statistics
  totalMemoryUsageBytes: number;
  totalMemoryUsageMB: number;
  gcCollectionCount: number;
}

export const performanceApi = {
  // Get current real-time performance metrics
  async getCurrentMetrics(): Promise<PerformanceMetricsDto> {
    const response = await api.get<PerformanceMetricsDto>('/performance/current');
    return response.data;
  },
};
