import api from './api';

export interface ContainerInfo {
  id: string;
  name: string;
  displayName: string;
}

export interface LogLine {
  timestamp: string;
  level: string;
  message: string;
  rawLine: string;
}

export interface ContainerLogsResponse {
  containerId: string;
  containerName: string;
  lines: LogLine[];
  totalLines: number;
}

/**
 * Get list of available Docker containers
 */
export const getContainers = async (): Promise<ContainerInfo[]> => {
  const response = await api.get<ContainerInfo[]>('/admin/docker-logs/containers');
  return response.data;
};

/**
 * Get tail of container logs
 */
export const getContainerLogs = async (
  containerId: string,
  lines: number = 100
): Promise<ContainerLogsResponse> => {
  const response = await api.get<ContainerLogsResponse>(
    `/admin/docker-logs/${containerId}/tail`,
    {
      params: { lines }
    }
  );
  return response.data;
};

/**
 * Download container logs as text file
 */
export const downloadContainerLogs = async (
  containerId: string,
  lines: number = 1000
): Promise<void> => {
  const response = await api.get(`/admin/docker-logs/${containerId}/download`, {
    params: { lines },
    responseType: 'blob',
  });

  // Create download link
  const blob = new Blob([response.data], { type: 'text/plain' });
  const url = window.URL.createObjectURL(blob);
  const link = document.createElement('a');
  link.href = url;

  // Extract filename from Content-Disposition header or generate one
  const contentDisposition = response.headers['content-disposition'];
  let filename = `container-logs-${containerId}-${new Date().toISOString().split('T')[0]}.txt`;

  if (contentDisposition) {
    const filenameMatch = contentDisposition.match(/filename="?(.+)"?/i);
    if (filenameMatch?.[1]) {
      filename = filenameMatch[1];
    }
  }

  link.download = filename;
  document.body.appendChild(link);
  link.click();
  document.body.removeChild(link);
  window.URL.revokeObjectURL(url);
};
