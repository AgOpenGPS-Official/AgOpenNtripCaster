import { useContext } from 'react';
import { AuthContext } from '../contexts/AuthContext';
import type { AuthContextType } from '../types';

/**
 * Custom hook to access authentication context
 * Must be used within AuthProvider
 */
export const useAuth = (): AuthContextType & {
  isAdmin: boolean;
  isReadOnly: boolean;
  canWrite: boolean;
} => {
  const context = useContext(AuthContext);

  if (!context) {
    throw new Error('useAuth must be used within an AuthProvider');
  }

  const isAdmin = context.user?.roles?.includes('Admin') ?? false;
  const isReadOnly = context.user?.roles?.includes('ReadOnly') ?? false;
  const canWrite = isAdmin; // Only Admin can write, ReadOnly cannot

  return {
    ...context,
    isAdmin,
    isReadOnly,
    canWrite,
  };
};

export default useAuth;
