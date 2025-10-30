import api from './api';
import type { CasterInfoDto, UpdateCasterInfoRequest, NetworkInfoDto, UpdateNetworkInfoRequest } from '../types';

export const casterNetworkApi = {
  // ============================================================================
  // CASTER INFO ENDPOINTS
  // ============================================================================

  /**
   * Get current caster configuration
   */
  async getCasterInfo(): Promise<CasterInfoDto> {
    try {
      const response = await api.get<CasterInfoDto>('admin/config/caster');
      return response.data;
    } catch (error) {
      console.error('Error fetching caster info:', error);
      throw error;
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
      throw error;
    }
  },

  // ============================================================================
  // NETWORK INFO ENDPOINTS
  // ============================================================================

  /**
   * Get current network configuration
   */
  async getNetworkInfo(): Promise<NetworkInfoDto> {
    try {
      const response = await api.get<NetworkInfoDto>('admin/config/network');
      return response.data;
    } catch (error) {
      console.error('Error fetching network info:', error);
      throw error;
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
      throw error;
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
