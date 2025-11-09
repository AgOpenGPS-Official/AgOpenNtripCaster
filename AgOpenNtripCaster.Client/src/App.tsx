import { BrowserRouter as Router, Routes, Route, Navigate } from 'react-router-dom';
import { AuthProvider } from './contexts/AuthContext';
import { SignalRProvider } from './contexts/SignalRContext';
import ProtectedRoute from './components/ProtectedRoute';
import { AlertStack } from './components/Alerts/AlertStack';
import LoginPage from './pages/auth/LoginPage';
import RegisterPage from './pages/auth/RegisterPage';
import DashboardPage from './pages/dashboard/DashboardPage';
import MySourcesPage from './pages/dashboard/MySourcesPage';
import AvailableSourcesPage from './pages/dashboard/AvailableSourcesPage';
import { ProfilePage } from './pages/profile/ProfilePage';
import UsersManagement from './pages/admin/UsersManagement';
import GroupsManagement from './pages/admin/GroupsManagement';
import MountPointsManagement from './pages/admin/MountPointsManagement';
import { CasterConfigPage } from './pages/admin/CasterConfigPage';
import { NetworkConfigPage } from './pages/admin/NetworkConfigPage';
import { AdminDashboardPage } from './pages/admin/AdminDashboardPage';
import { SystemSettingsPage } from './pages/admin/SystemSettingsPage';
import { ActivityLogPage } from './pages/admin/ActivityLogPage';
import { SecuritySettingsPage } from './pages/admin/SecuritySettingsPage';
import { AnalyticsPage } from './pages/admin/AnalyticsPage';
import { SystemLogsPage } from './pages/admin/SystemLogsPage';
import { DatabaseManagementPage } from './pages/admin/DatabaseManagementPage';
import { EmailSettingsPage } from './pages/admin/EmailSettingsPage';
import './styles/globals.css';
import './App.css';

const NotFoundPage = () => (
  <div style={{ padding: '2rem', textAlign: 'center', color: '#333' }}>
    <h1>404 - Page Not Found</h1>
    <a href="/">Go to Home</a>
  </div>
);

function App() {
  return (
    <Router>
      <AuthProvider>
        <SignalRProvider>
          <AlertStack />
          <Routes>
          {/* Public auth routes */}
          <Route path="/login" element={<LoginPage />} />
          <Route path="/register" element={<RegisterPage />} />

          {/* Protected routes */}
          <Route
            path="/dashboard"
            element={
              <ProtectedRoute>
                <DashboardPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/dashboard/my-sources"
            element={
              <ProtectedRoute>
                <MySourcesPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/dashboard/available-sources"
            element={
              <ProtectedRoute>
                <AvailableSourcesPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/profile"
            element={
              <ProtectedRoute>
                <ProfilePage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/users"
            element={
              <ProtectedRoute>
                <UsersManagement />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/groups"
            element={
              <ProtectedRoute>
                <GroupsManagement />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/mountpoints"
            element={
              <ProtectedRoute>
                <MountPointsManagement />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/caster-config"
            element={
              <ProtectedRoute>
                <CasterConfigPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/network-config"
            element={
              <ProtectedRoute>
                <NetworkConfigPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin"
            element={
              <ProtectedRoute>
                <AdminDashboardPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/system-settings"
            element={
              <ProtectedRoute>
                <SystemSettingsPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/activity-log"
            element={
              <ProtectedRoute>
                <ActivityLogPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/security"
            element={
              <ProtectedRoute>
                <SecuritySettingsPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/analytics"
            element={
              <ProtectedRoute>
                <AnalyticsPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/logs"
            element={
              <ProtectedRoute>
                <SystemLogsPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/database"
            element={
              <ProtectedRoute>
                <DatabaseManagementPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/email-settings"
            element={
              <ProtectedRoute>
                <EmailSettingsPage />
              </ProtectedRoute>
            }
          />

          {/* Redirect root to dashboard if authenticated, else to login */}
          <Route path="/" element={<Navigate to="/dashboard" replace />} />

          {/* 404 */}
          <Route path="*" element={<NotFoundPage />} />
        </Routes>
        </SignalRProvider>
      </AuthProvider>
    </Router>
  );
}

export default App;
