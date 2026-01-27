import { DonationApplication, FundingApplication } from '../types';

// 生成示例捐助数据
export function generateMockDonations(): DonationApplication[] {
  return [
    {
      id: 'DON-2024-001',
      donorType: 'enterprise',
      donorName: '腾讯公益基金会',
      contactPhone: '13800138001',
      idNumber: '91440300708461136T',
      amount: 500000,
      purpose: '支持山区学校基础设施建设',
      notes: '定向捐助给贵州山区中小学',
      status: 'completed',
      applicationDate: '2024-10-15',
      certificate: 'CERT-2024-001'
    },
    {
      id: 'DON-2024-002',
      donorType: 'individual',
      donorName: '张明',
      contactPhone: '13800138002',
      idNumber: '110101199001011234',
      amount: 10000,
      purpose: '资助贫困学生',
      notes: '希望帮助10名贫困学生',
      status: 'approved',
      applicationDate: '2024-11-01',
      certificate: 'CERT-2024-002'
    },
    {
      id: 'DON-2024-003',
      donorType: 'enterprise',
      donorName: '阿里巴巴公益基金会',
      contactPhone: '13800138003',
      idNumber: '91330000MA27XB1Y3P',
      amount: 800000,
      purpose: '教育信息化设备采购',
      notes: '用于配置多媒体教室',
      status: 'completed',
      applicationDate: '2024-09-20',
      certificate: 'CERT-2024-003'
    },
    {
      id: 'DON-2024-004',
      donorType: 'individual',
      donorName: '李华',
      contactPhone: '13800138004',
      idNumber: '310101198505051234',
      amount: 5000,
      purpose: '支持乡村教师培训',
      notes: '个人捐助',
      status: 'pending',
      applicationDate: '2024-11-02',
    },
  ];
}

// 生成示例用款数据
export function generateMockFundings(): FundingApplication[] {
  return [
    {
      id: 'FUND-2024-001',
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
      status: 'completed',
      applicationDate: '2024-10-01',
      approvalDate: '2024-10-05'
    },
    {
      id: 'FUND-2024-002',
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
      status: 'approved',
      applicationDate: '2024-10-15',
      approvalDate: '2024-10-20'
    },
    {
      id: 'FUND-2024-003',
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
      status: 'pending',
      applicationDate: '2024-11-01'
    }
  ];
}
