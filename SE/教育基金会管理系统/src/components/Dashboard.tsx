import { useEffect, useState } from 'react';
import { Card, CardContent, CardHeader, CardTitle } from './ui/card';
import { Button } from './ui/button';
import { Badge } from './ui/badge';
import { Table, TableBody, TableCell, TableHead, TableHeader, TableRow } from './ui/table';
import { TrendingUp, TrendingDown, Wallet, Plus } from 'lucide-react';
import { getDonations, getFundings, calculateMonthlyFinancials, getFinancials } from '../utils/storage';
import { DonationApplication, FundingApplication } from '../types';
import { DonationFormModal } from './DonationFormModal';
import { FundingFormModal } from './FundingFormModal';

export function Dashboard() {
  const [donations, setDonations] = useState<DonationApplication[]>([]);
  const [fundings, setFundings] = useState<FundingApplication[]>([]);
  const [monthlyIncome, setMonthlyIncome] = useState(0);
  const [monthlyExpense, setMonthlyExpense] = useState(0);
  const [currentBalance, setCurrentBalance] = useState(0);
  const [showDonationModal, setShowDonationModal] = useState(false);
  const [showFundingModal, setShowFundingModal] = useState(false);

  useEffect(() => {
    loadData();
  }, []);

  const loadData = () => {
    const allDonations = getDonations();
    const allFundings = getFundings();
    setDonations(allDonations);
    setFundings(allFundings);

    // 计算月度财务数据
    calculateMonthlyFinancials();
    
    // 获取当前月份
    const currentMonth = new Date().toISOString().substring(0, 7);
    const financials = getFinancials();
    const currentMonthData = financials.find(f => f.month === currentMonth);

    if (currentMonthData) {
      setMonthlyIncome(currentMonthData.income);
      setMonthlyExpense(currentMonthData.expense);
    } else {
      setMonthlyIncome(0);
      setMonthlyExpense(0);
    }

    // 计算累计余额
    const totalBalance = financials.reduce((sum, f) => {
      const lastMonth = financials[financials.length - 1];
      return lastMonth ? lastMonth.balance : 0;
    }, 0);
    setCurrentBalance(totalBalance);
  };

  const formatCurrency = (amount: number) => {
    return new Intl.NumberFormat('zh-CN', {
      style: 'currency',
      currency: 'CNY',
    }).format(amount);
  };

  const formatDate = (dateString: string) => {
    return new Date(dateString).toLocaleDateString('zh-CN');
  };

  const getStatusBadge = (status: string) => {
    const statusMap = {
      pending: { label: '待审核', className: 'bg-orange-100 text-orange-700' },
      approved: { label: '已通过', className: 'bg-green-100 text-green-700' },
      rejected: { label: '已拒绝', className: 'bg-red-100 text-red-700' },
      completed: { label: '已完成', className: 'bg-blue-100 text-blue-700' },
    };
    const config = statusMap[status as keyof typeof statusMap] || statusMap.pending;
    return <Badge className={config.className}>{config.label}</Badge>;
  };

  const recentDonations = donations.slice(-5).reverse();
  const recentFundings = fundings.slice(-5).reverse();

  const currentDate = new Date().toLocaleDateString('zh-CN', {
    year: 'numeric',
    month: 'long',
    day: 'numeric',
    weekday: 'long'
  });

  return (
    <div className="max-w-7xl mx-auto space-y-6">
      {/* 页面标题 */}
      <div className="flex items-center justify-between">
        <div>
          <h1 className="text-3xl text-gray-900 mb-2">控制台</h1>
          <p className="text-gray-600">{currentDate}</p>
        </div>
      </div>

      {/* 统计卡片 */}
      <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
        {/* 本月收入 */}
        <Card>
          <CardHeader className="flex flex-row items-center justify-between pb-2">
            <CardTitle className="text-sm text-gray-600">本月收入</CardTitle>
            <div className="w-10 h-10 bg-green-100 rounded-lg flex items-center justify-center">
              <TrendingUp className="w-5 h-5 text-green-600" />
            </div>
          </CardHeader>
          <CardContent>
            <div className="text-3xl text-gray-900 mb-1">
              {formatCurrency(monthlyIncome)}
            </div>
            <p className="text-sm text-gray-500">
              来自捐助收入
            </p>
          </CardContent>
        </Card>

        {/* 本月支出 */}
        <Card>
          <CardHeader className="flex flex-row items-center justify-between pb-2">
            <CardTitle className="text-sm text-gray-600">本月支出</CardTitle>
            <div className="w-10 h-10 bg-red-100 rounded-lg flex items-center justify-center">
              <TrendingDown className="w-5 h-5 text-red-600" />
            </div>
          </CardHeader>
          <CardContent>
            <div className="text-3xl text-gray-900 mb-1">
              {formatCurrency(monthlyExpense)}
            </div>
            <p className="text-sm text-gray-500">
              教育用款支出
            </p>
          </CardContent>
        </Card>

        {/* 当前余额 */}
        <Card>
          <CardHeader className="flex flex-row items-center justify-between pb-2">
            <CardTitle className="text-sm text-gray-600">当前余额</CardTitle>
            <div className="w-10 h-10 bg-blue-100 rounded-lg flex items-center justify-center">
              <Wallet className="w-5 h-5 text-blue-600" />
            </div>
          </CardHeader>
          <CardContent>
            <div className="text-3xl text-gray-900 mb-1">
              {formatCurrency(currentBalance)}
            </div>
            <p className="text-sm text-gray-500">
              可用资金总额
            </p>
          </CardContent>
        </Card>
      </div>

      {/* 近期捐助申请 */}
      <Card>
        <CardHeader className="flex flex-row items-center justify-between">
          <CardTitle>近期捐助申请</CardTitle>
          <Button size="sm" className="flex items-center gap-2" onClick={() => setShowDonationModal(true)}>
            <Plus className="w-4 h-4" />
            新增捐助申请
          </Button>
        </CardHeader>
        <CardContent>
          <Table>
            <TableHeader>
              <TableRow>
                <TableHead>申请人</TableHead>
                <TableHead>类型</TableHead>
                <TableHead>金额</TableHead>
                <TableHead>日期</TableHead>
                <TableHead>状态</TableHead>
                <TableHead>操作</TableHead>
              </TableRow>
            </TableHeader>
            <TableBody>
              {recentDonations.length === 0 ? (
                <TableRow>
                  <TableCell colSpan={6} className="text-center text-gray-500 py-8">
                    暂无捐助申请
                  </TableCell>
                </TableRow>
              ) : (
                recentDonations.map((donation) => (
                  <TableRow key={donation.id}>
                    <TableCell>{donation.donorName}</TableCell>
                    <TableCell>
                      {donation.donorType === 'enterprise' ? '企业' : '个人'}
                    </TableCell>
                    <TableCell>{formatCurrency(donation.amount)}</TableCell>
                    <TableCell>{formatDate(donation.applicationDate)}</TableCell>
                    <TableCell>{getStatusBadge(donation.status)}</TableCell>
                    <TableCell>
                      <Button variant="ghost" size="sm">
                        查看
                      </Button>
                    </TableCell>
                  </TableRow>
                ))
              )}
            </TableBody>
          </Table>
        </CardContent>
      </Card>

      {/* 待处理用款申请 */}
      <Card>
        <CardHeader className="flex flex-row items-center justify-between">
          <CardTitle>待处理用款申请</CardTitle>
          <Button size="sm" className="flex items-center gap-2" onClick={() => setShowFundingModal(true)}>
            <Plus className="w-4 h-4" />
            新增用款申请
          </Button>
        </CardHeader>
        <CardContent>
          <Table>
            <TableHeader>
              <TableRow>
                <TableHead>申请单位</TableHead>
                <TableHead>金额</TableHead>
                <TableHead>日期</TableHead>
                <TableHead>状态</TableHead>
                <TableHead>操作</TableHead>
              </TableRow>
            </TableHeader>
            <TableBody>
              {recentFundings.length === 0 ? (
                <TableRow>
                  <TableCell colSpan={5} className="text-center text-gray-500 py-8">
                    暂无用款申请
                  </TableCell>
                </TableRow>
              ) : (
                recentFundings.map((funding) => (
                  <TableRow key={funding.id}>
                    <TableCell>{funding.institutionName}</TableCell>
                    <TableCell>{formatCurrency(funding.amount)}</TableCell>
                    <TableCell>{formatDate(funding.applicationDate)}</TableCell>
                    <TableCell>{getStatusBadge(funding.status)}</TableCell>
                    <TableCell>
                      <Button variant="ghost" size="sm">
                        查看
                      </Button>
                    </TableCell>
                  </TableRow>
                ))
              )}
            </TableBody>
          </Table>
        </CardContent>
      </Card>

      {/* 模态窗口 */}
      <DonationFormModal
        open={showDonationModal}
        onClose={() => setShowDonationModal(false)}
        onSuccess={loadData}
      />

      <FundingFormModal
        open={showFundingModal}
        onClose={() => setShowFundingModal(false)}
        onSuccess={loadData}
      />
    </div>
  );
}
