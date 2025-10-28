import React from 'react';
import RegisterForm from '../../components/Auth/RegisterForm';
import styles from './AuthPage.module.css';

export const RegisterPage: React.FC = () => {
  return (
    <div className={styles.authPageContainer}>
      <div className={styles.authCard}>
        <div className={styles.header}>
          <h1>NtripCaster</h1>
          <p>Real-time GNSS Corrections</p>
        </div>
        <RegisterForm />
      </div>
    </div>
  );
};

export default RegisterPage;
