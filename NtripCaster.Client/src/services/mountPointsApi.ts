import api from './api';
import type {
  MountPointListResponse,
  MountPointDto,
  CreateMountPointRequest,
  CreateMountPointResponse,
  UpdateMountPointRequest,
  UpdateMountPointResponse,
  DeleteMountPointResponse,
  AllowGroupRequest,
  DenyGroupRequest,
  MountPointPermissionResponse,
} from '../types';

export const mountPointsApi = {
  // Get paginated list of mount points
  async getMountPoints(page: number = 1, pageSize: number = 10): Promise<MountPointListResponse> {
    const response = await api.get<MountPointListResponse>('/mountpoints', {
      params: { page, pageSize },
    });
    return response.data;
  },

  // Get mount point by ID
  async getMountPointById(mountPointId: number): Promise<MountPointDto> {
    const response = await api.get<MountPointDto>(`/mountpoints/${mountPointId}`);
    return response.data;
  },

  // Create new mount point
  async createMountPoint(request: CreateMountPointRequest): Promise<CreateMountPointResponse> {
    const response = await api.post<CreateMountPointResponse>('/mountpoints', request);
    return response.data;
  },

  // Update mount point
  async updateMountPoint(
    mountPointId: number,
    request: UpdateMountPointRequest
  ): Promise<UpdateMountPointResponse> {
    const response = await api.put<UpdateMountPointResponse>(
      `/mountpoints/${mountPointId}`,
      request
    );
    return response.data;
  },

  // Delete mount point
  async deleteMountPoint(mountPointId: number): Promise<DeleteMountPointResponse> {
    const response = await api.delete<DeleteMountPointResponse>(`/mountpoints/${mountPointId}`);
    return response.data;
  },

  // Allow group to access mount point
  async allowGroup(
    mountPointId: number,
    request: AllowGroupRequest
  ): Promise<MountPointPermissionResponse> {
    const response = await api.post<MountPointPermissionResponse>(
      `/mountpoints/${mountPointId}/allow-group`,
      request
    );
    return response.data;
  },

  // Deny group access to mount point
  async denyGroup(
    mountPointId: number,
    request: DenyGroupRequest
  ): Promise<MountPointPermissionResponse> {
    const response = await api.post<MountPointPermissionResponse>(
      `/mountpoints/${mountPointId}/deny-group`,
      request
    );
    return response.data;
  },
};
