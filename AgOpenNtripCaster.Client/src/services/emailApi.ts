import api from './api';

export interface EmailTriggerSettings {
  id: number;
  sendVerificationEmail: boolean;
  sendWelcomeEmail: boolean;
  sendSourceOfflineEmail: boolean;
  sendSourceOnlineEmail: boolean;
  adminEmailForSourceNotifications: string;
  updatedAt: string;
}

export const emailApi = {
  /**
   * Send a test email to verify SMTP configuration
   */
  async sendTestEmail(email: string): Promise<{ message: string; email: string }> {
    try {
      const response = await api.post<{ message: string; email: string }>(`admin/email/test?email=${encodeURIComponent(email)}`);
      return response.data;
    } catch (error) {
      console.error('Error sending test email:', error);
      throw error;
    }
  },

  /**
   * Get current email trigger settings
   */
  async getEmailSettings(): Promise<EmailTriggerSettings> {
    try {
      const response = await api.get<EmailTriggerSettings>('admin/email/settings');
      return response.data;
    } catch (error) {
      console.error('Error fetching email settings:', error);
      throw error;
    }
  },

  /**
   * Update email trigger settings
   */
  async updateEmailSettings(settings: EmailTriggerSettings): Promise<EmailTriggerSettings> {
    try {
      const response = await api.put<EmailTriggerSettings>('admin/email/settings', settings);
      return response.data;
    } catch (error) {
      console.error('Error updating email settings:', error);
      throw error;
    }
  },
};
