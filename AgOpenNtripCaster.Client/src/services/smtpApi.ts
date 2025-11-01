import api from './api';

export interface EmailSmtpSettings {
  id: number;
  host: string;
  port: number;
  username: string;
  password: string;
  fromEmail: string;
  fromName: string;
  enableSsl: boolean;
  enableTls: boolean;
  isConfigured: boolean;
  updatedAt: string;
}

export interface SmtpTestResult {
  message: string;
  success: boolean;
}

export const smtpApi = {
  async getSettings(): Promise<EmailSmtpSettings> {
    const response = await api.get<EmailSmtpSettings>('/admin/smtp/settings');
    return response.data;
  },

  async updateSettings(settings: EmailSmtpSettings): Promise<EmailSmtpSettings> {
    const response = await api.put<EmailSmtpSettings>('/admin/smtp/settings', {
      host: settings.host,
      port: settings.port,
      username: settings.username,
      password: settings.password,
      fromEmail: settings.fromEmail,
      fromName: settings.fromName,
      enableSsl: settings.enableSsl,
      enableTls: settings.enableTls,
      isConfigured: settings.isConfigured,
    });
    return response.data;
  },

  async testConnection(email: string): Promise<SmtpTestResult> {
    const response = await api.post<SmtpTestResult>('/admin/smtp/test', {
      host: 'test', // Will use current settings
      fromEmail: email,
    });
    return response.data;
  },
};
