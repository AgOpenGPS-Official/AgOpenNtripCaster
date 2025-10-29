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
    const response = await api.post<CreateMountPointResponse>('/mountpoints', request);
    return response.data;
  },

  // Update user's own mount point
  async updateMountPoint(
    mountPointId: number,
    request: UpdateMountPointRequest
  ): Promise<UpdateMountPointResponse> {
    const response = await api.put<UpdateMountPointResponse>(`/mountpoints/${mountPointId}`, request);
    return response.data;
  },

  // Delete user's own mount point
  async deleteMountPoint(mountPointId: number): Promise<DeleteMountPointResponse> {
    const response = await api.delete<DeleteMountPointResponse>(`/mountpoints/${mountPointId}`);
    return response.data;
  },
};
