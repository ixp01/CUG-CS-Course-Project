import { useState } from 'react';
import { Dialog, DialogContent, DialogHeader, DialogTitle } from './ui/dialog';
import { Button } from './ui/button';
import { Input } from './ui/input';
import { Label } from './ui/label';
import { Textarea } from './ui/textarea';
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from './ui/select';
import { addDonation } from '../utils/storage';
import { DonorType } from '../types';
import { toast } from 'sonner@2.0.3';

interface DonationFormModalProps {
  open: boolean;
  onClose: () => void;
  onSuccess: () => void;
}

export function DonationFormModal({ open, onClose, onSuccess }: DonationFormModalProps) {
  const [donorType, setDonorType] = useState<DonorType>('individual');
  const [donorName, setDonorName] = useState('');
  const [contactPhone, setContactPhone] = useState('');
  const [idNumber, setIdNumber] = useState('');
  const [amount, setAmount] = useState('');
  const [purpose, setPurpose] = useState('');
  const [notes, setNotes] = useState('');

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();

    if (!donorName || !contactPhone || !idNumber || !amount || !purpose) {
      toast.error('请填写所有必填字段');
      return;
    }

    const amountNum = parseFloat(amount);
    if (isNaN(amountNum) || amountNum <= 0) {
      toast.error('请输入有效的金额');
      return;
    }

    const donation = {
      id: `DON-${Date.now()}`,
      donorType,
      donorName,
      contactPhone,
      idNumber,
      amount: amountNum,
      purpose,
      notes,
      status: 'pending' as const,
      applicationDate: new Date().toISOString().split('T')[0],
    };

    addDonation(donation);
    toast.success('捐助申请提交成功！');
    handleClose();
    onSuccess();
  };

  const handleClose = () => {
    setDonorType('individual');
    setDonorName('');
    setContactPhone('');
    setIdNumber('');
    setAmount('');
    setPurpose('');
    setNotes('');
    onClose();
  };

  return (
    <Dialog open={open} onOpenChange={handleClose}>
      <DialogContent className="max-w-2xl max-h-[90vh] overflow-y-auto">
        <DialogHeader>
          <DialogTitle>新增捐助申请</DialogTitle>
        </DialogHeader>

        <form onSubmit={handleSubmit} className="space-y-4">
          <div className="space-y-2">
            <Label htmlFor="donorType">捐助人类型 *</Label>
            <Select value={donorType} onValueChange={(value) => setDonorType(value as DonorType)}>
              <SelectTrigger>
                <SelectValue />
              </SelectTrigger>
              <SelectContent>
                <SelectItem value="individual">个人</SelectItem>
                <SelectItem value="enterprise">企业</SelectItem>
              </SelectContent>
            </Select>
          </div>

          <div className="grid grid-cols-2 gap-4">
            <div className="space-y-2">
              <Label htmlFor="donorName">{donorType === 'enterprise' ? '企业名称' : '姓名'} *</Label>
              <Input
                id="donorName"
                value={donorName}
                onChange={(e) => setDonorName(e.target.value)}
                placeholder={donorType === 'enterprise' ? '请输入企业名称' : '请输入姓名'}
              />
            </div>

            <div className="space-y-2">
              <Label htmlFor="contactPhone">联系电话 *</Label>
              <Input
                id="contactPhone"
                type="tel"
                value={contactPhone}
                onChange={(e) => setContactPhone(e.target.value)}
                placeholder="请输入联系电话"
              />
            </div>
          </div>

          <div className="space-y-2">
            <Label htmlFor="idNumber">
              {donorType === 'enterprise' ? '统一社会信用代码' : '身份证号'} *
            </Label>
            <Input
              id="idNumber"
              value={idNumber}
              onChange={(e) => setIdNumber(e.target.value)}
              placeholder={donorType === 'enterprise' ? '请输入统一社会信用代码' : '请输入身份证号'}
            />
          </div>

          <div className="grid grid-cols-2 gap-4">
            <div className="space-y-2">
              <Label htmlFor="amount">捐助金额（元） *</Label>
              <Input
                id="amount"
                type="number"
                value={amount}
                onChange={(e) => setAmount(e.target.value)}
                placeholder="请输入捐助金额"
                min="0"
                step="0.01"
              />
            </div>

            <div className="space-y-2">
              <Label htmlFor="purpose">指定用途 *</Label>
              <Input
                id="purpose"
                value={purpose}
                onChange={(e) => setPurpose(e.target.value)}
                placeholder="如：教育设备采购"
              />
            </div>
          </div>

          <div className="space-y-2">
            <Label htmlFor="notes">备注说明</Label>
            <Textarea
              id="notes"
              value={notes}
              onChange={(e) => setNotes(e.target.value)}
              placeholder="请输入备注说明（选填）"
              rows={4}
            />
          </div>

          <div className="flex gap-3 pt-4">
            <Button type="submit" className="flex-1">
              提交申请
            </Button>
            <Button type="button" variant="outline" onClick={handleClose} className="flex-1">
              取消
            </Button>
          </div>
        </form>
      </DialogContent>
    </Dialog>
  );
}
