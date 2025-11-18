import { BrowserRouter as Router, Routes, Route, Navigate } from 'react-router-dom';
import { AuthProvider } from './contexts/AuthContext';
import { SignalRProvider } from './contexts/SignalRContext';
import { MountPointsProvider } from './contexts/MountPointsContext';
import { ClientPositionsProvider } from './contexts/ClientPositionsContext';
import { DashboardStatsProvider } from './contexts/DashboardStatsContext';
import ProtectedRoute from './components/ProtectedRoute';
import { AlertStack } from './components/Alerts/AlertStack';
import { GdprConsent } from './components/GdprConsent/GdprConsent';
import LoginPage from './pages/auth/LoginPage';
import RegisterPage from './pages/auth/RegisterPage';
import VerifyEmailPage from './pages/auth/VerifyEmailPage';
import DashboardPage from './pages/dashboard/DashboardPage';
import MySourcesPage from './pages/dashboard/MySourcesPage';
import AvailableSourcesPage from './pages/dashboard/AvailableSourcesPage';
import { ProfilePage } from './pages/profile/ProfilePage';
import { PrivacyPage } from './pages/privacy/PrivacyPage';
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
import { PerformancePage } from './pages/admin/PerformancePage';
import { SystemLogsPage } from './pages/admin/SystemLogsPage';
import { DatabaseManagementPage } from './pages/admin/DatabaseManagementPage';
import { NotificationSettingsPage } from './pages/admin/NotificationSettingsPage';
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
          <MountPointsProvider>
            <ClientPositionsProvider>
              <DashboardStatsProvider>
                <AlertStack />
                <GdprConsent />
                <Routes>
          {/* Public routes */}
          <Route path="/login" element={<LoginPage />} />
          <Route path="/register" element={<RegisterPage />} />
          <Route path="/auth/verify-email" element={<VerifyEmailPage />} />
          <Route path="/privacy" element={<PrivacyPage />} />

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
              <ProtectedRoute requiredRoles={['Admin', 'ReadOnly']}>
                <UsersManagement />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/groups"
            element={
              <ProtectedRoute requiredRoles={['Admin', 'ReadOnly']}>
                <GroupsManagement />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/mountpoints"
            element={
              <ProtectedRoute requiredRoles={['Admin', 'ReadOnly']}>
                <MountPointsManagement />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/caster-config"
            element={
              <ProtectedRoute requiredRoles={['Admin', 'ReadOnly']}>
                <CasterConfigPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/network-config"
            element={
              <ProtectedRoute requiredRoles={['Admin', 'ReadOnly']}>
                <NetworkConfigPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin"
            element={
              <ProtectedRoute requiredRoles={['Admin', 'ReadOnly']}>
                <AdminDashboardPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/system-settings"
            element={
              <ProtectedRoute requiredRoles={['Admin', 'ReadOnly']}>
                <SystemSettingsPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/activity-log"
            element={
              <ProtectedRoute requiredRoles={['Admin', 'ReadOnly']}>
                <ActivityLogPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/security"
            element={
              <ProtectedRoute requiredRoles={['Admin', 'ReadOnly']}>
                <SecuritySettingsPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/analytics"
            element={
              <ProtectedRoute requiredRoles={['Admin', 'ReadOnly']}>
                <AnalyticsPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/performance"
            element={
              <ProtectedRoute requiredRoles={['Admin', 'ReadOnly']}>
                <PerformancePage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/logs"
            element={
              <ProtectedRoute requiredRoles={['Admin', 'ReadOnly']}>
                <SystemLogsPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/database"
            element={
              <ProtectedRoute requiredRoles={['Admin', 'ReadOnly']}>
                <DatabaseManagementPage />
              </ProtectedRoute>
            }
          />

          <Route
            path="/admin/notification-settings"
            element={
              <ProtectedRoute requiredRoles={['Admin', 'ReadOnly']}>
                <NotificationSettingsPage />
              </ProtectedRoute>
            }
          />

          {/* Redirect root to dashboard if authenticated, else to login */}
          <Route path="/" element={<Navigate to="/dashboard" replace />} />

          {/* 404 */}
          <Route path="*" element={<NotFoundPage />} />
                </Routes>
              </DashboardStatsProvider>
            </ClientPositionsProvider>
          </MountPointsProvider>
        </SignalRProvider>
      </AuthProvider>
    </Router>
  );
}

export default App;
