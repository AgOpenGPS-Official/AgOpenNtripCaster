import React from 'react';
import LoginForm from '../../components/Auth/LoginForm';
import styles from './AuthPage.module.css';

export const LoginPage: React.FC = () => {
  return (
    <div className={styles.authPageContainer}>
      <div className={styles.authCard}>
        <div className={styles.header}>
          <h1>NtripCaster</h1>
          <p>Real-time GNSS Corrections</p>
        </div>
        <LoginForm />
      </div>
    </div>
  );
};

export default LoginPage;
