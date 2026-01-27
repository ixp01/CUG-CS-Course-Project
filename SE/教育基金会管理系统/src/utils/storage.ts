import { User, DonationApplication, FundingApplication, MonthlyFinancial } from '../types';

// 存储键
const STORAGE_KEYS = {
  USERS: 'education_fund_users',
  CURRENT_USER: 'education_fund_current_user',
  DONATIONS: 'education_fund_donations',
  FUNDINGS: 'education_fund_fundings',
  FINANCIALS: 'education_fund_financials',
};

// 初始化默认用户和示例数据
export const initializeDefaultUser = () => {
  const users = getUsers();
  const defaultUser = users.find(u => u.username === 'user');
  
  if (!defaultUser) {
    users.push({
      username: 'user',
      password: '123456',
      phone: '13800138000',
    });
    localStorage.setItem(STORAGE_KEYS.USERS, JSON.stringify(users));
  }

  // 初始化示例数据（仅在首次加载时）
  const donations = getDonations();
  if (donations.length === 0) {
    initializeSampleData();
  }
};

// 初始化示例数据
const initializeSampleData = () => {
  // 添加示例捐助数据
  const sampleDonations = [
    {
      id: 'DON-2024-09-001',
      donorType: 'enterprise' as const,
      donorName: '腾讯公益基金会',
      contactPhone: '13800138001',
      idNumber: '91440300708461136T',
      amount: 500000,
      purpose: '支持山区学校基础设施建设',
      notes: '定向捐助给贵州山区中小学',
      status: 'completed' as const,
      applicationDate: '2024-09-15',
      certificate: 'CERT-2024-001'
    },
    {
      id: 'DON-2024-10-001',
      donorType: 'enterprise' as const,
      donorName: '阿里巴巴公益基金会',
      contactPhone: '13800138003',
      idNumber: '91330000MA27XB1Y3P',
      amount: 800000,
      purpose: '教育信息化设备采购',
      notes: '用于配置多媒体教室',
      status: 'completed' as const,
      applicationDate: '2024-10-20',
      certificate: 'CERT-2024-003'
    },
    {
      id: 'DON-2024-11-001',
      donorType: 'individual' as const,
      donorName: '张明',
      contactPhone: '13800138002',
      idNumber: '110101199001011234',
      amount: 10000,
      purpose: '资助贫困学生',
      notes: '希望帮助10名贫困学生',
      status: 'approved' as const,
      applicationDate: '2024-11-01',
      certificate: 'CERT-2024-002'
    },
    {
      id: 'DON-2024-11-002',
      donorType: 'individual' as const,
      donorName: '李华',
      contactPhone: '13800138004',
      idNumber: '310101198505051234',
      amount: 5000,
      purpose: '支持乡村教师培训',
      notes: '个人捐助',
      status: 'pending' as const,
      applicationDate: '2024-11-02',
    },
  ];

  sampleDonations.forEach(donation => addDonation(donation));

  // 添加示例用款数据
  const sampleFundings = [
    {
      id: 'FUND-2024-10-001',
      institutionName: '希望小学',
      amount: 150000,
      purpose: '购置教学设备',
      purposeCategory: '设备采购',
      description: '购买电脑、投影仪等多媒体教学设备',
      budgetDetails: [
        { id: '1', item: '电脑', amount: 80000, description: '20台教学电脑' },
        { id: '2', item: '投影仪', amount: 50000, description: '10台多媒体投影仪' },
        { id: '3', item: '其他设备', amount: 20000, description: '音响、白板等' }
      ],
      status: 'completed' as const,
      applicationDate: '2024-10-01',
      approvalDate: '2024-10-05'
    },
    {
      id: 'FUND-2024-10-002',
      institutionName: '阳光中学',
      amount: 80000,
      purpose: '图书馆建设',
      purposeCategory: '基础设施',
      description: '建设学校图书馆，购置图书',
      budgetDetails: [
        { id: '1', item: '图书采购', amount: 50000, description: '购买5000册图书' },
        { id: '2', item: '书架', amount: 20000, description: '图书馆书架' },
        { id: '3', item: '阅览桌椅', amount: 10000, description: '阅览室桌椅' }
      ],
      status: 'approved' as const,
      applicationDate: '2024-10-15',
      approvalDate: '2024-10-20'
    },
    {
      id: 'FUND-2024-11-001',
      institutionName: '明德小学',
      amount: 50000,
      purpose: '教师培训',
      purposeCategory: '培训教育',
      description: '组织教师参加专业培训',
      budgetDetails: [
        { id: '1', item: '培训费用', amount: 30000, description: '20名教师培训费' },
        { id: '2', item: '交通住宿', amount: 15000, description: '差旅费用' },
        { id: '3', item: '培训材料', amount: 5000, description: '教材资料' }
      ],
      status: 'pending' as const,
      applicationDate: '2024-11-01'
    }
  ];

  sampleFundings.forEach(funding => addFunding(funding));

  // 计算财务数据
  calculateMonthlyFinancials();
};

// 用户相关
export const getUsers = (): User[] => {
  const data = localStorage.getItem(STORAGE_KEYS.USERS);
  return data ? JSON.parse(data) : [];
};

export const addUser = (user: User): boolean => {
  const users = getUsers();
  if (users.find(u => u.username === user.username)) {
    return false;
  }
  users.push(user);
  localStorage.setItem(STORAGE_KEYS.USERS, JSON.stringify(users));
  return true;
};

export const getCurrentUser = (): string | null => {
  return localStorage.getItem(STORAGE_KEYS.CURRENT_USER);
};

