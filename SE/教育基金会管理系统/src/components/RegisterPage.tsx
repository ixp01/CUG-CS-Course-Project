import { useState } from 'react';
import { Button } from './ui/button';
import { Input } from './ui/input';
import { Label } from './ui/label';
import { User, Lock, Phone, GraduationCap, ArrowLeft } from 'lucide-react';
import { addUser } from '../utils/storage';
import { toast } from 'sonner@2.0.3';

interface RegisterPageProps {
  onNavigateToLogin: () => void;
}

export function RegisterPage({ onNavigateToLogin }: RegisterPageProps) {
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [confirmPassword, setConfirmPassword] = useState('');
  const [phone, setPhone] = useState('');
  const [showPassword, setShowPassword] = useState(false);

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();

    if (!username || !password || !phone) {
      toast.error('请填写所有必填字段');
      return;
    }

    if (password !== confirmPassword) {
      toast.error('两次输入的密码不一致');
      return;
    }

    if (password.length < 6) {
      toast.error('密码长度至少为6位');
      return;
    }

    const phoneRegex = /^1[3-9]\d{9}$/;
    if (!phoneRegex.test(phone)) {
      toast.error('请输入有效的手机号');
      return;
    }

    const success = addUser({ username, password, phone });
    if (success) {
      toast.success('注册成功！请登录');
      onNavigateToLogin();
    } else {
      toast.error('用户名已存在');
    }
  };

  return (
    <div className="min-h-screen flex">
      {/* 左侧品牌展示区 */}
      <div className="hidden lg:flex lg:w-2/5 bg-gradient-to-br from-green-600 via-green-700 to-green-900 p-12 flex-col justify-between text-white">
        <div>
          <div className="flex items-center gap-3 mb-8">
            <div className="w-12 h-12 bg-white/20 rounded-lg flex items-center justify-center backdrop-blur-sm">
              <GraduationCap className="w-7 h-7" />
            </div>
            <div>
              <h1 className="text-2xl">教育基金会</h1>
              <p className="text-sm text-green-200">捐助管理系统</p>
            </div>
          </div>

          <div className="space-y-6">
            <div>
              <h2 className="text-4xl mb-4">加入我们</h2>
              <p className="text-lg text-green-100">
                开始您的教育公益之旅
              </p>
            </div>

            <div className="space-y-4 pt-8">
              <div className="flex items-start gap-3">
                <div className="w-8 h-8 bg-white/10 rounded-full flex items-center justify-center backdrop-blur-sm flex-shrink-0 mt-1">
                  <span>1</span>
                </div>
                <div>
                  <div className="mb-1">创建账户</div>
                  <div className="text-sm text-green-200">填写基本信息完成注册</div>
                </div>
              </div>

              <div className="flex items-start gap-3">
                <div className="w-8 h-8 bg-white/10 rounded-full flex items-center justify-center backdrop-blur-sm flex-shrink-0 mt-1">
                  <span>2</span>
                </div>
                <div>
                  <div className="mb-1">验证身份</div>
                  <div className="text-sm text-green-200">确保账户安全性</div>
                </div>
              </div>

              <div className="flex items-start gap-3">
                <div className="w-8 h-8 bg-white/10 rounded-full flex items-center justify-center backdrop-blur-sm flex-shrink-0 mt-1">
                  <span>3</span>
                </div>
                <div>
                  <div className="mb-1">开始管理</div>
                  <div className="text-sm text-green-200">访问完整的管理功能</div>
                </div>
              </div>
            </div>
          </div>
        </div>

        <div className="text-sm text-green-200">
          © 2024 教育基金会管理系统. All rights reserved.
        </div>
      </div>

      {/* 右侧注册表单区 */}
      <div className="flex-1 flex items-center justify-center p-8 bg-gray-50">
        <div className="w-full max-w-md">
          <div className="bg-white rounded-2xl shadow-xl p-8">
            <button
              onClick={onNavigateToLogin}
              className="flex items-center gap-2 text-gray-600 hover:text-gray-900 mb-6"
            >
              <ArrowLeft className="w-4 h-4" />
              返回登录
            </button>

            <div className="mb-8">
              <h2 className="text-3xl mb-2">注册新账户</h2>
              <p className="text-gray-600">创建您的管理账户</p>
            </div>

            <form onSubmit={handleSubmit} className="space-y-5">
              <div className="space-y-2">
                <Label htmlFor="username">用户名 *</Label>
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
                <Label htmlFor="phone">手机号 *</Label>
                <div className="relative">
                  <Phone className="absolute left-3 top-1/2 -translate-y-1/2 w-5 h-5 text-gray-400" />
                  <Input
                    id="phone"
                    type="tel"
                    placeholder="请输入手机号"
                    className="pl-10"
                    value={phone}
                    onChange={(e) => setPhone(e.target.value)}
                  />
                </div>
              </div>

              <div className="space-y-2">
                <Label htmlFor="password">密码 *</Label>
                <div className="relative">
                  <Lock className="absolute left-3 top-1/2 -translate-y-1/2 w-5 h-5 text-gray-400" />
                  <Input
                    id="password"
                    type={showPassword ? 'text' : 'password'}
                    placeholder="请输入密码（至少6位）"
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

              <div className="space-y-2">
                <Label htmlFor="confirmPassword">确认密码 *</Label>
                <div className="relative">
                  <Lock className="absolute left-3 top-1/2 -translate-y-1/2 w-5 h-5 text-gray-400" />
                  <Input
                    id="confirmPassword"
                    type={showPassword ? 'text' : 'password'}
                    placeholder="请再次输入密码"
                    className="pl-10"
                    value={confirmPassword}
                    onChange={(e) => setConfirmPassword(e.target.value)}
                  />
                </div>
              </div>

              <Button type="submit" className="w-full">
                注册
              </Button>
            </form>

            <div className="mt-6 text-center">
              <span className="text-gray-600">已有账户？</span>
              <button
                onClick={onNavigateToLogin}
                className="ml-2 text-blue-600 hover:text-blue-700"
              >
                立即登录
              </button>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
