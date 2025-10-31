import api from './api';

export interface ActivityDto {
  id: number;
  type: string;
  mountPointId: number;
  mountPointName: string;
  userId?: string;
  userName?: string;
  description: string;
  createdAt: string;
}

export const activityApi = {
  // Get recent activities
  async getRecentActivities(limit: number = 50): Promise<ActivityDto[]> {
    const response = await api.get<ActivityDto[]>('/activity/recent', {
      params: { limit }
    });
    return response.data;
  },

  // Get activities for a mount point
  async getMountPointActivities(mountPointId: number, limit: number = 20): Promise<ActivityDto[]> {
    const response = await api.get<ActivityDto[]>(`/activity/mount-point/${mountPointId}`, {
      params: { limit }
    });
    return response.data;
  },

  // Get user's own activities
  async getMyActivities(limit: number = 20): Promise<ActivityDto[]> {
    const response = await api.get<ActivityDto[]>('/activity/my-activities', {
      params: { limit }
    });
    return response.data;
  },
};