export const setCurrentUser = (username: string) => {
  localStorage.setItem(STORAGE_KEYS.CURRENT_USER, username);
};

export const logout = () => {
  localStorage.removeItem(STORAGE_KEYS.CURRENT_USER);
};

export const validateLogin = (username: string, password: string): boolean => {
  const users = getUsers();
  return users.some(u => u.username === username && u.password === password);
};

export const findUserByCredentials = (username: string, phone: string): User | null => {
  const users = getUsers();
  return users.find(u => u.username === username && u.phone === phone) || null;
};

// 捐助申请相关
export const getDonations = (): DonationApplication[] => {
  const data = localStorage.getItem(STORAGE_KEYS.DONATIONS);
  return data ? JSON.parse(data) : [];
};

export const saveDonations = (donations: DonationApplication[]) => {
  localStorage.setItem(STORAGE_KEYS.DONATIONS, JSON.stringify(donations));
};

export const addDonation = (donation: DonationApplication) => {
  const donations = getDonations();
  donations.push(donation);
  saveDonations(donations);
};

export const updateDonation = (id: string, updates: Partial<DonationApplication>) => {
  const donations = getDonations();
  const index = donations.findIndex(d => d.id === id);
  if (index !== -1) {
    donations[index] = { ...donations[index], ...updates };
    saveDonations(donations);
  }
};

export const deleteDonation = (id: string) => {
  const donations = getDonations().filter(d => d.id !== id);
  saveDonations(donations);
};

// 用款申请相关
export const getFundings = (): FundingApplication[] => {
  const data = localStorage.getItem(STORAGE_KEYS.FUNDINGS);
  return data ? JSON.parse(data) : [];
};

export const saveFundings = (fundings: FundingApplication[]) => {
  localStorage.setItem(STORAGE_KEYS.FUNDINGS, JSON.stringify(fundings));
};

export const addFunding = (funding: FundingApplication) => {
  const fundings = getFundings();
  fundings.push(funding);
  saveFundings(fundings);
};

export const updateFunding = (id: string, updates: Partial<FundingApplication>) => {
  const fundings = getFundings();
  const index = fundings.findIndex(f => f.id === id);
  if (index !== -1) {
    fundings[index] = { ...fundings[index], ...updates };
    saveFundings(fundings);
  }
};

export const deleteFunding = (id: string) => {
  const fundings = getFundings().filter(f => f.id !== id);
  saveFundings(fundings);
};

// 财务数据相关
export const getFinancials = (): MonthlyFinancial[] => {
  const data = localStorage.getItem(STORAGE_KEYS.FINANCIALS);
  return data ? JSON.parse(data) : [];
};

export const saveFinancials = (financials: MonthlyFinancial[]) => {
  localStorage.setItem(STORAGE_KEYS.FINANCIALS, JSON.stringify(financials));
};

export const getFinancialByMonth = (month: string): MonthlyFinancial | null => {
  const financials = getFinancials();
  return financials.find(f => f.month === month) || null;
};

export const updateMonthlyFinancial = (month: string, data: Partial<MonthlyFinancial>) => {
  const financials = getFinancials();
  const index = financials.findIndex(f => f.month === month);
  
  if (index !== -1) {
    financials[index] = { ...financials[index], ...data };
  } else {
    financials.push({
      month,
      income: 0,
      expense: 0,
      balance: 0,
      incomeDetails: [],
      expenseDetails: [],
      ...data,
    });
  }
  
  saveFinancials(financials);
};

// 计算月度财务数据
export const calculateMonthlyFinancials = () => {
  const donations = getDonations();
  const fundings = getFundings();
  
  // 按月份分组
  const monthlyData = new Map<string, MonthlyFinancial>();
  
  // 处理捐助收入
  donations.filter(d => d.status === 'completed' || d.status === 'approved').forEach(donation => {
    const month = donation.applicationDate.substring(0, 7); // YYYY-MM
    
    if (!monthlyData.has(month)) {
      monthlyData.set(month, {
        month,
        income: 0,
        expense: 0,
        balance: 0,
        incomeDetails: [],
        expenseDetails: [],
      });
    }
    
    const data = monthlyData.get(month)!;
    data.income += donation.amount;
    data.incomeDetails.push({
      id: donation.id,
      date: donation.applicationDate,
      description: `${donation.donorName}的捐助`,
      amount: donation.amount,
      category: donation.donorType === 'enterprise' ? '企业捐助' : '个人捐助',
    });
  });
  
  // 处理用款支出
  fundings.filter(f => f.status === 'completed' || f.status === 'approved').forEach(funding => {
    const month = funding.applicationDate.substring(0, 7);
    
    if (!monthlyData.has(month)) {
      monthlyData.set(month, {
        month,
        income: 0,
        expense: 0,
        balance: 0,
        incomeDetails: [],
        expenseDetails: [],
      });
    }
    
    const data = monthlyData.get(month)!;
    data.expense += funding.amount;
    data.expenseDetails.push({
      id: funding.id,
      date: funding.applicationDate,
      description: `${funding.institutionName}的用款`,
      amount: funding.amount,
      category: funding.purposeCategory,
    });
  });
  
  // 计算余额
  let cumulativeBalance = 0;
  const sortedMonths = Array.from(monthlyData.keys()).sort();
  
  sortedMonths.forEach(month => {
    const data = monthlyData.get(month)!;
    cumulativeBalance += data.income - data.expense;
    data.balance = cumulativeBalance;
  });
  
  saveFinancials(Array.from(monthlyData.values()));
};
