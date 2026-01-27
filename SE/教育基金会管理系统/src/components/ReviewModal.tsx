import { useState } from 'react';
import { Dialog, DialogContent, DialogHeader, DialogTitle } from './ui/dialog';
import { Button } from './ui/button';
import { Label } from './ui/label';
import { Textarea } from './ui/textarea';
import { CheckCircle, XCircle } from 'lucide-react';
import { DonationApplication, FundingApplication } from '../types';

interface ReviewModalProps {
  open: boolean;
  onClose: () => void;
  application: DonationApplication | FundingApplication | null;
  type: 'donation' | 'funding';
  onApprove: (id: string, opinion: string) => void;
  onReject: (id: string, opinion: string) => void;
}

export function ReviewModal({ open, onClose, application, type, onApprove, onReject }: ReviewModalProps) {
  const [opinion, setOpinion] = useState('');

  const handleClose = () => {
    setOpinion('');
    onClose();
  };

  const handleApprove = () => {
    if (!application) return;
    onApprove(application.id, opinion);
    handleClose();
  };

  const handleReject = () => {
    if (!application) return;
    if (!opinion.trim()) {
      alert('请输入拒绝理由');
      return;
    }
    onReject(application.id, opinion);
    handleClose();
  };

  if (!application) return null;

  const isDonation = type === 'donation';
  const donationApp = isDonation ? (application as DonationApplication) : null;
  const fundingApp = !isDonation ? (application as FundingApplication) : null;

  return (
    <Dialog open={open} onOpenChange={handleClose}>
      <DialogContent className="max-w-2xl max-h-[90vh] overflow-y-auto">
        <DialogHeader>
          <DialogTitle>{isDonation ? '捐助申请审核' : '用款申请审批'}</DialogTitle>
        </DialogHeader>

        <div className="space-y-4">
          {/* 申请信息预览 */}
          <div className="bg-gray-50 rounded-lg p-4 space-y-3">
            <h3 className="text-sm text-gray-500">申请信息</h3>
            
            {isDonation && donationApp ? (
              <div className="grid grid-cols-2 gap-3 text-sm">
                <div>
                  <span className="text-gray-600">申请人：</span>
                  <span className="ml-2">{donationApp.donorName}</span>
                </div>
                <div>
                  <span className="text-gray-600">类型：</span>
                  <span className="ml-2">{donationApp.donorType === 'enterprise' ? '企业' : '个人'}</span>
                </div>
                <div>
                  <span className="text-gray-600">联系电话：</span>
                  <span className="ml-2">{donationApp.contactPhone}</span>
                </div>
                <div>
                  <span className="text-gray-600">证件号：</span>
                  <span className="ml-2">{donationApp.idNumber}</span>
                </div>
                <div>
                  <span className="text-gray-600">捐助金额：</span>
                  <span className="ml-2">¥{donationApp.amount.toLocaleString()}</span>
                </div>
                <div>
                  <span className="text-gray-600">申请日期：</span>
                  <span className="ml-2">{donationApp.applicationDate}</span>
                </div>
                <div className="col-span-2">
                  <span className="text-gray-600">指定用途：</span>
                  <span className="ml-2">{donationApp.purpose}</span>
                </div>
                {donationApp.notes && (
                  <div className="col-span-2">
                    <span className="text-gray-600">备注：</span>
                    <span className="ml-2">{donationApp.notes}</span>
                  </div>
                )}
              </div>
            ) : fundingApp ? (
              <div className="space-y-3">
                <div className="grid grid-cols-2 gap-3 text-sm">
                  <div>
                    <span className="text-gray-600">申请单位：</span>
                    <span className="ml-2">{fundingApp.institutionName}</span>
                  </div>
                  <div>
                    <span className="text-gray-600">申请金额：</span>
                    <span className="ml-2">¥{fundingApp.amount.toLocaleString()}</span>
                  </div>
                  <div>
                    <span className="text-gray-600">用途分类：</span>
                    <span className="ml-2">{fundingApp.purposeCategory}</span>
                  </div>
                  <div>
                    <span className="text-gray-600">申请日期：</span>
                    <span className="ml-2">{fundingApp.applicationDate}</span>
                  </div>
                  <div className="col-span-2">
                    <span className="text-gray-600">用款用途：</span>
                    <span className="ml-2">{fundingApp.purpose}</span>
                  </div>
                  <div className="col-span-2">
                    <span className="text-gray-600">详细说明：</span>
                    <span className="ml-2">{fundingApp.description}</span>
                  </div>
                </div>

                {/* 预算明细 */}
                {fundingApp.budgetDetails.length > 0 && (
                  <div>
                    <div className="text-sm text-gray-600 mb-2">预算明细：</div>
                    <div className="space-y-1">
                      {fundingApp.budgetDetails.map((budget) => (
                        <div key={budget.id} className="flex justify-between text-sm bg-white p-2 rounded">
                          <span>{budget.item}</span>
                          <span>¥{budget.amount.toLocaleString()}</span>
                        </div>
                      ))}
                    </div>
                  </div>
                )}
              </div>
            ) : null}
          </div>

          {/* 审核意见 */}
          <div className="space-y-2">
            <Label htmlFor="opinion">审核意见</Label>
            <Textarea
              id="opinion"
              value={opinion}
              onChange={(e) => setOpinion(e.target.value)}
              placeholder="请输入审核意见（拒绝时必填）"
              rows={4}
            />
          </div>

          {/* 操作按钮 */}
          <div className="flex gap-3 pt-4">
            <Button
              type="button"
              onClick={handleApprove}
              className="flex-1 bg-green-600 hover:bg-green-700"
            >
              <CheckCircle className="w-4 h-4 mr-2" />
              通过
            </Button>
            <Button
              type="button"
              onClick={handleReject}
              variant="destructive"
              className="flex-1"
            >
              <XCircle className="w-4 h-4 mr-2" />
              拒绝
            </Button>
            <Button type="button" variant="outline" onClick={handleClose}>
              取消
            </Button>
          </div>
        </div>
      </DialogContent>
    </Dialog>
  );
}
