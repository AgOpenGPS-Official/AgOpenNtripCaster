import React, { useState, useEffect } from 'react';
import { Link } from 'react-router-dom';
import styles from './GdprConsent.module.css';

export const GdprConsent: React.FC = () => {
  const [showBanner, setShowBanner] = useState(false);

  useEffect(() => {
    const consent = localStorage.getItem('gdpr-consent');
    if (!consent) {
      setShowBanner(true);
    }
  }, []);

  const handleAccept = () => {
    localStorage.setItem('gdpr-consent', 'true');
    localStorage.setItem('gdpr-consent-date', new Date().toISOString());
    setShowBanner(false);
  };

  const handleDecline = () => {
    localStorage.setItem('gdpr-consent', 'false');
    localStorage.setItem('gdpr-consent-date', new Date().toISOString());
    setShowBanner(false);
  };

  if (!showBanner) {
    return null;
  }

  return (
    <div className={styles.gdprBanner}>
      <div className={styles.gdprContent}>
        <div className={styles.gdprText}>
          <h3>Privacy & Cookies</h3>
          <p>
            We use essential cookies to provide secure authentication and maintain your session.
            These cookies are necessary for the proper functioning of this NTRIP Caster service.
            We do not use tracking or advertising cookies.
          </p>
          <p>
            By continuing to use this service, you agree to our use of essential cookies.
            For more information, please read our{' '}
            <Link to="/privacy">privacy policy</Link>.
          </p>
        </div>
        <div className={styles.gdprButtons}>
          <button onClick={handleAccept} className={styles.gdprAccept}>
            Accept
          </button>
          <button onClick={handleDecline} className={styles.gdprDecline}>
            Decline
          </button>
        </div>
      </div>
    </div>
  );
};

export default GdprConsent;
