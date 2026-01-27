import { useState } from 'react';
import { Button } from './ui/button';
import { Input } from './ui/input';
import { Label } from './ui/label';
import { Checkbox } from './ui/checkbox';
import { User, Lock, GraduationCap, Shield, Users } from 'lucide-react';
import { validateLogin, setCurrentUser } from '../utils/storage';
import { toast } from 'sonner@2.0.3';

interface LoginPageProps {
  onLogin: () => void;
  onNavigateToRegister: () => void;
  onNavigateToForgotPassword: () => void;
}

export function LoginPage({ onLogin, onNavigateToRegister, onNavigateToForgotPassword }: LoginPageProps) {
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [showPassword, setShowPassword] = useState(false);
  const [rememberMe, setRememberMe] = useState(false);

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    
    if (!username || !password) {
      toast.error('请输入用户名和密码');
      return;
    }

    if (validateLogin(username, password)) {
      setCurrentUser(username);
      toast.success('登录成功！');
      onLogin();
    } else {
      toast.error('用户名或密码错误');
    }
  };

  return (
    <div className="min-h-screen flex">
      {/* 左侧品牌展示区 */}
      <div className="hidden lg:flex lg:w-2/5 bg-gradient-to-br from-blue-600 via-blue-700 to-blue-900 p-12 flex-col justify-between text-white">
        <div>
          <div className="flex items-center gap-3 mb-8">
            <div className="w-12 h-12 bg-white/20 rounded-lg flex items-center justify-center backdrop-blur-sm">
              <GraduationCap className="w-7 h-7" />
            </div>
            <div>
              <h1 className="text-2xl">教育基金会</h1>
              <p className="text-sm text-blue-200">捐助管理系统</p>
            </div>
          </div>

          <div className="space-y-6">
            <div>
              <h2 className="text-4xl mb-4">专业、透明、高效</h2>
              <p className="text-lg text-blue-100">
                全方位的教育基金管理解决方案
              </p>
            </div>

            <div className="space-y-4 pt-8">
              <div className="flex items-center gap-3">
                <div className="w-10 h-10 bg-white/10 rounded-lg flex items-center justify-center backdrop-blur-sm">
                  <Shield className="w-5 h-5" />
                </div>
                <div>
                  <div className="text-2xl">1,258+</div>
                  <div className="text-sm text-blue-200">已保护的捐助项目</div>
                </div>
              </div>

              <div className="flex items-center gap-3">
                <div className="w-10 h-10 bg-white/10 rounded-lg flex items-center justify-center backdrop-blur-sm">
                  <Users className="w-5 h-5" />
                </div>
                <div>
                  <div className="text-2xl">86+</div>
                  <div className="text-sm text-blue-200">服务的教育机构</div>
                </div>
              </div>
            </div>
          </div>
        </div>

        <div className="text-sm text-blue-200">
          © 2024 教育基金会管理系统. All rights reserved.
        </div>
      </div>

      {/* 右侧登录表单区 */}
      <div className="flex-1 flex items-center justify-center p-8 bg-gray-50">
        <div className="w-full max-w-md">
          <div className="bg-white rounded-2xl shadow-xl p-8">
            <div className="mb-8">
              <h2 className="text-3xl mb-2">登录账户</h2>
              <p className="text-gray-600">欢迎回来，请输入您的登录信息</p>
            </div>

            <form onSubmit={handleSubmit} className="space-y-5">
              <div className="space-y-2">
                <Label htmlFor="username">用户名</Label>
                <div className="relative">
                  <User className="absolute left-3 top-1/2 -translate-y-1/2 w-5 h-5 text-gray-400" />
                  <Input
                    id="username"
                    type="text"
                    placeholder="请输入用户名"
                    className="pl-10"
                    value={username}
                    onChange={(e) => setUsername(e.target.value)}
                  />
                </div>
              </div>

              <div className="space-y-2">
                <Label htmlFor="password">密码</Label>
                <div className="relative">
                  <Lock className="absolute left-3 top-1/2 -translate-y-1/2 w-5 h-5 text-gray-400" />
                  <Input
                    id="password"
                    type={showPassword ? 'text' : 'password'}
                    placeholder="请输入密码"
                    className="pl-10 pr-20"
                    value={password}
                    onChange={(e) => setPassword(e.target.value)}
                  />
                  <button
                    type="button"
                    onClick={() => setShowPassword(!showPassword)}
                    className="absolute right-3 top-1/2 -translate-y-1/2 text-sm text-blue-600 hover:text-blue-700"
                  >
                    {showPassword ? '隐藏' : '显示'}
                  </button>
                </div>
              </div>

              <div className="flex items-center justify-between">
                <div className="flex items-center gap-2">
                  <Checkbox
                    id="remember"
                    checked={rememberMe}
                    onCheckedChange={(checked) => setRememberMe(checked as boolean)}
                  />
                  <Label htmlFor="remember" className="text-sm cursor-pointer">
                    记住我
                  </Label>
                </div>
                <button
                  type="button"
                  onClick={onNavigateToForgotPassword}
                  className="text-sm text-blue-600 hover:text-blue-700"
                >
                  忘记密码？
                </button>
              </div>

              <Button type="submit" className="w-full">
                登录
              </Button>
            </form>

            <div className="mt-6 text-center">
              <span className="text-gray-600">还没有账户？</span>
              <button
                onClick={onNavigateToRegister}
                className="ml-2 text-blue-600 hover:text-blue-700"
              >
                注册新账户
              </button>
            </div>

            <div className="mt-6 p-4 bg-blue-50 rounded-lg">
              <p className="text-sm text-gray-600 text-center">
                演示账户: <span className="text-blue-600">user</span> / <span className="text-blue-600">123456</span>
              </p>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
