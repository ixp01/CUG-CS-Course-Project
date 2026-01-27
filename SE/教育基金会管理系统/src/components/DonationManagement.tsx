import { useEffect, useState } from 'react';
import { Card, CardContent, CardHeader, CardTitle } from './ui/card';
import { Button } from './ui/button';
import { Badge } from './ui/badge';
import { Input } from './ui/input';
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from './ui/select';
import { Table, TableBody, TableCell, TableHead, TableHeader, TableRow } from './ui/table';
import { Plus, Search, Eye, Trash2, CheckCircle } from 'lucide-react';
import { getDonations, updateDonation, deleteDonation, calculateMonthlyFinancials } from '../utils/storage';
import { DonationApplication } from '../types';
import { DonationFormModal } from './DonationFormModal';
import { ReviewModal } from './ReviewModal';
import { toast } from 'sonner@2.0.3';

export function DonationManagement() {
  const [donations, setDonations] = useState<DonationApplication[]>([]);
  const [filteredDonations, setFilteredDonations] = useState<DonationApplication[]>([]);
  const [statusFilter, setStatusFilter] = useState<string>('all');
  const [searchKeyword, setSearchKeyword] = useState('');
  const [showFormModal, setShowFormModal] = useState(false);
  const [showReviewModal, setShowReviewModal] = useState(false);
  const [selectedDonation, setSelectedDonation] = useState<DonationApplication | null>(null);

  useEffect(() => {
    loadDonations();
  }, []);

  useEffect(() => {
    filterDonations();
  }, [donations, statusFilter, searchKeyword]);

  const loadDonations = () => {
    const data = getDonations();
    setDonations(data);
  };

  const filterDonations = () => {
    let filtered = [...donations];

    // 状态筛选
    if (statusFilter !== 'all') {
      filtered = filtered.filter(d => d.status === statusFilter);
    }

    // 关键词搜索
    if (searchKeyword) {
      const keyword = searchKeyword.toLowerCase();
      filtered = filtered.filter(d =>
        d.donorName.toLowerCase().includes(keyword) ||
        d.purpose.toLowerCase().includes(keyword) ||
        d.id.toLowerCase().includes(keyword)
      );
    }

    setFilteredDonations(filtered);
  };

  const handleApprove = (id: string, opinion: string) => {
    updateDonation(id, { status: 'approved' });
    calculateMonthlyFinancials();
    toast.success('捐助申请已通过！');
    loadDonations();
  };

  const handleReject = (id: string, opinion: string) => {
    updateDonation(id, { status: 'rejected' });
    toast.error('捐助申请已拒绝');
    loadDonations();
  };

  const handleDelete = (id: string) => {
    if (window.confirm('确定要删除这条捐助申请吗？')) {
      deleteDonation(id);
      calculateMonthlyFinancials();
      toast.success('删除成功');
      loadDonations();
    }
  };

  const handleComplete = (id: string) => {
    updateDonation(id, { status: 'completed' });
    calculateMonthlyFinancials();
    toast.success('捐助已标记为完成！');
    loadDonations();
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

  const handleReview = (donation: DonationApplication) => {
    setSelectedDonation(donation);
    setShowReviewModal(true);
  };

  return (
    <div className="max-w-7xl mx-auto space-y-6">
      {/* 页面标题 */}
      <div className="flex items-center justify-between">
        <div>
          <h1 className="text-3xl text-gray-900">捐助管理</h1>
          <p className="text-gray-600 mt-1">管理所有捐助申请和记录</p>
        </div>
        <Button onClick={() => setShowFormModal(true)} className="flex items-center gap-2">
          <Plus className="w-4 h-4" />
          新增捐助申请
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
                placeholder="搜索申请人、用途或申请编号..."
                className="pl-10"
                value={searchKeyword}
                onChange={(e) => setSearchKeyword(e.target.value)}
              />
            </div>
          </div>
        </CardContent>
      </Card>

      {/* 捐助申请表格 */}
      <Card>
        <CardHeader>
          <CardTitle>捐助申请列表（共 {filteredDonations.length} 条）</CardTitle>
        </CardHeader>
        <CardContent>
          <div className="overflow-x-auto">
            <Table>
              <TableHeader>
                <TableRow>
                  <TableHead>申请编号</TableHead>
                  <TableHead>申请人</TableHead>
                  <TableHead>类型</TableHead>
                  <TableHead>金额</TableHead>
                  <TableHead>申请日期</TableHead>
                  <TableHead>状态</TableHead>
                  <TableHead>操作</TableHead>
                </TableRow>
              </TableHeader>
              <TableBody>
                {filteredDonations.length === 0 ? (
                  <TableRow>
                    <TableCell colSpan={7} className="text-center text-gray-500 py-8">
                      暂无数据
                    </TableCell>
                  </TableRow>
                ) : (
                  filteredDonations.map((donation) => (
                    <TableRow key={donation.id}>
                      <TableCell className="text-sm">{donation.id}</TableCell>
                      <TableCell>{donation.donorName}</TableCell>
                      <TableCell>
                        {donation.donorType === 'enterprise' ? '企业' : '个人'}
                      </TableCell>
                      <TableCell>{formatCurrency(donation.amount)}</TableCell>
                      <TableCell>{formatDate(donation.applicationDate)}</TableCell>
                      <TableCell>{getStatusBadge(donation.status)}</TableCell>
                      <TableCell>
                        <div className="flex gap-2">
                          {donation.status === 'pending' && (
                            <Button
                              size="sm"
                              variant="outline"
                              onClick={() => handleReview(donation)}
                            >
                              审核
                            </Button>
                          )}
                          {donation.status === 'approved' && (
                            <Button
                              size="sm"
                              variant="outline"
                              onClick={() => handleComplete(donation.id)}
                            >
                              <CheckCircle className="w-4 h-4 mr-1" />
                              完成
                            </Button>
                          )}
                          <Button
                            size="sm"
                            variant="ghost"
                            onClick={() => handleDelete(donation.id)}
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
      <DonationFormModal
        open={showFormModal}
        onClose={() => setShowFormModal(false)}
        onSuccess={loadDonations}
      />

      {/* 审核模态窗口 */}
      <ReviewModal
        open={showReviewModal}
        onClose={() => {
          setShowReviewModal(false);
          setSelectedDonation(null);
        }}
        application={selectedDonation}
        type="donation"
        onApprove={handleApprove}
        onReject={handleReject}
      />
    </div>
  );
}
