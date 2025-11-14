import React from 'react';
import { Link } from 'react-router-dom';
import styles from './PrivacyPage.module.css';

export const PrivacyPage: React.FC = () => {
  return (
    <div className={styles.container}>
      <div className={styles.header}>
        <Link to="/" className={styles.backLink}>← Back to Home</Link>
        <h1>Privacy Policy</h1>
        <p className={styles.lastUpdated}>Last updated: {new Date().toLocaleDateString()}</p>
      </div>

      <div className={styles.content}>
        <section className={styles.section}>
          <h2>1. Introduction</h2>
          <p>
            Welcome to AgOpenNtripCaster. We are committed to protecting your privacy and handling your personal data in an open and transparent manner. This privacy policy explains how we collect, use, and protect your information when you use our NTRIP Caster service.
          </p>
        </section>

        <section className={styles.section}>
          <h2>2. Data Controller</h2>
          <p>
            AgOpenNtripCaster is the data controller responsible for your personal data. If you have any questions about this privacy policy or our data practices, please contact us.
          </p>
        </section>

        <section className={styles.section}>
          <h2>3. Information We Collect</h2>
          <h3>3.1 Account Information</h3>
          <p>When you register for an account, we collect:</p>
          <ul>
            <li>Email address</li>
            <li>Full name</li>
            <li>Password (encrypted)</li>
            <li>Account creation date</li>
          </ul>

          <h3>3.2 Connection Data</h3>
          <p>When you use our NTRIP service, we collect:</p>
          <ul>
            <li>IP addresses of connected clients and sources</li>
            <li>Connection timestamps and duration</li>
            <li>Mount points accessed</li>
            <li>Data transfer statistics</li>
          </ul>

          <h3>3.3 Technical Data</h3>
          <p>We automatically collect certain technical information:</p>
          <ul>
            <li>Browser type and version</li>
            <li>Device information</li>
            <li>Operating system</li>
            <li>Authentication tokens and session data</li>
          </ul>
        </section>

        <section className={styles.section}>
          <h2>4. How We Use Your Information</h2>
          <p>We use your personal data for the following purposes:</p>
          <ul>
            <li><strong>Service Provision:</strong> To provide and maintain the NTRIP Caster service</li>
            <li><strong>Authentication:</strong> To verify your identity and manage your account</li>
            <li><strong>Security:</strong> To protect against unauthorized access and ensure service integrity</li>
            <li><strong>Communication:</strong> To send service-related notifications and updates</li>
            <li><strong>Analytics:</strong> To monitor usage and improve service performance</li>
            <li><strong>Legal Compliance:</strong> To comply with legal obligations and enforce our terms of service</li>
          </ul>
        </section>

        <section className={styles.section}>
          <h2>5. Cookies and Tracking</h2>
          <h3>5.1 Essential Cookies</h3>
          <p>
            We use essential cookies that are strictly necessary for the operation of our service. These cookies enable core functionality such as authentication and session management. Without these cookies, the service cannot function properly.
          </p>

          <h3>5.2 No Tracking or Advertising</h3>
          <p>
            We do <strong>not</strong> use tracking cookies, advertising cookies, or third-party analytics services. We respect your privacy and do not track your behavior across websites or use your data for advertising purposes.
          </p>

          <h3>5.3 Cookie Types We Use</h3>
          <ul>
            <li><strong>Authentication Tokens:</strong> To keep you logged in and secure your session</li>
            <li><strong>Session Cookies:</strong> To maintain your session state</li>
            <li><strong>Preference Cookies:</strong> To remember your settings (e.g., dark mode)</li>
            <li><strong>GDPR Consent:</strong> To remember your cookie consent choice</li>
          </ul>
        </section>

        <section className={styles.section}>
          <h2>6. Data Retention</h2>
          <p>We retain your personal data only for as long as necessary:</p>
          <ul>
            <li><strong>Account Data:</strong> Retained until you delete your account</li>
            <li><strong>Connection Logs:</strong> Retained for 90 days for security and troubleshooting</li>
            <li><strong>Session Data:</strong> Automatically deleted after session expiry</li>
          </ul>
        </section>

        <section className={styles.section}>
          <h2>7. Data Security</h2>
          <p>
            We implement appropriate technical and organizational measures to protect your personal data:
          </p>
          <ul>
            <li>Passwords are encrypted using industry-standard hashing algorithms</li>
            <li>Data transmission is secured using HTTPS/TLS encryption</li>
            <li>Access to personal data is restricted to authorized personnel only</li>
            <li>Regular security audits and updates</li>
          </ul>
        </section>

        <section className={styles.section}>
          <h2>8. Your Rights (GDPR)</h2>
          <p>Under the General Data Protection Regulation (GDPR), you have the following rights:</p>
          <ul>
            <li><strong>Right to Access:</strong> Request a copy of your personal data</li>
            <li><strong>Right to Rectification:</strong> Request correction of inaccurate data</li>
            <li><strong>Right to Erasure:</strong> Request deletion of your personal data</li>
            <li><strong>Right to Restriction:</strong> Request restriction of processing</li>
            <li><strong>Right to Data Portability:</strong> Receive your data in a structured format</li>
            <li><strong>Right to Object:</strong> Object to processing of your data</li>
            <li><strong>Right to Withdraw Consent:</strong> Withdraw consent at any time</li>
          </ul>
          <p>
            To exercise any of these rights, please contact us or use the account management features in your profile settings.
          </p>
        </section>

        <section className={styles.section}>
          <h2>9. Data Sharing</h2>
          <p>
            We do <strong>not</strong> sell, rent, or share your personal data with third parties for marketing purposes. We only share data in the following limited circumstances:
          </p>
          <ul>
            <li><strong>Legal Requirements:</strong> When required by law or legal process</li>
            <li><strong>Service Providers:</strong> With trusted service providers who assist in operating our service (e.g., hosting providers)</li>
            <li><strong>Security:</strong> To protect against fraud, abuse, or security threats</li>
          </ul>
        </section>

        <section className={styles.section}>
          <h2>10. International Data Transfers</h2>
          <p>
            Your data is stored and processed within the European Economic Area (EEA). If data is transferred outside the EEA, we ensure appropriate safeguards are in place to protect your data in accordance with GDPR requirements.
          </p>
        </section>

        <section className={styles.section}>
          <h2>11. Children's Privacy</h2>
          <p>
            Our service is not intended for children under the age of 16. We do not knowingly collect personal data from children. If you believe we have collected data from a child, please contact us immediately.
          </p>
        </section>

        <section className={styles.section}>
          <h2>12. Changes to This Policy</h2>
          <p>
            We may update this privacy policy from time to time. We will notify you of any significant changes by posting the new policy on this page and updating the "Last updated" date. Your continued use of the service after changes indicates acceptance of the updated policy.
          </p>
        </section>

        <section className={styles.section}>
          <h2>13. Contact Information</h2>
          <p>
            If you have any questions about this privacy policy or our data practices, please contact us:
          </p>
          <ul>
            <li>Email: privacy@ntripcaster.local</li>
            <li>Website: <Link to="/">AgOpenNtripCaster</Link></li>
          </ul>
        </section>

        <section className={styles.section}>
          <h2>14. Account Deletion</h2>
          <p>
            You can delete your account at any time through your profile settings. When you delete your account:
          </p>
          <ul>
            <li>Your personal data will be permanently deleted</li>
            <li>All GNSS sources you created will be deleted</li>
            <li>Active connections will be terminated</li>
            <li>This action cannot be undone</li>
          </ul>
          <p>
            Some data may be retained for a limited period as required by law or for legitimate business purposes (e.g., security logs).
          </p>
        </section>
      </div>

      <div className={styles.footer}>
        <Link to="/" className={styles.homeButton}>Return to Home</Link>
      </div>
    </div>
  );
};

export default PrivacyPage;
