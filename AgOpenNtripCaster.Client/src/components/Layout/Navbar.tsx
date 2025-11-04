import React, { useState } from 'react';
import { useAuth } from '../../hooks/useAuth';
import { useNavigate } from 'react-router-dom';
import { useTheme } from '../../context/ThemeContext';
import { SystemStatusIndicator } from './SystemStatusIndicator';
import { NtripStatusIndicator } from './NtripStatusIndicator';
import styles from './Navbar.module.css';

interface NavbarProps {
  onToggleSidebar: () => void;
  sidebarOpen: boolean;
}

export const Navbar: React.FC<NavbarProps> = ({ onToggleSidebar, sidebarOpen }) => {
  const { user, logout } = useAuth();
  const { theme, toggleTheme } = useTheme();
  const navigate = useNavigate();
  const [showUserMenu, setShowUserMenu] = useState(false);

  const handleLogout = () => {
    logout();
    navigate('/login');
  };

  return (
    <nav className={styles.navbar}>
      <div className={styles.left}>
        <button
          className={styles.hamburger}
          onClick={onToggleSidebar}
          title={sidebarOpen ? 'Close sidebar' : 'Open sidebar'}
        >
          <span className={styles.line}></span>
          <span className={styles.line}></span>
          <span className={styles.line}></span>
        </button>
        <div className={styles.logo}>
          <img src="/logo.png" alt="AgOpenNtripCaster Logo" className={styles.logoImg} />
          <h1>AgOpen Ntripcaster</h1>
        </div>
      </div>

      {/* System Status Indicator in center */}
      <div className={styles.centerIndicators}>
        <SystemStatusIndicator />
      </div>

      <div className={styles.right}>
        <NtripStatusIndicator />
        <div className={styles.userMenu}>
          <button
            className={styles.userButton}
            onClick={() => setShowUserMenu(!showUserMenu)}
          >
            <span className={styles.avatar}>
              {user?.fullName?.charAt(0).toUpperCase() || 'U'}
            </span>
            <span className={styles.userName}>{user?.fullName}</span>
          </button>

          {showUserMenu && (
            <div className={styles.menuDropdown}>
              <div className={styles.menuItem}>
                <strong>{user?.email}</strong>
                <small>{user?.roles?.join(', ')}</small>
              </div>
              <hr className={styles.divider} />
              <button
                className={styles.menuButton}
                onClick={() => {
                  setShowUserMenu(false);
                  navigate('/profile');
                }}
              >
                Profile Settings
              </button>
              <hr className={styles.divider} />
              <button
                className={styles.menuButton}
                onClick={toggleTheme}
                title={`Switch to ${theme === 'light' ? 'dark' : 'light'} mode`}
              >
                {theme === 'light' ? '🌙' : '☀️'} {theme === 'light' ? 'Dark Mode' : 'Light Mode'}
              </button>
              <hr className={styles.divider} />
              <button
                className={`${styles.menuButton} ${styles.logout}`}
                onClick={handleLogout}
              >
                Logout
              </button>
            </div>
          )}
        </div>
      </div>
    </nav>
  );
};

export default Navbar;
