import api from './api';
import type {
  GroupListResponse,
  NtripGroupDto,
  CreateGroupRequest,
  CreateGroupResponse,
  UpdateGroupRequest,
  UpdateGroupResponse,
  DeleteGroupResponse,
  AddUserToGroupRequest,
  RemoveUserFromGroupRequest,
} from '../types';

export const groupsApi = {
  // Get paginated list of groups
  async getGroups(page: number = 1, pageSize: number = 10): Promise<GroupListResponse> {
    const response = await api.get<GroupListResponse>('/groups', {
      params: { page, pageSize },
    });
    return response.data;
  },

  // Get group by ID
  async getGroupById(groupId: number): Promise<NtripGroupDto> {
    const response = await api.get<NtripGroupDto>(`/groups/${groupId}`);
    return response.data;
  },

  // Create new group
  async createGroup(request: CreateGroupRequest): Promise<CreateGroupResponse> {
    const response = await api.post<CreateGroupResponse>('/groups', request);
    return response.data;
  },

  // Update group
  async updateGroup(groupId: number, request: UpdateGroupRequest): Promise<UpdateGroupResponse> {
    const response = await api.put<UpdateGroupResponse>(`/groups/${groupId}`, request);
    return response.data;
  },

  // Delete group
  async deleteGroup(groupId: number): Promise<DeleteGroupResponse> {
    const response = await api.delete<DeleteGroupResponse>(`/groups/${groupId}`);
    return response.data;
  },

  // Add user to group
  async addUserToGroup(groupId: number, request: AddUserToGroupRequest) {
    const response = await api.post(`/groups/${groupId}/add-user`, request);
    return response.data;
  },

  // Remove user from group
  async removeUserFromGroup(groupId: number, request: RemoveUserFromGroupRequest) {
    const response = await api.post(`/groups/${groupId}/remove-user`, request);
    return response.data;
  },
};
