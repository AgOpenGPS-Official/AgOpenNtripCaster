import axios from 'axios';
import type { CasterInfoDto, UpdateCasterInfoRequest, NetworkInfoDto, UpdateNetworkInfoRequest } from '../types';

const API_BASE_URL = import.meta.env.VITE_API_URL || '/api';

export const casterNetworkApi = {
  // ============================================================================
  // CASTER INFO ENDPOINTS
  // ============================================================================

  /**
   * Get current caster configuration
   */
  async getCasterInfo(): Promise<CasterInfoDto> {
    try {
      const response = await axios.get<CasterInfoDto>(`${API_BASE_URL}/admin/config/caster`);
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
      const response = await axios.put<CasterInfoDto>(`${API_BASE_URL}/admin/config/caster`, request);
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
      const response = await axios.get<NetworkInfoDto>(`${API_BASE_URL}/admin/config/network`);
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
      const response = await axios.put<NetworkInfoDto>(`${API_BASE_URL}/admin/config/network`, request);
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
      const response = await axios.get<{ caster: CasterInfoDto | null; network: NetworkInfoDto | null }>(
        `${API_BASE_URL}/admin/config/all`
      );
      return response.data;
    } catch (error) {
      console.error('Error fetching all config:', error);
      throw error;
    }
  },
};
