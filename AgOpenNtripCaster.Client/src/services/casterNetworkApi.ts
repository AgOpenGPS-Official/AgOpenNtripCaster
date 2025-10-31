import api from './api';
import type { CasterInfoDto, UpdateCasterInfoRequest, NetworkInfoDto, UpdateNetworkInfoRequest } from '../types';
import axios from 'axios';

// Custom error type to include status code
export class ConfigError extends Error {
  statusCode?: number;
  isNotFound?: boolean;

  constructor(message: string, statusCode?: number, isNotFound?: boolean) {
    super(message);
    this.name = 'ConfigError';
    this.statusCode = statusCode;
    this.isNotFound = isNotFound;
  }
}

export const casterNetworkApi = {
  // ============================================================================
  // CASTER INFO ENDPOINTS
  // ============================================================================

  /**
   * Get current caster configuration
   * @returns CasterInfoDto or null if not created
   * @throws ConfigError with statusCode if it's a server error
   */
  async getCasterInfo(): Promise<CasterInfoDto | null> {
    try {
      const response = await api.get<CasterInfoDto>('admin/config/caster');
      return response.data;
    } catch (error) {
      if (axios.isAxiosError(error) && error.response?.status === 404) {
        // Config not created yet - return null instead of throwing
        return null;
      }
      console.error('Error fetching caster info:', error);
      const statusCode = axios.isAxiosError(error) ? error.response?.status : undefined;
      throw new ConfigError('Failed to load caster configuration', statusCode);
    }
  },

  /**
   * Update caster configuration
   */
  async updateCasterInfo(request: UpdateCasterInfoRequest): Promise<CasterInfoDto> {
    try {
      const response = await api.put<CasterInfoDto>('admin/config/caster', request);
      return response.data;
    } catch (error) {
      console.error('Error updating caster info:', error);
      const statusCode = axios.isAxiosError(error) ? error.response?.status : undefined;
      throw new ConfigError('Failed to update caster configuration', statusCode);
    }
  },

  // ============================================================================
  // NETWORK INFO ENDPOINTS
  // ============================================================================

  /**
   * Get current network configuration
   * @returns NetworkInfoDto or null if not created
   * @throws ConfigError with statusCode if it's a server error
   */
  async getNetworkInfo(): Promise<NetworkInfoDto | null> {
    try {
      const response = await api.get<NetworkInfoDto>('admin/config/network');
      return response.data;
    } catch (error) {
      if (axios.isAxiosError(error) && error.response?.status === 404) {
        // Config not created yet - return null instead of throwing
        return null;
      }
      console.error('Error fetching network info:', error);
      const statusCode = axios.isAxiosError(error) ? error.response?.status : undefined;
      throw new ConfigError('Failed to load network configuration', statusCode);
    }
  },

  /**
   * Update network configuration
   */
  async updateNetworkInfo(request: UpdateNetworkInfoRequest): Promise<NetworkInfoDto> {
    try {
      const response = await api.put<NetworkInfoDto>('admin/config/network', request);
      return response.data;
    } catch (error) {
      console.error('Error updating network info:', error);
      const statusCode = axios.isAxiosError(error) ? error.response?.status : undefined;
      throw new ConfigError('Failed to update network configuration', statusCode);
    }
  },

  // ============================================================================
  // COMBINED ENDPOINTS
  // ============================================================================

  /**
   * Get both caster and network configuration in one call
   */
  async getAllConfig(): Promise<{ caster: CasterInfoDto | null; network: NetworkInfoDto | null }> {
    try {
      const response = await api.get<{ caster: CasterInfoDto | null; network: NetworkInfoDto | null }>(
        'admin/config/all'
      );
      return response.data;
    } catch (error) {
      console.error('Error fetching all config:', error);
      throw error;
    }
  },
};
