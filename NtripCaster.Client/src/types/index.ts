// ============================================================================
// AUTH TYPES
// ============================================================================

export interface RegisterRequest {
  email: string;
  fullName: string;
  password: string;
  passwordConfirm: string;
}

export interface RegisterResponse {
  success: boolean;
  message: string;
  userId?: string;
}

export interface LoginRequest {
  email: string;
  password: string;
}

export interface LoginResponse {
  success: boolean;
  message: string;
  accessToken?: string;
  refreshToken?: string;
  user?: UserDto;
}

export interface VerifyEmailRequest {
  email: string;
  token: string;
}

export interface VerifyEmailResponse {
  success: boolean;
  message: string;
}

export interface RefreshTokenRequest {
  accessToken: string;
  refreshToken: string;
}

export interface RefreshTokenResponse {
  success: boolean;
  message: string;
  accessToken?: string;
  refreshToken?: string;
}

// ============================================================================
// USER TYPES
// ============================================================================

export interface UserDto {
  id: string;
  email: string;
  userName: string;
  fullName: string;
  isActive: boolean;
  emailConfirmed: boolean;
  createdAt: string;
  maxConnections: number;
  roles?: string[];
}

export interface CreateUserRequest {
  email: string;
  fullName: string;
  password: string;
  groupIds?: number[];
  maxConnections?: number;
  isActive?: boolean;
}

export interface CreateUserResponse {
  success: boolean;
  message: string;
  user?: UserDto;
}

export interface UpdateUserRequest {
  fullName?: string;
  email?: string;
  groupIds?: number[];
  maxConnections?: number;
  isActive?: boolean;
}

export interface UpdateUserResponse {
  success: boolean;
  message: string;
  user?: UserDto;
}

export interface ChangePasswordRequest {
  currentPassword: string;
  newPassword: string;
}

export interface ChangePasswordResponse {
  success: boolean;
  message: string;
}

export interface UserListResponse {
  total: number;
  page: number;
  pageSize: number;
  users: UserDto[];
}

export interface DeleteUserResponse {
  success: boolean;
  message: string;
}

// ============================================================================
// GROUP TYPES
// ============================================================================

export interface NtripGroupDto {
  id: number;
  name: string;
  description: string;
  isActive: boolean;
  createdAt: string;
  memberCount: number;
  allowedMountPoints?: string[];
}

export interface CreateGroupRequest {
  name: string;
  description: string;
  isActive?: boolean;
}

export interface CreateGroupResponse {
  success: boolean;
  message: string;
  group?: NtripGroupDto;
}

export interface UpdateGroupRequest {
  name?: string;
  description?: string;
  isActive?: boolean;
}

export interface UpdateGroupResponse {
  success: boolean;
  message: string;
  group?: NtripGroupDto;
}

export interface DeleteGroupResponse {
  success: boolean;
  message: string;
}

export interface GroupListResponse {
  total: number;
  page: number;
  pageSize: number;
  groups: NtripGroupDto[];
}

export interface AddUserToGroupRequest {
  userId: string;
}

export interface RemoveUserFromGroupRequest {
  userId: string;
}

export interface AddUserToGroupResponse {
  success: boolean;
  message: string;
  group?: NtripGroupDto;
}

// ============================================================================
// MOUNT POINT TYPES
// ============================================================================

export interface MountPointDto {
  id: number;
  name: string;
  description: string;
  requireClientAuthentication: boolean;
  isActive: boolean;
  createdAt: string;
  activeSourceCount: number;
  activeClientCount: number;
  allowedGroupNames: string[];
}

export interface CreateMountPointRequest {
  name: string;
  description: string;
  sourcePassword: string;
  requireClientAuthentication?: boolean;
  isActive?: boolean;
  allowedGroupIds?: number[];
}

export interface CreateMountPointResponse {
  success: boolean;
  message: string;
  mountPoint?: MountPointDto;
}

export interface UpdateMountPointRequest {
  description?: string;
  sourcePassword?: string;
  requireClientAuthentication?: boolean;
  isActive?: boolean;
  allowedGroupIds?: number[];
}

export interface UpdateMountPointResponse {
  success: boolean;
  message: string;
  mountPoint?: MountPointDto;
}

export interface DeleteMountPointResponse {
  success: boolean;
  message: string;
}

export interface MountPointListResponse {
  total: number;
  page: number;
  pageSize: number;
  mountPoints: MountPointDto[];
}

export interface AllowGroupRequest {
  groupId: number;
}

export interface DenyGroupRequest {
  groupId: number;
}

export interface MountPointPermissionResponse {
  success: boolean;
  message: string;
  mountPoint?: MountPointDto;
}

// ============================================================================
// COMMON RESPONSE TYPES
// ============================================================================

export interface ApiError {
  message: string;
  errors?: Record<string, string[]>;
  statusCode?: number;
}

// ============================================================================
// AUTH CONTEXT TYPES
// ============================================================================

export interface AuthContextType {
  user: UserDto | null;
  isAuthenticated: boolean;
  isLoading: boolean;
  error: string | null;
  login: (email: string, password: string) => Promise<void>;
  register: (email: string, fullName: string, password: string, passwordConfirm?: string) => Promise<void>;
  logout: () => void;
  verifyEmail: (email: string, token: string) => Promise<void>;
  clearError: () => void;
  getAccessToken: () => string | null;
  getRefreshToken: () => string | null;
}
