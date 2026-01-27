import { useState, useEffect } from 'react';
import { LoginPage } from './components/LoginPage';
import { RegisterPage } from './components/RegisterPage';
import { ForgotPasswordPage } from './components/ForgotPasswordPage';
import { Dashboard } from './components/Dashboard';
import { DonationManagement } from './components/DonationManagement';
import { FundingManagement } from './components/FundingManagement';
import { FinancialReport } from './components/FinancialReport';
import { Layout } from './components/Layout';
import { getCurrentUser, initializeDefaultUser } from './utils/storage';
import { Toaster } from './components/ui/sonner';

type Page = 'login' | 'register' | 'forgot-password' | 'dashboard' | 'donations' | 'fundings' | 'reports';

export default function App() {
  const [currentPage, setCurrentPage] = useState<Page>('login');
  const [isAuthenticated, setIsAuthenticated] = useState(false);

  useEffect(() => {
    // 初始化默认用户
    initializeDefaultUser();
    
    // 检查是否已登录
    const user = getCurrentUser();
    if (user) {
      setIsAuthenticated(true);
      setCurrentPage('dashboard');
    }
  }, []);

  const handleLogin = () => {
    setIsAuthenticated(true);
    setCurrentPage('dashboard');
  };

  const handleLogout = () => {
    setIsAuthenticated(false);
    setCurrentPage('login');
  };

  // 如果未登录，显示登录相关页面
  if (!isAuthenticated) {
    return (
      <>
        {currentPage === 'login' && (
          <LoginPage 
            onLogin={handleLogin}
            onNavigateToRegister={() => setCurrentPage('register')}
            onNavigateToForgotPassword={() => setCurrentPage('forgot-password')}
          />
        )}
        {currentPage === 'register' && (
          <RegisterPage onNavigateToLogin={() => setCurrentPage('login')} />
        )}
        {currentPage === 'forgot-password' && (
          <ForgotPasswordPage onNavigateToLogin={() => setCurrentPage('login')} />
        )}
        <Toaster />
      </>
    );
  }

  // 已登录，显示主应用
  return (
    <>
      <Layout currentPage={currentPage} onNavigate={setCurrentPage} onLogout={handleLogout}>
        {currentPage === 'dashboard' && <Dashboard />}
        {currentPage === 'donations' && <DonationManagement />}
        {currentPage === 'fundings' && <FundingManagement />}
        {currentPage === 'reports' && <FinancialReport />}
      </Layout>
      <Toaster />
    </>
  );
}
