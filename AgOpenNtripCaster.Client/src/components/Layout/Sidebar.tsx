import React from 'react';
import { NavLink } from 'react-router-dom';
import styles from './Sidebar.module.css';

interface SidebarProps {
  isOpen: boolean;
  userRole?: string;
}

export const Sidebar: React.FC<SidebarProps> = ({ isOpen, userRole }) => {
  const isAdmin = userRole === 'Admin';

  return (
    <aside className={`${styles.sidebar} ${isOpen ? styles.open : styles.closed}`}>
      <nav className={styles.nav}>
        {/* Main Navigation */}
        <div className={styles.section}>
          <h3 className={styles.sectionTitle}>Dashboard</h3>
          <ul className={styles.menu}>
            <li>
              <NavLink
                to="/dashboard"
                className={({ isActive }) =>
                  `${styles.navLink} ${isActive ? styles.active : ''}`
                }
              >
                <span className={styles.icon}>📊</span>
                <span className={styles.label}>Overview</span>
              </NavLink>
            </li>
            <li>
              <NavLink
                to="/dashboard/my-sources"
                className={({ isActive }) =>
                  `${styles.navLink} ${isActive ? styles.active : ''}`
                }
              >
                <span className={styles.icon}>🚀</span>
                <span className={styles.label}>My Sources</span>
              </NavLink>
            </li>
            <li>
              <NavLink
                to="/dashboard/available-sources"
                className={({ isActive }) =>
                  `${styles.navLink} ${isActive ? styles.active : ''}`
                }
              >
                <span className={styles.icon}>🌍</span>
                <span className={styles.label}>Available Sources</span>
              </NavLink>
            </li>
          </ul>
        </div>

        {/* Admin Navigation */}
        {isAdmin && (
          <div className={styles.section}>
            <h3 className={styles.sectionTitle}>Administration</h3>
            <ul className={styles.menu}>
              <li>
                <NavLink
                  to="/admin/users"
                  className={({ isActive }) =>
                    `${styles.navLink} ${isActive ? styles.active : ''}`
                  }
                >
                  <span className={styles.icon}>👥</span>
                  <span className={styles.label}>Users</span>
                </NavLink>
              </li>
              <li>
                <NavLink
                  to="/admin/groups"
                  className={({ isActive }) =>
                    `${styles.navLink} ${isActive ? styles.active : ''}`
                  }
                >
                  <span className={styles.icon}>👫</span>
                  <span className={styles.label}>Groups</span>
                </NavLink>
              </li>
              <li>
                <NavLink
                  to="/admin/mountpoints"
                  className={({ isActive }) =>
                    `${styles.navLink} ${isActive ? styles.active : ''}`
                  }
                >
                  <span className={styles.icon}>🔌</span>
                  <span className={styles.label}>Mount Points</span>
                </NavLink>
              </li>
              <li>
                <NavLink
                  to="/admin/caster-config"
                  className={({ isActive }) =>
                    `${styles.navLink} ${isActive ? styles.active : ''}`
                  }
                >
                  <span className={styles.icon}>🗺️</span>
                  <span className={styles.label}>Caster Config</span>
                </NavLink>
              </li>
              <li>
                <NavLink
                  to="/admin/network-config"
                  className={({ isActive }) =>
                    `${styles.navLink} ${isActive ? styles.active : ''}`
                  }
                >
                  <span className={styles.icon}>🌐</span>
                  <span className={styles.label}>Network Config</span>
                </NavLink>
              </li>
              <li>
                <NavLink
                  to="/admin/realtime-map"
                  className={({ isActive }) =>
                    `${styles.navLink} ${isActive ? styles.active : ''}`
                  }
                >
                  <span className={styles.icon}>🛰️</span>
                  <span className={styles.label}>Network Monitoring</span>
                </NavLink>
              </li>
            </ul>
          </div>
        )}

        {/* Help Section */}
        <div className={styles.section}>
          <h3 className={styles.sectionTitle}>Help</h3>
          <ul className={styles.menu}>
            <li>
              <a
                href="https://github.com/ntripcaster/docs"
                target="_blank"
                rel="noopener noreferrer"
                className={styles.navLink}
              >
                <span className={styles.icon}>📚</span>
                <span className={styles.label}>Documentation</span>
              </a>
            </li>
          </ul>
        </div>
      </nav>
    </aside>
  );
};

export default Sidebar;
