import React, { useEffect, useState } from 'react';
import { useNavigate, useSearchParams } from 'react-router-dom';
import { authApi } from '../../services/auth';
import styles from './AuthPage.module.css';

export const VerifyEmailPage: React.FC = () => {
  const [searchParams] = useSearchParams();
  const navigate = useNavigate();
  const [status, setStatus] = useState<'verifying' | 'success' | 'error' | 'expired'>('verifying');
  const [message, setMessage] = useState('Verifying your email...');
  const [email, setEmail] = useState<string>('');
  const [resending, setResending] = useState(false);

  useEffect(() => {
    const verifyEmail = async () => {
      const emailParam = searchParams.get('email');
      const token = searchParams.get('token');

      if (!emailParam || !token) {
        setStatus('error');
        setMessage('Invalid verification link. Please check your email and try again.');
        return;
      }

      setEmail(emailParam);

      try {
        const response = await authApi.verifyEmail({ email: emailParam, token });

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
          // Check if token expired
          if (response.message?.toLowerCase().includes('expired') ||
              response.message?.toLowerCase().includes('invalid')) {
            setStatus('expired');
            setMessage('This verification link has expired.');
          } else {
            setStatus('error');
            setMessage(response.message || 'Email verification failed.');
          }
        }
      } catch (error: any) {
        const errorMsg = error.response?.data?.message || '';

        // Check if token expired
        if (errorMsg.toLowerCase().includes('expired') ||
            errorMsg.toLowerCase().includes('invalid')) {
          setStatus('expired');
          setMessage('This verification link has expired.');
        } else {
          setStatus('error');
          setMessage(errorMsg || 'Email verification failed. Please try again or contact support.');
        }
      }
    };

    verifyEmail();
  }, [searchParams, navigate]);

  const handleResendEmail = async () => {
    if (!email) return;

    try {
      setResending(true);
      await authApi.resendVerificationEmail(email);
      setMessage('A new verification email has been sent! Please check your inbox.');
      setStatus('success');
    } catch (error: any) {
      setMessage(error.response?.data?.message || 'Failed to resend verification email. Please try again.');
    } finally {
      setResending(false);
    }
  };

  return (
    <div className={styles.authPageContainer}>
      <div className={styles.authCard}>
        <div className={styles.header}>
          <h1>NtripCaster</h1>
          <p>Email Verification</p>
        </div>

        <div style={{ textAlign: 'center', padding: '2rem' }}>
          {/* Icon based on status */}
          {status === 'verifying' && (
            <div style={{ fontSize: '4rem', marginBottom: '1rem', color: '#3498db' }}>⏳</div>
          )}
          {status === 'success' && (
            <div style={{ fontSize: '4rem', marginBottom: '1rem', color: '#27ae60' }}>✓</div>
          )}
          {(status === 'error' || status === 'expired') && (
            <div style={{ fontSize: '4rem', marginBottom: '1rem', color: '#e74c3c' }}>✗</div>
          )}

          {/* Title */}
          <h2 style={{ marginBottom: '1rem', color: '#2c3e50' }}>
            {status === 'verifying' && 'Verifying Your Email...'}
            {status === 'success' && 'Verification Successful!'}
            {status === 'error' && 'Verification Failed'}
            {status === 'expired' && 'Link Expired'}
          </h2>

          {/* Message */}
          <p style={{
            color: (status === 'error' || status === 'expired') ? '#e74c3c' : '#666',
            marginBottom: '2rem',
            fontSize: '1rem',
            lineHeight: '1.5'
          }}>
            {message}
          </p>

          {/* Action buttons */}
          {status === 'success' && !resending && (
            <p style={{ color: '#999', fontSize: '0.9rem' }}>
              Redirecting to login page in 3 seconds...
            </p>
          )}

          {status === 'expired' && (
            <div style={{ display: 'flex', flexDirection: 'column', gap: '1rem', alignItems: 'center' }}>
              <button
                onClick={handleResendEmail}
                disabled={resending}
                className={styles.submitButton}
                style={{
                  backgroundColor: resending ? '#95a5a6' : '#3498db',
                  cursor: resending ? 'not-allowed' : 'pointer',
                  minWidth: '200px'
                }}
              >
                {resending ? 'Sending...' : '📧 Resend Verification Email'}
              </button>
              <button
                onClick={() => navigate('/login')}
                className={styles.submitButton}
                style={{ backgroundColor: '#95a5a6', minWidth: '200px' }}
              >
                Back to Login
              </button>
            </div>
          )}

          {status === 'error' && (
            <div style={{ display: 'flex', flexDirection: 'column', gap: '1rem', alignItems: 'center' }}>
              <button
                onClick={() => navigate('/login')}
                className={styles.submitButton}
                style={{ backgroundColor: '#3498db', minWidth: '200px' }}
              >
                Go to Login
              </button>
              <button
                onClick={() => navigate('/register')}
                className={styles.submitButton}
                style={{ backgroundColor: '#95a5a6', minWidth: '200px' }}
              >
                Register New Account
              </button>
            </div>
          )}
        </div>
      </div>
    </div>
  );
};

export default VerifyEmailPage;
