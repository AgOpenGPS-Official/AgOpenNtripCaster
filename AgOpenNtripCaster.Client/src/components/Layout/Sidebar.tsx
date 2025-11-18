import React, { useState } from 'react';
import { NavLink } from 'react-router-dom';
import styles from './Sidebar.module.css';

interface SidebarProps {
  isOpen: boolean;
  userRole?: string;
}

export const Sidebar: React.FC<SidebarProps> = ({ isOpen, userRole }) => {
  const isAdmin = userRole === 'Admin';
  const isReadOnly = userRole === 'ReadOnly';
  const hasAdminAccess = isAdmin || isReadOnly; // Both Admin and ReadOnly can see admin pages

  // Get initial state from localStorage, default to all collapsed
  const [expandedSections, setExpandedSections] = useState<Record<string, boolean>>(() => {
    try {
      const saved = localStorage.getItem('sidebarExpandedSections');
      return saved ? JSON.parse(saved) : {
        management: false,
        configuration: false,
        monitoring: false,
        system: false,
      };
    } catch {
      return {
        management: false,
        configuration: false,
        monitoring: false,
        system: false,
      };
    }
  });

  const toggleSection = (section: string) => {
    setExpandedSections((prev) => {
      const updated = {
        ...prev,
        [section]: !prev[section],
      };
      // Save to localStorage so state persists across navigation
      localStorage.setItem('sidebarExpandedSections', JSON.stringify(updated));
      return updated;
    });
  };

  return (
    <aside className={`${styles.sidebar} ${isOpen ? styles.open : styles.closed}`}>
      <nav className={styles.nav}>
        {/* Main Navigation */}
        <div className={styles.section}>
          <h3 className={styles.sectionTitle}>User Dashboard</h3>
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
        {hasAdminAccess && (
          <>
            {/* Main Admin Dashboard */}
            <div className={styles.section}>
              <ul className={styles.menu}>
                <li>
                  <NavLink
                    to="/admin"
                    className={({ isActive }) =>
                      `${styles.navLink} ${isActive ? styles.active : ''}`
                    }
                  >
                    <span className={styles.icon}>⚙️</span>
                    <span className={styles.label}>Admin Dashboard</span>
                  </NavLink>
                </li>
              </ul>
            </div>

            {/* User & Network Management */}
            <div className={styles.section}>
              <button
                className={styles.sectionToggle}
                onClick={() => toggleSection('management')}
                title="Toggle Management section"
              >
                <span className={styles.toggleChevron}>
                  {expandedSections.management ? '▼' : '▶'}
                </span>
                <span className={styles.sectionTitleText}>Management</span>
              </button>
              {expandedSections.management && (
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
              </ul>
              )}
            </div>

            {/* Configuration */}
            <div className={styles.section}>
              <button
                className={styles.sectionToggle}
                onClick={() => toggleSection('configuration')}
                title="Toggle Configuration section"
              >
                <span className={styles.toggleChevron}>
                  {expandedSections.configuration ? '▼' : '▶'}
                </span>
                <span className={styles.sectionTitleText}>Configuration</span>
              </button>
              {expandedSections.configuration && (
              <ul className={styles.menu}>
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
                    to="/admin/system-settings"
                    className={({ isActive }) =>
                      `${styles.navLink} ${isActive ? styles.active : ''}`
                    }
                  >
                    <span className={styles.icon}>⚙️</span>
                    <span className={styles.label}>System Settings</span>
                  </NavLink>
                </li>
                <li>
                  <NavLink
                    to="/admin/security"
                    className={({ isActive }) =>
                      `${styles.navLink} ${isActive ? styles.active : ''}`
                    }
                  >
                    <span className={styles.icon}>🔐</span>
                    <span className={styles.label}>Security Settings</span>
                  </NavLink>
                </li>
                <li>
                  <NavLink
                    to="/admin/notification-settings"
                    className={({ isActive }) =>
                      `${styles.navLink} ${isActive ? styles.active : ''}`
                    }
                  >
                    <span className={styles.icon}>🔔</span>
                    <span className={styles.label}>Notification Settings</span>
                  </NavLink>
                </li>
              </ul>
              )}
            </div>

            {/* Monitoring & Analytics */}
            <div className={styles.section}>
              <button
                className={styles.sectionToggle}
                onClick={() => toggleSection('monitoring')}
                title="Toggle Monitoring section"
              >
                <span className={styles.toggleChevron}>
                  {expandedSections.monitoring ? '▼' : '▶'}
                </span>
                <span className={styles.sectionTitleText}>Monitoring</span>
              </button>
              {expandedSections.monitoring && (
              <ul className={styles.menu}>
                <li>
                  <NavLink
                    to="/admin/analytics"
                    className={({ isActive }) =>
                      `${styles.navLink} ${isActive ? styles.active : ''}`
                    }
                  >
                    <span className={styles.icon}>📊</span>
                    <span className={styles.label}>Analytics & Reports</span>
                  </NavLink>
                </li>
                <li>
                  <NavLink
                    to="/admin/activity-log"
                    className={({ isActive }) =>
                      `${styles.navLink} ${isActive ? styles.active : ''}`
                    }
                  >
                    <span className={styles.icon}>📋</span>
                    <span className={styles.label}>Activity Log</span>
                  </NavLink>
                </li>
              </ul>
              )}
            </div>

            {/* System Management */}
            <div className={styles.section}>
              <button
                className={styles.sectionToggle}
                onClick={() => toggleSection('system')}
                title="Toggle System section"
              >
                <span className={styles.toggleChevron}>
                  {expandedSections.system ? '▼' : '▶'}
                </span>
                <span className={styles.sectionTitleText}>System</span>
              </button>
              {expandedSections.system && (
              <ul className={styles.menu}>
                <li>
                  <NavLink
                    to="/admin/logs"
                    className={({ isActive }) =>
                      `${styles.navLink} ${isActive ? styles.active : ''}`
                    }
                  >
                    <span className={styles.icon}>📝</span>
                    <span className={styles.label}>System Logs</span>
                  </NavLink>
                </li>
                <li>
                  <NavLink
                    to="/admin/database"
                    className={({ isActive }) =>
                      `${styles.navLink} ${isActive ? styles.active : ''}`
                    }
                  >
                    <span className={styles.icon}>🗄️</span>
                    <span className={styles.label}>Database Management</span>
                  </NavLink>
                </li>
              </ul>
              )}
            </div>
          </>
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
            <li>
              <NavLink
                to="/privacy"
                className={({ isActive }) =>
                  `${styles.navLink} ${isActive ? styles.active : ''}`
                }
              >
                <span className={styles.icon}>🔒</span>
                <span className={styles.label}>Privacy Policy</span>
              </NavLink>
            </li>
          </ul>
        </div>
      </nav>
    </aside>
  );
};

export default Sidebar;
