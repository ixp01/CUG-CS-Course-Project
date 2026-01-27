import { useEffect, useState } from 'react';
import { Card, CardContent, CardHeader, CardTitle } from './ui/card';
import { Button } from './ui/button';
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from './ui/select';
import { Table, TableBody, TableCell, TableHead, TableHeader, TableRow } from './ui/table';
import { Download, FileSpreadsheet, FileText, TrendingUp, TrendingDown, Wallet } from 'lucide-react';
import { getFinancials, calculateMonthlyFinancials } from '../utils/storage';
import { MonthlyFinancial } from '../types';
import { LineChart, Line, BarChart, Bar, PieChart, Pie, Cell, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';
import { exportToExcel, exportToTxt } from '../utils/exportUtils';
import { toast } from 'sonner@2.0.3';

export function FinancialReport() {
  const [financials, setFinancials] = useState<MonthlyFinancial[]>([]);
  const [selectedMonth, setSelectedMonth] = useState<string>('');
  const [currentMonthData, setCurrentMonthData] = useState<MonthlyFinancial | null>(null);

  useEffect(() => {
    loadFinancials();
  }, []);

  useEffect(() => {
    if (selectedMonth && financials.length > 0) {
      const monthData = financials.find(f => f.month === selectedMonth);
      setCurrentMonthData(monthData || null);
    }
  }, [selectedMonth, financials]);

  const loadFinancials = () => {
    calculateMonthlyFinancials();
    const data = getFinancials().sort((a, b) => a.month.localeCompare(b.month));
    setFinancials(data);
    
    if (data.length > 0) {
      // 默认选择最新月份
      setSelectedMonth(data[data.length - 1].month);
    }
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

  // 准备图表数据
  const trendData = financials.map(f => ({
    month: f.month,
    income: f.income,
    expense: f.expense,
    balance: f.balance,
  }));

  // 收入来源分布（企业 vs 个人）
  const incomeSourceData = currentMonthData ? [
    { name: '企业捐助', value: currentMonthData.incomeDetails.filter(d => d.category === '企业捐助').reduce((sum, d) => sum + d.amount, 0) },
    { name: '个人捐助', value: currentMonthData.incomeDetails.filter(d => d.category === '个人捐助').reduce((sum, d) => sum + d.amount, 0) },
  ].filter(d => d.value > 0) : [];

  // 支出用途分布
  const expenseCategoryData = currentMonthData ? 
    Object.entries(
      currentMonthData.expenseDetails.reduce((acc, d) => {
        acc[d.category] = (acc[d.category] || 0) + d.amount;
        return acc;
      }, {} as Record<string, number>)
    ).map(([name, value]) => ({ name, value }))
    : [];

  const COLORS = ['#3b82f6', '#10b981', '#f59e0b', '#ef4444', '#8b5cf6', '#ec4899'];

  const availableMonths = financials.map(f => ({
    value: f.month,
    label: new Date(f.month + '-01').toLocaleDateString('zh-CN', { year: 'numeric', month: 'long' })
  }));

  const handleExportExcel = () => {
    if (!currentMonthData) {
      toast.error('请先选择要导出的月份');
      return;
    }
    exportToExcel(currentMonthData, selectedMonth);
    toast.success('Excel文件导出成功！');
  };

  const handleExportTxt = () => {
    if (!currentMonthData) {
      toast.error('请先选择要导出的月份');
      return;
    }
    exportToTxt(currentMonthData, selectedMonth);
    toast.success('TXT文本导出成功！');
  };

  return (
    <div className="max-w-7xl mx-auto space-y-6">
      {/* 页面标题 */}
      <div className="flex items-center justify-between">
        <div>
          <h1 className="text-3xl text-gray-900">财务报表</h1>
          <p className="text-gray-600 mt-1">查看月度收支情况和财务分析</p>
        </div>
      </div>

      {/* 报表控制区 */}
      <Card>
        <CardContent className="pt-6">
          <div className="flex items-center gap-4">
            <div className="flex-1">
              <Select value={selectedMonth} onValueChange={setSelectedMonth}>
                <SelectTrigger>
                  <SelectValue placeholder="选择月份" />
                </SelectTrigger>
                <SelectContent>
                  {availableMonths.map(month => (
                    <SelectItem key={month.value} value={month.value}>
                      {month.label}
                    </SelectItem>
                  ))}
                </SelectContent>
              </Select>
            </div>
            <Button 
              variant="outline" 
              className="flex items-center gap-2"
              onClick={handleExportExcel}
              disabled={!currentMonthData}
            >
              <FileSpreadsheet className="w-4 h-4" />
              导出Excel
            </Button>
            <Button 
              variant="outline" 
              className="flex items-center gap-2"
              onClick={handleExportTxt}
              disabled={!currentMonthData}
            >
              <FileText className="w-4 h-4" />
              导出TXT
            </Button>
          </div>
        </CardContent>
      </Card>

      {currentMonthData ? (
        <>
          {/* 关键指标卡片 */}
          <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
            <Card>
              <CardHeader className="flex flex-row items-center justify-between pb-2">
                <CardTitle className="text-sm text-gray-600">本月收入</CardTitle>
                <div className="w-10 h-10 bg-green-100 rounded-lg flex items-center justify-center">
                  <TrendingUp className="w-5 h-5 text-green-600" />
                </div>
              </CardHeader>
              <CardContent>
                <div className="text-3xl text-gray-900 mb-1">
                  {formatCurrency(currentMonthData.income)}
                </div>
                <p className="text-sm text-gray-500">
                  {currentMonthData.incomeDetails.length} 笔收入
                </p>
              </CardContent>
            </Card>

            <Card>
              <CardHeader className="flex flex-row items-center justify-between pb-2">
                <CardTitle className="text-sm text-gray-600">本月支出</CardTitle>
                <div className="w-10 h-10 bg-red-100 rounded-lg flex items-center justify-center">
                  <TrendingDown className="w-5 h-5 text-red-600" />
                </div>
              </CardHeader>
              <CardContent>
                <div className="text-3xl text-gray-900 mb-1">
                  {formatCurrency(currentMonthData.expense)}
                </div>
                <p className="text-sm text-gray-500">
                  {currentMonthData.expenseDetails.length} 笔支出
                </p>
              </CardContent>
            </Card>

            <Card>
              <CardHeader className="flex flex-row items-center justify-between pb-2">
                <CardTitle className="text-sm text-gray-600">月末余额</CardTitle>
                <div className="w-10 h-10 bg-blue-100 rounded-lg flex items-center justify-center">
                  <Wallet className="w-5 h-5 text-blue-600" />
                </div>
              </CardHeader>
              <CardContent>
                <div className="text-3xl text-gray-900 mb-1">
                  {formatCurrency(currentMonthData.balance)}
                </div>
                <p className="text-sm text-gray-500">
                  净增加 {formatCurrency(currentMonthData.income - currentMonthData.expense)}
                </p>
              </CardContent>
            </Card>
          </div>

          {/* 图表区域 */}
          <div className="grid grid-cols-1 lg:grid-cols-5 gap-6">
            {/* 收入支出趋势图 */}
            <Card className="lg:col-span-3">
              <CardHeader>
                <CardTitle>收入支出趋势</CardTitle>
              </CardHeader>
              <CardContent>
                <ResponsiveContainer width="100%" height={300}>
                  <LineChart data={trendData}>
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis dataKey="month" />
                    <YAxis />
                    <Tooltip formatter={(value) => formatCurrency(value as number)} />
                    <Legend />
                    <Line type="monotone" dataKey="income" stroke="#10b981" name="收入" strokeWidth={2} />
                    <Line type="monotone" dataKey="expense" stroke="#ef4444" name="支出" strokeWidth={2} />
                  </LineChart>
                </ResponsiveContainer>
              </CardContent>
            </Card>

            {/* 收入来源分布 */}
            <Card className="lg:col-span-2">
              <CardHeader>
                <CardTitle>收入来源分布</CardTitle>
              </CardHeader>
              <CardContent>
                {incomeSourceData.length > 0 ? (
                  <ResponsiveContainer width="100%" height={300}>
                    <PieChart>
                      <Pie
                        data={incomeSourceData}
                        cx="50%"
                        cy="50%"
                        labelLine={false}
                        label={({ name, percent }) => `${name} ${(percent * 100).toFixed(0)}%`}
                        outerRadius={80}
                        fill="#8884d8"
                        dataKey="value"
                      >
                        {incomeSourceData.map((entry, index) => (
                          <Cell key={`cell-${index}`} fill={COLORS[index % COLORS.length]} />
                        ))}
                      </Pie>
                      <Tooltip formatter={(value) => formatCurrency(value as number)} />
                    </PieChart>
                  </ResponsiveContainer>
                ) : (
                  <div className="h-[300px] flex items-center justify-center text-gray-500">
                    暂无数据
                  </div>
                )}
              </CardContent>
            </Card>
          </div>

          {/* 支出用途分布 */}
          {expenseCategoryData.length > 0 && (
            <Card>
              <CardHeader>
                <CardTitle>支出用途分布</CardTitle>
              </CardHeader>
              <CardContent>
                <ResponsiveContainer width="100%" height={300}>
                  <BarChart data={expenseCategoryData}>
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis dataKey="name" />
                    <YAxis />
                    <Tooltip formatter={(value) => formatCurrency(value as number)} />
                    <Bar dataKey="value" fill="#3b82f6" name="金额" />
                  </BarChart>
                </ResponsiveContainer>
              </CardContent>
            </Card>
          )}

          {/* 收入明细 */}
          <Card>
            <CardHeader>
              <CardTitle>收入明细</CardTitle>
            </CardHeader>
            <CardContent>
              <Table>
                <TableHeader>
                  <TableRow>
                    <TableHead>日期</TableHead>
                    <TableHead>描述</TableHead>
                    <TableHead>类别</TableHead>
                    <TableHead className="text-right">金额</TableHead>
                  </TableRow>
                </TableHeader>
                <TableBody>
                  {currentMonthData.incomeDetails.length === 0 ? (
                    <TableRow>
                      <TableCell colSpan={4} className="text-center text-gray-500 py-8">
                        暂无收入记录
                      </TableCell>
                    </TableRow>
                  ) : (
                    currentMonthData.incomeDetails.map((detail) => (
                      <TableRow key={detail.id}>
                        <TableCell>{formatDate(detail.date)}</TableCell>
                        <TableCell>{detail.description}</TableCell>
                        <TableCell>{detail.category}</TableCell>
                        <TableCell className="text-right text-green-600">
                          +{formatCurrency(detail.amount)}
                        </TableCell>
                      </TableRow>
                    ))
                  )}
                </TableBody>
              </Table>
            </CardContent>
          </Card>

          {/* 支出明细 */}
          <Card>
            <CardHeader>
              <CardTitle>支出明细</CardTitle>
            </CardHeader>
            <CardContent>
              <Table>
                <TableHeader>
                  <TableRow>
                    <TableHead>日期</TableHead>
                    <TableHead>描述</TableHead>
                    <TableHead>类别</TableHead>
                    <TableHead className="text-right">金额</TableHead>
                  </TableRow>
                </TableHeader>
                <TableBody>
                  {currentMonthData.expenseDetails.length === 0 ? (
                    <TableRow>
                      <TableCell colSpan={4} className="text-center text-gray-500 py-8">
                        暂无支出记录
                      </TableCell>
                    </TableRow>
                  ) : (
                    currentMonthData.expenseDetails.map((detail) => (
                      <TableRow key={detail.id}>
                        <TableCell>{formatDate(detail.date)}</TableCell>
                        <TableCell>{detail.description}</TableCell>
                        <TableCell>{detail.category}</TableCell>
                        <TableCell className="text-right text-red-600">
                          -{formatCurrency(detail.amount)}
                        </TableCell>
                      </TableRow>
                    ))
                  )}
                </TableBody>
              </Table>
            </CardContent>
          </Card>
        </>
      ) : (
        <Card>
          <CardContent className="py-12 text-center text-gray-500">
            <p>暂无财务数据</p>
            <p className="text-sm mt-2">请先添加捐助或用款申请</p>
          </CardContent>
        </Card>
      )}
    </div>
  );
}
