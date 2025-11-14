import api from './api';

export interface TelegramSettings {
  id: number;
  enabled: boolean;
  botToken: string;
  chatId: string;
  notifySourceConnected: boolean;
  notifySourceDisconnected: boolean;
  notifySystemStarted: boolean;
  notifyErrors: boolean;
  createdAt: string;
  updatedAt: string;
}

export const telegramApi = {
  async getSettings(): Promise<TelegramSettings> {
    const response = await api.get<TelegramSettings>('/telegram-settings');
    return response.data;
  },

  async updateSettings(settings: Partial<TelegramSettings>): Promise<TelegramSettings> {
    const response = await api.put<TelegramSettings>('/telegram-settings', settings);
    return response.data;
  },

  async testConnection(): Promise<{ message: string }> {
    const response = await api.post<{ message: string }>('/telegram-settings/test');
    return response.data;
  },
};
