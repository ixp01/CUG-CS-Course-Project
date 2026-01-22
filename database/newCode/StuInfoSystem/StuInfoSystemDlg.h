
// StuInfoSystemDlg.h: 头文件
//
#include"SQLinterface.h"
#pragma once
#include<string>
#include<CString>
#include"SeekDlg.h"

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
	SQLinterface SQL;
	vector<StuInfo> Info;
public:
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton1();
	CListCtrl m_list;
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton2();
};
