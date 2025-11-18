import React, { useEffect, useState } from 'react';
import { useNavigate, useSearchParams } from 'react-router-dom';
import { authApi } from '../../services/auth';
import styles from './AuthPage.module.css';

export const VerifyEmailPage: React.FC = () => {
  const [searchParams] = useSearchParams();
  const navigate = useNavigate();
  const [status, setStatus] = useState<'verifying' | 'success' | 'error'>('verifying');
  const [message, setMessage] = useState('Verifying your email...');

  useEffect(() => {
    const verifyEmail = async () => {
      const email = searchParams.get('email');
      const token = searchParams.get('token');

      if (!email || !token) {
        setStatus('error');
        setMessage('Invalid verification link. Please check your email and try again.');
        return;
      }

      try {
        const response = await authApi.verifyEmail({ email, token });

        if (response.success) {
          setStatus('success');
          setMessage('Email verified successfully! You can now log in.');

          // Redirect to login after 3 seconds
          setTimeout(() => {
            navigate('/login', {
              state: {
                message: 'Email verified! Please log in with your credentials.',
                verified: true
              }
            });
          }, 3000);
        } else {
          setStatus('error');
          setMessage(response.message || 'Email verification failed. The link may have expired.');
        }
      } catch (error: any) {
        setStatus('error');
        setMessage(
          error.response?.data?.message ||
          'Email verification failed. Please try again or contact support.'
        );
      }
    };

    verifyEmail();
  }, [searchParams, navigate]);

  return (
    <div className={styles.authContainer}>
      <div className={styles.authCard}>
        <div style={{ textAlign: 'center' }}>
          {/* Icon based on status */}
          {status === 'verifying' && (
            <div style={{ fontSize: '4rem', marginBottom: '1rem' }}>⏳</div>
          )}
          {status === 'success' && (
            <div style={{ fontSize: '4rem', marginBottom: '1rem', color: '#27ae60' }}>✓</div>
          )}
          {status === 'error' && (
            <div style={{ fontSize: '4rem', marginBottom: '1rem', color: '#e74c3c' }}>✗</div>
          )}

          {/* Title */}
          <h1 style={{ marginBottom: '1rem' }}>
            {status === 'verifying' && 'Email Verification'}
            {status === 'success' && 'Verification Successful'}
            {status === 'error' && 'Verification Failed'}
          </h1>

          {/* Message */}
          <p style={{
            color: status === 'error' ? '#e74c3c' : '#666',
            marginBottom: '2rem',
            fontSize: '1.1rem'
          }}>
            {message}
          </p>

          {/* Action buttons */}
          {status === 'success' && (
            <p style={{ color: '#999', fontSize: '0.9rem' }}>
              Redirecting to login page...
            </p>
          )}

          {status === 'error' && (
            <div style={{ display: 'flex', gap: '1rem', justifyContent: 'center' }}>
              <button
                onClick={() => navigate('/login')}
                className={styles.submitButton}
                style={{ backgroundColor: '#3498db' }}
              >
                Go to Login
              </button>
              <button
                onClick={() => navigate('/register')}
                className={styles.submitButton}
                style={{ backgroundColor: '#95a5a6' }}
              >
                Register Again
              </button>
            </div>
          )}
        </div>
      </div>
    </div>
  );
};

export default VerifyEmailPage;
