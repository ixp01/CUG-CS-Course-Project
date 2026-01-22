
// StuInfoSystemDlg.h: 头文件
//

#include"SQLInterface.h"
#pragma once


// CStuInfoSystemDlg 对话框
class CStuInfoSystemDlg : public CDialogEx
{
// 构造
public:
	CStuInfoSystemDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STUINFOSYSTEM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton6();

public:
	SQLInterface SQL;
	afx_msg void OnBnClickedButton1();
	vector<StuInfo> Info;
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
};
