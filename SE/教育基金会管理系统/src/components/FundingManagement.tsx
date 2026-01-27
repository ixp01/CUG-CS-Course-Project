import { useEffect, useState } from 'react';
import { Card, CardContent, CardHeader, CardTitle } from './ui/card';
import { Button } from './ui/button';
import { Badge } from './ui/badge';
import { Input } from './ui/input';
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from './ui/select';
import { Table, TableBody, TableCell, TableHead, TableHeader, TableRow } from './ui/table';
import { Plus, Search, Trash2, CheckCircle } from 'lucide-react';
import { getFundings, updateFunding, deleteFunding, calculateMonthlyFinancials } from '../utils/storage';
import { FundingApplication } from '../types';
import { FundingFormModal } from './FundingFormModal';
import { ReviewModal } from './ReviewModal';
import { toast } from 'sonner@2.0.3';

export function FundingManagement() {
  const [fundings, setFundings] = useState<FundingApplication[]>([]);
  const [filteredFundings, setFilteredFundings] = useState<FundingApplication[]>([]);
  const [statusFilter, setStatusFilter] = useState<string>('all');
  const [searchKeyword, setSearchKeyword] = useState('');
  const [showFormModal, setShowFormModal] = useState(false);
  const [showReviewModal, setShowReviewModal] = useState(false);
  const [selectedFunding, setSelectedFunding] = useState<FundingApplication | null>(null);

  useEffect(() => {
    loadFundings();
  }, []);

  useEffect(() => {
    filterFundings();
  }, [fundings, statusFilter, searchKeyword]);

  const loadFundings = () => {
    const data = getFundings();
    setFundings(data);
  };

  const filterFundings = () => {
    let filtered = [...fundings];

    // 状态筛选
    if (statusFilter !== 'all') {
      filtered = filtered.filter(f => f.status === statusFilter);
    }

    // 关键词搜索
    if (searchKeyword) {
      const keyword = searchKeyword.toLowerCase();
      filtered = filtered.filter(f =>
        f.institutionName.toLowerCase().includes(keyword) ||
        f.purpose.toLowerCase().includes(keyword) ||
        f.id.toLowerCase().includes(keyword)
      );
    }

    setFilteredFundings(filtered);
  };

  const handleApprove = (id: string, opinion: string) => {
    const today = new Date().toISOString().split('T')[0];
    updateFunding(id, { status: 'approved', approvalDate: today });
    calculateMonthlyFinancials();
    toast.success('用款申请已通过！');
    loadFundings();
  };

  const handleReject = (id: string, opinion: string) => {
    updateFunding(id, { status: 'rejected' });
    toast.error('用款申请已拒绝');
    loadFundings();
  };

  const handleDelete = (id: string) => {
    if (window.confirm('确定要删除这条用款申请吗？')) {
      deleteFunding(id);
      calculateMonthlyFinancials();
      toast.success('删除成功');
      loadFundings();
    }
  };

  const handleComplete = (id: string) => {
    updateFunding(id, { status: 'completed' });
    calculateMonthlyFinancials();
    toast.success('用款已标记为完成！');
    loadFundings();
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

  const handleReview = (funding: FundingApplication) => {
    setSelectedFunding(funding);
    setShowReviewModal(true);
  };

  return (
    <div className="max-w-7xl mx-auto space-y-6">
      {/* 页面标题 */}
      <div className="flex items-center justify-between">
        <div>
          <h1 className="text-3xl text-gray-900">用款申请</h1>
          <p className="text-gray-600 mt-1">管理所有用款申请和审批</p>
        </div>
        <Button onClick={() => setShowFormModal(true)} className="flex items-center gap-2">
          <Plus className="w-4 h-4" />
          新增用款申请
        </Button>
      </div>

      {/* 筛选区域 */}
      <Card>
        <CardContent className="pt-6">
          <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
            <Select value={statusFilter} onValueChange={setStatusFilter}>
              <SelectTrigger>
                <SelectValue placeholder="状态筛选" />
              </SelectTrigger>
              <SelectContent>
                <SelectItem value="all">全部状态</SelectItem>
                <SelectItem value="pending">待审核</SelectItem>
                <SelectItem value="approved">已通过</SelectItem>
                <SelectItem value="rejected">已拒绝</SelectItem>
                <SelectItem value="completed">已完成</SelectItem>
              </SelectContent>
            </Select>

            <div className="relative col-span-2">
              <Search className="absolute left-3 top-1/2 -translate-y-1/2 w-5 h-5 text-gray-400" />
              <Input
                placeholder="搜索申请单位、用途或申请编号..."
                className="pl-10"
                value={searchKeyword}
                onChange={(e) => setSearchKeyword(e.target.value)}
              />
            </div>
          </div>
        </CardContent>
      </Card>

      {/* 用款申请表格 */}
      <Card>
        <CardHeader>
          <CardTitle>用款申请列表（共 {filteredFundings.length} 条）</CardTitle>
        </CardHeader>
        <CardContent>
          <div className="overflow-x-auto">
            <Table>
              <TableHeader>
                <TableRow>
                  <TableHead>申请编号</TableHead>
                  <TableHead>申请单位</TableHead>
                  <TableHead>金额</TableHead>
                  <TableHead>申请日期</TableHead>
                  <TableHead>用途</TableHead>
                  <TableHead>状态</TableHead>
                  <TableHead>操作</TableHead>
                </TableRow>
              </TableHeader>
              <TableBody>
                {filteredFundings.length === 0 ? (
                  <TableRow>
                    <TableCell colSpan={7} className="text-center text-gray-500 py-8">
                      暂无数据
                    </TableCell>
                  </TableRow>
                ) : (
                  filteredFundings.map((funding) => (
                    <TableRow key={funding.id}>
                      <TableCell className="text-sm">{funding.id}</TableCell>
                      <TableCell>{funding.institutionName}</TableCell>
                      <TableCell>{formatCurrency(funding.amount)}</TableCell>
                      <TableCell>{formatDate(funding.applicationDate)}</TableCell>
                      <TableCell className="max-w-[200px] truncate">
                        {funding.purpose}
                      </TableCell>
                      <TableCell>{getStatusBadge(funding.status)}</TableCell>
                      <TableCell>
                        <div className="flex gap-2">
                          {funding.status === 'pending' && (
                            <Button
                              size="sm"
                              variant="outline"
                              onClick={() => handleReview(funding)}
                            >
                              审批
                            </Button>
                          )}
                          {funding.status === 'approved' && (
                            <Button
                              size="sm"
                              variant="outline"
                              onClick={() => handleComplete(funding.id)}
                            >
                              <CheckCircle className="w-4 h-4 mr-1" />
                              完成
                            </Button>
                          )}
                          <Button
                            size="sm"
                            variant="ghost"
                            onClick={() => handleDelete(funding.id)}
                          >
                            <Trash2 className="w-4 h-4" />
                          </Button>
                        </div>
                      </TableCell>
                    </TableRow>
                  ))
                )}
              </TableBody>
            </Table>
          </div>
        </CardContent>
      </Card>

      {/* 表单模态窗口 */}
      <FundingFormModal
        open={showFormModal}
        onClose={() => setShowFormModal(false)}
        onSuccess={loadFundings}
      />

      {/* 审批模态窗口 */}
      <ReviewModal
        open={showReviewModal}
        onClose={() => {
          setShowReviewModal(false);
          setSelectedFunding(null);
        }}
        application={selectedFunding}
        type="funding"
        onApprove={handleApprove}
        onReject={handleReject}
      />
    </div>
  );
}
