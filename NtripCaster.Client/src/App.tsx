import { BrowserRouter as Router, Routes, Route, Navigate } from 'react-router-dom';
import { AuthProvider } from './contexts/AuthContext';
import ProtectedRoute from './components/ProtectedRoute';
import LoginPage from './pages/auth/LoginPage';
import RegisterPage from './pages/auth/RegisterPage';
import './App.css';

// Placeholder pages for now
const DashboardPage = () => (
  <div style={{ padding: '2rem', color: '#333' }}>
    <h1>Dashboard</h1>
    <p>Welcome to NtripCaster Dashboard! This page will display real-time GNSS data.</p>
  </div>
);

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
