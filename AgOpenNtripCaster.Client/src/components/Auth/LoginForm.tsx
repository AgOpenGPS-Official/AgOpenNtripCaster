import React, { useState } from 'react';
import { useForm } from 'react-hook-form';
import { useNavigate } from 'react-router-dom';
import { useAuth } from '../../hooks/useAuth';
import { authApi } from '../../services/auth';
import type { LoginRequest } from '../../types';
import styles from './AuthForm.module.css';

export const LoginForm: React.FC = () => {
  const { register, handleSubmit, formState: { errors, isSubmitting } } = useForm<LoginRequest>({
    defaultValues: {
      email: '',
      password: '',
    },
  });

  const { login } = useAuth();
  const navigate = useNavigate();
  const [apiError, setApiError] = useState<string | null>(null);
  const [isUnverified, setIsUnverified] = useState(false);
  const [unverifiedEmail, setUnverifiedEmail] = useState<string>('');
  const [resending, setResending] = useState(false);
  const [resendSuccess, setResendSuccess] = useState(false);

  const onSubmit = async (data: LoginRequest) => {
    setApiError(null);
    setIsUnverified(false);
    setResendSuccess(false);

    try {
      await login(data.email, data.password);
      navigate('/dashboard');
    } catch (error: unknown) {
      // Extract error message from various possible error formats
      let errorMessage = 'Login failed. Please try again.';

      if (error && typeof error === 'object' && 'response' in error) {
        const axiosError = error as { response?: { data?: { message?: string } } };
        if (axiosError.response?.data?.message) {
          errorMessage = axiosError.response.data.message;
        }
      } else if (error && typeof error === 'object' && 'message' in error) {
        errorMessage = (error as { message: string }).message;
      }

      // Check if it's an unverified email error
      const isEmailVerificationError = errorMessage.includes('verify your email');

      // Map backend messages to user-friendly messages
      if (errorMessage.includes('Invalid email or password')) {
        errorMessage = 'Invalid email or password. Please check your credentials and try again.';
      } else if (errorMessage.includes('Account is disabled')) {
        errorMessage = 'Your account has been disabled. Please contact the administrator.';
      } else if (isEmailVerificationError) {
        errorMessage = 'Please verify your email address before logging in. Check your inbox for the verification link.';
        setIsUnverified(true);
        setUnverifiedEmail(data.email);
      } else if (errorMessage.includes('Email and password are required')) {
        errorMessage = 'Email and password are required fields.';
      }

      setApiError(errorMessage);
    }
  };

  const handleResendVerification = async () => {
    if (!unverifiedEmail) return;

    try {
      setResending(true);
      setResendSuccess(false);
      const response = await authApi.resendVerificationEmail(unverifiedEmail);

      if (response.success) {
        setResendSuccess(true);
        setApiError(response.message || 'Verification email sent! Please check your inbox.');
      } else {
        setApiError(response.message || 'Failed to resend verification email.');
      }
    } catch (error: any) {
      setApiError(error.response?.data?.message || 'Failed to resend verification email. Please try again.');
    } finally {
      setResending(false);
    }
  };

  return (
    <form onSubmit={handleSubmit(onSubmit)} className={styles.form}>
      <h2>Login</h2>

      {apiError && (
        <div className={resendSuccess ? styles.successAlert : styles.errorAlert}>
          {apiError}
          {isUnverified && !resendSuccess && (
            <button
              type="button"
              onClick={handleResendVerification}
              disabled={resending}
              style={{
                marginTop: '1rem',
                padding: '0.5rem 1rem',
                backgroundColor: '#3498db',
                color: 'white',
                border: 'none',
                borderRadius: '4px',
                cursor: resending ? 'not-allowed' : 'pointer',
                opacity: resending ? 0.6 : 1,
                fontSize: '0.875rem',
                fontWeight: '500',
              }}
            >
              {resending ? '📧 Sending...' : '📧 Resend Verification Email'}
            </button>
          )}
        </div>
      )}

      <div className={styles.formGroup}>
        <label htmlFor="email">Email</label>
        <input
          id="email"
          type="email"
          placeholder="Enter your email"
          disabled={isSubmitting}
          {...register('email', {
            required: 'Email is required',
            pattern: {
              value: /^[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,}$/i,
              message: 'Invalid email address',
            },
          })}
        />
        {errors.email && (
          <span className={styles.error}>{errors.email.message}</span>
        )}
      </div>

      <div className={styles.formGroup}>
        <label htmlFor="password">Password</label>
        <input
          id="password"
          type="password"
          placeholder="Enter your password"
          disabled={isSubmitting}
          {...register('password', {
            required: 'Password is required',
            minLength: {
              value: 6,
              message: 'Password must be at least 6 characters',
            },
          })}
        />
        {errors.password && (
          <span className={styles.error}>{errors.password.message}</span>
        )}
      </div>

      <button
        type="submit"
        disabled={isSubmitting}
        className={styles.submitButton}
      >
        {isSubmitting ? 'Logging in...' : 'Login'}
      </button>

      <p className={styles.link}>
        Don't have an account?{' '}
        <a href="/register">Register here</a>
      </p>

      <p className={styles.link} style={{ marginTop: '0.5rem', fontSize: '0.875rem' }}>
        <a href="/privacy">Privacy Policy</a>
      </p>
    </form>
  );
};

export default LoginForm;
