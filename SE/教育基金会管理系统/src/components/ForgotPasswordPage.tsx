import { useState } from 'react';
import { Button } from './ui/button';
import { Input } from './ui/input';
import { Label } from './ui/label';
import { User, Phone, GraduationCap, ArrowLeft, CheckCircle } from 'lucide-react';
import { findUserByCredentials } from '../utils/storage';
import { toast } from 'sonner@2.0.3';

interface ForgotPasswordPageProps {
  onNavigateToLogin: () => void;
}

export function ForgotPasswordPage({ onNavigateToLogin }: ForgotPasswordPageProps) {
  const [username, setUsername] = useState('');
  const [phone, setPhone] = useState('');
  const [foundPassword, setFoundPassword] = useState('');

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();

    if (!username || !phone) {
      toast.error('请输入用户名和手机号');
      return;
    }

    const user = findUserByCredentials(username, phone);
    if (user) {
      setFoundPassword(user.password);
      toast.success('验证成功！');
    } else {
      toast.error('用户名或手机号错误');
      setFoundPassword('');
    }
  };

  return (
    <div className="min-h-screen flex">
      {/* 左侧品牌展示区 */}
      <div className="hidden lg:flex lg:w-2/5 bg-gradient-to-br from-purple-600 via-purple-700 to-purple-900 p-12 flex-col justify-between text-white">
        <div>
          <div className="flex items-center gap-3 mb-8">
            <div className="w-12 h-12 bg-white/20 rounded-lg flex items-center justify-center backdrop-blur-sm">
              <GraduationCap className="w-7 h-7" />
            </div>
            <div>
              <h1 className="text-2xl">教育基金会</h1>
              <p className="text-sm text-purple-200">捐助管理系统</p>
            </div>
          </div>

          <div className="space-y-6">
            <div>
              <h2 className="text-4xl mb-4">找回密码</h2>
              <p className="text-lg text-purple-100">
                通过注册信息验证您的身份
              </p>
            </div>

            <div className="space-y-4 pt-8">
              <div className="flex items-start gap-3">
                <div className="w-8 h-8 bg-white/10 rounded-full flex items-center justify-center backdrop-blur-sm flex-shrink-0 mt-1">
                  <span>1</span>
                </div>
                <div>
                  <div className="mb-1">输入账户信息</div>
                  <div className="text-sm text-purple-200">填写您的用户名和注册手机号</div>
                </div>
              </div>

              <div className="flex items-start gap-3">
                <div className="w-8 h-8 bg-white/10 rounded-full flex items-center justify-center backdrop-blur-sm flex-shrink-0 mt-1">
                  <span>2</span>
                </div>
                <div>
                  <div className="mb-1">验证身份</div>
                  <div className="text-sm text-purple-200">系统将验证您的信息</div>
                </div>
              </div>

              <div className="flex items-start gap-3">
                <div className="w-8 h-8 bg-white/10 rounded-full flex items-center justify-center backdrop-blur-sm flex-shrink-0 mt-1">
                  <span>3</span>
                </div>
                <div>
                  <div className="mb-1">获取密码</div>
                  <div className="text-sm text-purple-200">查看您的账户密码</div>
                </div>
              </div>
            </div>
          </div>
        </div>

        <div className="text-sm text-purple-200">
          © 2024 教育基金会管理系统. All rights reserved.
        </div>
      </div>

      {/* 右侧找回密码表单区 */}
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
              <h2 className="text-3xl mb-2">找回密码</h2>
              <p className="text-gray-600">请输入您的账户信息进行验证</p>
            </div>

            {!foundPassword ? (
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
                  <Label htmlFor="phone">手机号</Label>
                  <div className="relative">
                    <Phone className="absolute left-3 top-1/2 -translate-y-1/2 w-5 h-5 text-gray-400" />
                    <Input
                      id="phone"
                      type="tel"
                      placeholder="请输入注册手机号"
                      className="pl-10"
                      value={phone}
                      onChange={(e) => setPhone(e.target.value)}
                    />
                  </div>
                </div>

                <Button type="submit" className="w-full">
                  找回密码
                </Button>
              </form>
            ) : (
              <div className="space-y-6">
                <div className="flex items-center justify-center">
                  <div className="w-16 h-16 bg-green-100 rounded-full flex items-center justify-center">
                    <CheckCircle className="w-10 h-10 text-green-600" />
                  </div>
                </div>

                <div className="text-center">
                  <h3 className="text-xl mb-2">验证成功！</h3>
                  <p className="text-gray-600 mb-6">您的密码为：</p>
                  <div className="bg-blue-50 border-2 border-blue-200 rounded-lg p-6">
                    <div className="text-3xl text-blue-600 tracking-wider">
                      {foundPassword}
                    </div>
                  </div>
                  <p className="text-sm text-gray-500 mt-4">
                    请妥善保管您的密码
                  </p>
                </div>

                <Button onClick={onNavigateToLogin} className="w-full">
                  返回登录
                </Button>
              </div>
            )}

            {!foundPassword && (
              <div className="mt-6 text-center">
                <span className="text-gray-600">想起密码了？</span>
                <button
                  onClick={onNavigateToLogin}
                  className="ml-2 text-blue-600 hover:text-blue-700"
                >
                  立即登录
                </button>
              </div>
            )}
          </div>
        </div>
      </div>
    </div>
  );
}
