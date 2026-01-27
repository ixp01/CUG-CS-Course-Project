// 用户类型
export interface User {
  username: string;
  password: string;
  phone: string;
}

// 捐助申请状态
export type DonationStatus = 'pending' | 'approved' | 'rejected' | 'completed';

// 捐助人类型
export type DonorType = 'enterprise' | 'individual';

// 捐助申请
export interface DonationApplication {
  id: string;
  donorType: DonorType;
  donorName: string;
  contactPhone: string;
  idNumber: string;
  amount: number;
  purpose: string;
  notes: string;
  status: DonationStatus;
  applicationDate: string;
  certificate?: string;
}

// 用款申请状态
export type FundingStatus = 'pending' | 'approved' | 'rejected' | 'completed';

// 用款申请
export interface FundingApplication {
  id: string;
  institutionName: string;
  amount: number;
  purpose: string;
  purposeCategory: string;
  description: string;
  budgetDetails: BudgetItem[];
  status: FundingStatus;
  applicationDate: string;
  approvalDate?: string;
}

// 预算明细
export interface BudgetItem {
  id: string;
  item: string;
  amount: number;
  description: string;
}

// 月度财务数据
export interface MonthlyFinancial {
  month: string;
  income: number;
  expense: number;
  balance: number;
  incomeDetails: TransactionDetail[];
  expenseDetails: TransactionDetail[];
}

// 交易明细
export interface TransactionDetail {
  id: string;
  date: string;
  description: string;
  amount: number;
  category: string;
}
