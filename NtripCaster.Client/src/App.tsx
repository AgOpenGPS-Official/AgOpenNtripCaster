import { BrowserRouter as Router, Routes, Route, Navigate } from 'react-router-dom';
import { AuthProvider } from './contexts/AuthContext';
import ProtectedRoute from './components/ProtectedRoute';
import LoginPage from './pages/auth/LoginPage';
import RegisterPage from './pages/auth/RegisterPage';
import DashboardPage from './pages/dashboard/DashboardPage';
import MySourcesPage from './pages/dashboard/MySourcesPage';
import AvailableSourcesPage from './pages/dashboard/AvailableSourcesPage';
import UsersManagement from './pages/admin/UsersManagement';
import GroupsManagement from './pages/admin/GroupsManagement';
import MountPointsManagement from './pages/admin/MountPointsManagement';
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

          {/* Redirect root to dashboard if authenticated, else to login */}
          <Route path="/" element={<Navigate to="/dashboard" replace />} />

          {/* 404 */}
          <Route path="*" element={<NotFoundPage />} />
        </Routes>
      </AuthProvider>
    </Router>
  );
}

export default App;
