import api from './api';

export interface SecurityPolicy {
  requireMfa: boolean;
  passwordMinLength: number;
  passwordExpireDays: number;
  maxLoginAttempts: number;
  lockoutDurationMinutes: number;
  sessionTimeoutMinutes: number;
  ipWhitelistEnabled: boolean;
  ipWhitelist: string;
  tlsEnabled: boolean;
}

export interface AuditLogEntry {
  timestamp: string;
  action: string;
  admin: string;
  details: string;
}

export const securityApi = {
  async getPolicies(): Promise<SecurityPolicy> {
    const response = await api.get<SecurityPolicy>('/admin/security/policies');
    return response.data;
  },

  async updatePolicies(policies: SecurityPolicy): Promise<SecurityPolicy> {
    const response = await api.put<SecurityPolicy>('/admin/security/policies', policies);
    return response.data;
  },

  async validateIp(ipAddress: string): Promise<{ allowed: boolean; reason: string }> {
    const response = await api.post<{ allowed: boolean; reason: string }>(
      '/admin/security/validate-ip',
      ipAddress
    );
    return response.data;
  },

  async getAuditLog(): Promise<AuditLogEntry[]> {
    const response = await api.get<AuditLogEntry[]>('/admin/security/audit-log');
    return response.data;
  },
};
