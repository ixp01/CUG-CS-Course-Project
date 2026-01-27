import { GraduationCap, LogOut, LayoutDashboard, Heart, Wallet, FileText } from 'lucide-react';
import { Button } from './ui/button';
import { getCurrentUser, logout } from '../utils/storage';

type Page = 'dashboard' | 'donations' | 'fundings' | 'reports';

interface LayoutProps {
  currentPage: Page;
  onNavigate: (page: Page) => void;
  onLogout: () => void;
  children: React.ReactNode;
}

export function Layout({ currentPage, onNavigate, onLogout, children }: LayoutProps) {
  const username = getCurrentUser();

  const handleLogout = () => {
    logout();
    onLogout();
  };

  const navItems = [
    { id: 'dashboard' as Page, label: '控制台', icon: LayoutDashboard },
    { id: 'donations' as Page, label: '捐助管理', icon: Heart },
    { id: 'fundings' as Page, label: '用款申请', icon: Wallet },
    { id: 'reports' as Page, label: '财务报表', icon: FileText },
  ];

  return (
    <div className="min-h-screen bg-gray-50">
      {/* 顶部导航栏 */}
      <header className="bg-white border-b border-gray-200 sticky top-0 z-50">
        <div className="px-6 py-4">
          <div className="flex items-center justify-between">
            {/* 左侧Logo */}
            <div className="flex items-center gap-3">
              <div className="w-10 h-10 bg-blue-600 rounded-lg flex items-center justify-center">
                <GraduationCap className="w-6 h-6 text-white" />
              </div>
              <div>
                <h1 className="text-xl text-gray-900">教育基金会管理系统</h1>
              </div>
            </div>

            {/* 中部导航菜单 */}
            <nav className="hidden md:flex items-center gap-1">
              {navItems.map((item) => {
                const Icon = item.icon;
                const isActive = currentPage === item.id;
                return (
                  <button
                    key={item.id}
                    onClick={() => onNavigate(item.id)}
                    className={`flex items-center gap-2 px-4 py-2 rounded-lg transition-colors ${
                      isActive
                        ? 'bg-blue-50 text-blue-600'
                        : 'text-gray-600 hover:bg-gray-50 hover:text-gray-900'
                    }`}
                  >
                    <Icon className="w-5 h-5" />
                    <span>{item.label}</span>
                  </button>
                );
              })}
            </nav>

            {/* 右侧用户信息 */}
            <div className="flex items-center gap-4">
              <div className="text-right hidden sm:block">
                <div className="text-sm text-gray-900">{username}</div>
                <div className="text-xs text-gray-500">管理员</div>
              </div>
              <Button
                variant="outline"
                size="sm"
                onClick={handleLogout}
                className="flex items-center gap-2"
              >
                <LogOut className="w-4 h-4" />
                <span className="hidden sm:inline">退出</span>
              </Button>
            </div>
          </div>
        </div>
      </header>

      {/* 移动端导航 */}
      <div className="md:hidden bg-white border-b border-gray-200">
        <nav className="flex overflow-x-auto">
          {navItems.map((item) => {
            const Icon = item.icon;
            const isActive = currentPage === item.id;
            return (
              <button
                key={item.id}
                onClick={() => onNavigate(item.id)}
                className={`flex-1 flex flex-col items-center gap-1 px-4 py-3 min-w-[80px] ${
                  isActive
                    ? 'text-blue-600 border-b-2 border-blue-600'
                    : 'text-gray-600'
                }`}
              >
                <Icon className="w-5 h-5" />
                <span className="text-xs">{item.label}</span>
              </button>
            );
          })}
        </nav>
      </div>

      {/* 主要内容区 */}
      <main className="p-6">
        {children}
      </main>
    </div>
  );
}
