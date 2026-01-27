import { MonthlyFinancial } from '../types';

// 导出为Excel格式（实际上是CSV，Excel可以打开）
export const exportToExcel = (data: MonthlyFinancial, month: string) => {
  // 准备CSV数据
  const lines: string[] = [];
  
  // 添加标题
  lines.push(`教育基金会财务报表 - ${formatMonthName(month)}`);
  lines.push('');
  
  // 添加汇总信息
  lines.push('财务汇总');
  lines.push('项目,金额（元）');
  lines.push(`本月收入,${data.income.toFixed(2)}`);
  lines.push(`本月支出,${data.expense.toFixed(2)}`);
  lines.push(`月末余额,${data.balance.toFixed(2)}`);
  lines.push('');
  
  // 添加收入明细
  lines.push('收入明细');
  lines.push('日期,描述,类别,金额（元）');
  data.incomeDetails.forEach(detail => {
    lines.push(`${detail.date},${detail.description},${detail.category},${detail.amount.toFixed(2)}`);
  });
  lines.push(`收入合计,,,${data.income.toFixed(2)}`);
  lines.push('');
  
  // 添加支出明细
  lines.push('支出明细');
  lines.push('日期,描述,类别,金额（元）');
  data.expenseDetails.forEach(detail => {
    lines.push(`${detail.date},${detail.description},${detail.category},${detail.amount.toFixed(2)}`);
  });
  lines.push(`支出合计,,,${data.expense.toFixed(2)}`);
  lines.push('');
  
  // 添加备注
  lines.push('备注');
  lines.push(`导出时间: ${new Date().toLocaleString('zh-CN')}`);
  lines.push('本报表由教育基金会管理系统自动生成');
  
  // 生成CSV内容（添加BOM以支持中文）
  const BOM = '\uFEFF';
  const csvContent = BOM + lines.join('\n');
  
  // 创建下载
  const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8;' });
  const link = document.createElement('a');
  const url = URL.createObjectURL(blob);
  
  link.setAttribute('href', url);
  link.setAttribute('download', `财务报表_${month}.csv`);
  link.style.visibility = 'hidden';
  document.body.appendChild(link);
  link.click();
  document.body.removeChild(link);
};

// 导出为TXT文本格式
export const exportToTxt = (data: MonthlyFinancial, month: string) => {
  const lines: string[] = [];
  
  // 添加标题
  lines.push('═══════════════════════════════════════════');
  lines.push(`      教育基金会财务报表 - ${formatMonthName(month)}      `);
  lines.push('═══════════════════════════════════════════');
  lines.push('');
  
  // 添加汇总信息
  lines.push('【财务汇总】');
  lines.push('─────────────────────────────────────────');
  lines.push(`本月收入：¥${data.income.toLocaleString('zh-CN', { minimumFractionDigits: 2, maximumFractionDigits: 2 })}`);
  lines.push(`本月支出：¥${data.expense.toLocaleString('zh-CN', { minimumFractionDigits: 2, maximumFractionDigits: 2 })}`);
  lines.push(`月末余额：¥${data.balance.toLocaleString('zh-CN', { minimumFractionDigits: 2, maximumFractionDigits: 2 })}`);
  lines.push('');
  
  // 添加收入明细
  lines.push('【收入明细】');
  lines.push('─────────────────────────────────────────');
  if (data.incomeDetails.length === 0) {
    lines.push('本月暂无收入记录');
  } else {
    data.incomeDetails.forEach((detail, index) => {
      lines.push(`${index + 1}. ${detail.description}`);
      lines.push(`   日期：${detail.date}`);
      lines.push(`   类别：${detail.category}`);
      lines.push(`   金额：¥${detail.amount.toLocaleString('zh-CN', { minimumFractionDigits: 2, maximumFractionDigits: 2 })}`);
      lines.push('');
    });
    lines.push(`收入合计：¥${data.income.toLocaleString('zh-CN', { minimumFractionDigits: 2, maximumFractionDigits: 2 })}`);
  }
  lines.push('');
  
  // 添加支出明细
  lines.push('【支出明细】');
  lines.push('─────────────────────────────────────────');
  if (data.expenseDetails.length === 0) {
    lines.push('本月暂无支出记录');
  } else {
    data.expenseDetails.forEach((detail, index) => {
      lines.push(`${index + 1}. ${detail.description}`);
      lines.push(`   日期：${detail.date}`);
      lines.push(`   类别：${detail.category}`);
      lines.push(`   金额：¥${detail.amount.toLocaleString('zh-CN', { minimumFractionDigits: 2, maximumFractionDigits: 2 })}`);
      lines.push('');
    });
    lines.push(`支出合计：¥${data.expense.toLocaleString('zh-CN', { minimumFractionDigits: 2, maximumFractionDigits: 2 })}`);
  }
  lines.push('');
  
  // 添加统计分析
  lines.push('【统计分析】');
  lines.push('─────────────────────────────────────────');
  const netChange = data.income - data.expense;
  lines.push(`本月净变化：¥${netChange.toLocaleString('zh-CN', { minimumFractionDigits: 2, maximumFractionDigits: 2 })}`);
  lines.push(`收入笔数：${data.incomeDetails.length} 笔`);
  lines.push(`支出笔数：${data.expenseDetails.length} 笔`);
  if (data.incomeDetails.length > 0) {
    const avgIncome = data.income / data.incomeDetails.length;
    lines.push(`平均收入：¥${avgIncome.toLocaleString('zh-CN', { minimumFractionDigits: 2, maximumFractionDigits: 2 })}`);
  }
  if (data.expenseDetails.length > 0) {
    const avgExpense = data.expense / data.expenseDetails.length;
    lines.push(`平均支出：¥${avgExpense.toLocaleString('zh-CN', { minimumFractionDigits: 2, maximumFractionDigits: 2 })}`);
  }
  lines.push('');
  
  // 添加备注
  lines.push('═══════════════════════════════════════════');
  lines.push(`导出时间：${new Date().toLocaleString('zh-CN')}`);
  lines.push('本报表由教育基金会管理系统自动生成');
  lines.push('═══════════════════════════════════════════');
  
  // 生成文本内容
  const txtContent = lines.join('\n');
  
  // 创建下载
  const blob = new Blob([txtContent], { type: 'text/plain;charset=utf-8;' });
  const link = document.createElement('a');
  const url = URL.createObjectURL(blob);
  
  link.setAttribute('href', url);
  link.setAttribute('download', `财务报表_${month}.txt`);
  link.style.visibility = 'hidden';
  document.body.appendChild(link);
  link.click();
  document.body.removeChild(link);
};

// 格式化月份名称
const formatMonthName = (month: string): string => {
  const date = new Date(month + '-01');
  return date.toLocaleDateString('zh-CN', { year: 'numeric', month: 'long' });
};
