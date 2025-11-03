import api from './api';
import type {
  MountPointListResponse,
  CreateMountPointRequest,
  CreateMountPointResponse,
  UpdateMountPointRequest,
  UpdateMountPointResponse,
  DeleteMountPointResponse,
} from '../types';

export const myMountPointsApi = {
  // Get current user's own mount points (sources they created)
  async getMyMountPoints(page: number = 1, pageSize: number = 10): Promise<MountPointListResponse> {
    const response = await api.get<MountPointListResponse>('/mountpoints/my-mountpoints', {
      params: { page, pageSize },
    });
    return response.data;
  },

  // Create new mount point (source) owned by current user
  async createMountPoint(request: CreateMountPointRequest): Promise<CreateMountPointResponse> {
    try {
      const response = await api.post<CreateMountPointResponse>('/mountpoints', request);
      return response.data;
    } catch (error: any) {
      // Extract error message from response
      if (error.response?.data?.message) {
        throw new Error(error.response.data.message);
      }
      throw error;
    }
  },

  // Update user's own mount point
  async updateMountPoint(
    mountPointId: number,
    request: UpdateMountPointRequest
  ): Promise<UpdateMountPointResponse> {
    try {
      const response = await api.put<UpdateMountPointResponse>(`/mountpoints/${mountPointId}`, request);
      return response.data;
    } catch (error: any) {
      // Extract error message from response
      if (error.response?.data?.message) {
        throw new Error(error.response.data.message);
      }
      throw error;
    }
  },

  // Delete user's own mount point
  async deleteMountPoint(mountPointId: number): Promise<DeleteMountPointResponse> {
    try {
      const response = await api.delete<DeleteMountPointResponse>(`/mountpoints/${mountPointId}`);
      return response.data;
    } catch (error: any) {
      // Extract error message from response
      if (error.response?.data?.message) {
        throw new Error(error.response.data.message);
      }
      throw error;
    }
  },
};
