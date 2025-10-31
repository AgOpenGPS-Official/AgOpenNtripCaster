import api from './api';

export interface EmailConfig {
  smtpServer: string;
  smtpPort: number;
  senderEmail: string;
  senderPassword: string;
  useTls: boolean;
}

export interface LoggingConfig {
  logLevel: string;
  maxLogSize: number;
  retentionDays: number;
  enableConsoleLogging: boolean;
  enableFileLogging: boolean;
}

export interface SystemSettings {
  emailConfig: EmailConfig;
  loggingConfig: LoggingConfig;
}

export const systemSettingsApi = {
  async getSettings(): Promise<SystemSettings> {
    const response = await api.get<SystemSettings>('/admin/settings');
    return response.data;
  },

  async updateEmailSettings(config: EmailConfig): Promise<EmailConfig> {
    const response = await api.put<EmailConfig>('/admin/settings/email', config);
    return response.data;
  },

  async updateLoggingSettings(config: LoggingConfig): Promise<LoggingConfig> {
    const response = await api.put<LoggingConfig>('/admin/settings/logging', config);
    return response.data;
  },

  async testEmailSettings(config: EmailConfig): Promise<{ message: string }> {
    const response = await api.post<{ message: string }>('/admin/settings/email/test', config);
    return response.data;
  },
};
