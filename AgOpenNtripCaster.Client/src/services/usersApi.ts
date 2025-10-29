import api from './api';
import type {
  UserListResponse,
  UserDto,
  CreateUserRequest,
  CreateUserResponse,
  UpdateUserRequest,
  UpdateUserResponse,
  DeleteUserResponse,
} from '../types';

export const usersApi = {
  // Get paginated list of users
  async getUsers(page: number = 1, pageSize: number = 10): Promise<UserListResponse> {
    const response = await api.get<UserListResponse>('/users', {
      params: { page, pageSize },
    });
    return response.data;
  },

  // Get user by ID
  async getUserById(userId: string): Promise<UserDto> {
    const response = await api.get<UserDto>(`/users/${userId}`);
    return response.data;
  },

  // Create new user
  async createUser(request: CreateUserRequest): Promise<CreateUserResponse> {
    const response = await api.post<CreateUserResponse>('/users', request);
    return response.data;
  },

  // Update user
  async updateUser(userId: string, request: UpdateUserRequest): Promise<UpdateUserResponse> {
    const response = await api.put<UpdateUserResponse>(`/users/${userId}`, request);
    return response.data;
  },

  // Delete user
  async deleteUser(userId: string): Promise<DeleteUserResponse> {
    const response = await api.delete<DeleteUserResponse>(`/users/${userId}`);
    return response.data;
  },
};
