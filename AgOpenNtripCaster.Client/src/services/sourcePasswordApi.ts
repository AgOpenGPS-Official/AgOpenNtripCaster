import api from './api';

export interface SourcePasswordResponse {
  isSet: boolean;
  masked: string | null;
  plainPassword?: string | null;
}

export interface GeneratedSourcePasswordResponse {
  sourcePassword: string;
  message: string;
}

export const sourcePasswordApi = {
  /**
   * Get current user's source password status (masked)
   */
  async getSourcePassword(): Promise<SourcePasswordResponse> {
    try {
      const response = await api.get<SourcePasswordResponse>('/users/me/source-password');
      return response.data;
    } catch (error) {
      console.error('Error fetching source password status:', error);
      throw error;
    }
  },

  /**
   * Generate a new source password for current user
   * Returns the plain password - must be saved by user!
   */
  async resetSourcePassword(): Promise<GeneratedSourcePasswordResponse> {
    try {
      const response = await api.post<GeneratedSourcePasswordResponse>('/users/me/source-password/reset', {});
      return response.data;
    } catch (error) {
      console.error('Error generating source password:', error);
      throw error;
    }
  },
};

export default sourcePasswordApi;
