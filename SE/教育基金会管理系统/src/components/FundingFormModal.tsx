import { useState } from 'react';
import { Dialog, DialogContent, DialogHeader, DialogTitle } from './ui/dialog';
import { Button } from './ui/button';
import { Input } from './ui/input';
import { Label } from './ui/label';
import { Textarea } from './ui/textarea';
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from './ui/select';
import { Plus, X } from 'lucide-react';
import { addFunding } from '../utils/storage';
import { BudgetItem } from '../types';
import { toast } from 'sonner@2.0.3';

interface FundingFormModalProps {
  open: boolean;
  onClose: () => void;
  onSuccess: () => void;
}

export function FundingFormModal({ open, onClose, onSuccess }: FundingFormModalProps) {
  const [institutionName, setInstitutionName] = useState('');
  const [amount, setAmount] = useState('');
  const [purpose, setPurpose] = useState('');
  const [purposeCategory, setPurposeCategory] = useState('');
  const [description, setDescription] = useState('');
  const [budgetDetails, setBudgetDetails] = useState<BudgetItem[]>([
    { id: '1', item: '', amount: 0, description: '' }
  ]);

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();

    if (!institutionName || !amount || !purpose || !purposeCategory || !description) {
      toast.error('请填写所有必填字段');
      return;
    }

    const amountNum = parseFloat(amount);
    if (isNaN(amountNum) || amountNum <= 0) {
      toast.error('请输入有效的金额');
      return;
    }

    // 验证预算明细
    const validBudgetDetails = budgetDetails.filter(b => b.item && b.amount > 0);
    if (validBudgetDetails.length === 0) {
      toast.error('请至少添加一项预算明细');
      return;
    }

    const totalBudget = validBudgetDetails.reduce((sum, b) => sum + b.amount, 0);
    if (Math.abs(totalBudget - amountNum) > 0.01) {
      toast.error(`预算明细总额（¥${totalBudget}）与申请金额（¥${amountNum}）不一致`);
      return;
    }

    const funding = {
      id: `FUND-${Date.now()}`,
      institutionName,
      amount: amountNum,
      purpose,
      purposeCategory,
      description,
      budgetDetails: validBudgetDetails,
      status: 'pending' as const,
      applicationDate: new Date().toISOString().split('T')[0],
    };

    addFunding(funding);
    toast.success('用款申请提交成功！');
    handleClose();
    onSuccess();
  };

  const handleClose = () => {
    setInstitutionName('');
    setAmount('');
    setPurpose('');
    setPurposeCategory('');
    setDescription('');
    setBudgetDetails([{ id: '1', item: '', amount: 0, description: '' }]);
    onClose();
  };

  const addBudgetItem = () => {
    setBudgetDetails([
      ...budgetDetails,
      { id: Date.now().toString(), item: '', amount: 0, description: '' }
    ]);
  };

  const removeBudgetItem = (id: string) => {
    if (budgetDetails.length > 1) {
      setBudgetDetails(budgetDetails.filter(b => b.id !== id));
    }
  };

  const updateBudgetItem = (id: string, field: keyof BudgetItem, value: any) => {
    setBudgetDetails(budgetDetails.map(b =>
      b.id === id ? { ...b, [field]: value } : b
    ));
  };

  const totalBudget = budgetDetails.reduce((sum, b) => sum + (b.amount || 0), 0);

  return (
    <Dialog open={open} onOpenChange={handleClose}>
      <DialogContent className="max-w-3xl max-h-[90vh] overflow-y-auto">
        <DialogHeader>
          <DialogTitle>新增用款申请</DialogTitle>
        </DialogHeader>

        <form onSubmit={handleSubmit} className="space-y-4">
          <div className="space-y-2">
            <Label htmlFor="institutionName">申请单位 *</Label>
            <Input
              id="institutionName"
              value={institutionName}
              onChange={(e) => setInstitutionName(e.target.value)}
              placeholder="请输入学校或教育机构名称"
            />
          </div>

          <div className="grid grid-cols-2 gap-4">
            <div className="space-y-2">
              <Label htmlFor="amount">申请金额（元） *</Label>
              <Input
                id="amount"
                type="number"
                value={amount}
                onChange={(e) => setAmount(e.target.value)}
                placeholder="请输入申请金额"
                min="0"
                step="0.01"
              />
            </div>

            <div className="space-y-2">
              <Label htmlFor="purposeCategory">用途分类 *</Label>
              <Select value={purposeCategory} onValueChange={setPurposeCategory}>
                <SelectTrigger>
                  <SelectValue placeholder="请选择用途分类" />
                </SelectTrigger>
                <SelectContent>
                  <SelectItem value="设备采购">设备采购</SelectItem>
                  <SelectItem value="基础设施">基础设施</SelectItem>
                  <SelectItem value="培训教育">培训教育</SelectItem>
                  <SelectItem value="学生资助">学生资助</SelectItem>
                  <SelectItem value="其他">其他</SelectItem>
                </SelectContent>
              </Select>
            </div>
          </div>

          <div className="space-y-2">
            <Label htmlFor="purpose">用款用途 *</Label>
            <Input
              id="purpose"
              value={purpose}
              onChange={(e) => setPurpose(e.target.value)}
              placeholder="如：购置教学设备"
            />
          </div>

          <div className="space-y-2">
            <Label htmlFor="description">详细说明 *</Label>
            <Textarea
              id="description"
              value={description}
              onChange={(e) => setDescription(e.target.value)}
              placeholder="请详细说明用款的具体用途和计划"
              rows={3}
            />
          </div>

          {/* 预算明细 */}
          <div className="space-y-3">
            <div className="flex items-center justify-between">
              <Label>预算明细 *</Label>
              <Button type="button" size="sm" variant="outline" onClick={addBudgetItem}>
                <Plus className="w-4 h-4 mr-1" />
                添加项目
              </Button>
            </div>

            <div className="space-y-2">
              {budgetDetails.map((budget, index) => (
                <div key={budget.id} className="flex gap-2 items-start p-3 bg-gray-50 rounded-lg">
                  <div className="flex-1 grid grid-cols-3 gap-2">
                    <Input
                      placeholder="项目名称"
                      value={budget.item}
                      onChange={(e) => updateBudgetItem(budget.id, 'item', e.target.value)}
                    />
                    <Input
                      type="number"
                      placeholder="金额"
                      value={budget.amount || ''}
                      onChange={(e) => updateBudgetItem(budget.id, 'amount', parseFloat(e.target.value) || 0)}
                      min="0"
                      step="0.01"
                    />
                    <Input
                      placeholder="说明"
                      value={budget.description}
                      onChange={(e) => updateBudgetItem(budget.id, 'description', e.target.value)}
                    />
                  </div>
                  {budgetDetails.length > 1 && (
                    <Button
                      type="button"
                      size="sm"
                      variant="ghost"
                      onClick={() => removeBudgetItem(budget.id)}
                    >
                      <X className="w-4 h-4" />
                    </Button>
                  )}
                </div>
              ))}
            </div>

            <div className="flex justify-end text-sm">
              <span className="text-gray-600">预算总额：</span>
              <span className="ml-2">¥{totalBudget.toFixed(2)}</span>
            </div>
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
