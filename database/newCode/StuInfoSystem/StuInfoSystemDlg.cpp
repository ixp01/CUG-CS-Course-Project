
// StuInfoSystemDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "StuInfoSystem.h"
#include "StuInfoSystemDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CStuInfoSystemDlg 对话框



CStuInfoSystemDlg::CStuInfoSystemDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STUINFOSYSTEM_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStuInfoSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

BEGIN_MESSAGE_MAP(CStuInfoSystemDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CStuInfoSystemDlg::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CStuInfoSystemDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON5, &CStuInfoSystemDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON2, &CStuInfoSystemDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CStuInfoSystemDlg 消息处理程序

BOOL CStuInfoSystemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);  // 整行选择、网格线
	m_list.InsertColumn(0, _T("Sno"), LVCFMT_LEFT, 140);  // 插入第2列的列名
	m_list.InsertColumn(1, _T("Sname"), LVCFMT_LEFT, 120);  // 插入第3列的列名
	m_list.InsertColumn(2, _T("Ssex"), LVCFMT_LEFT, 120);  // 插入第4列的列名
	m_list.InsertColumn(3, _T("Sage"), LVCFMT_LEFT, 120);  // 插入第5列的列名
	m_list.InsertColumn(4, _T("Dno"), LVCFMT_LEFT, 120);  // 插入第6列的列名


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CStuInfoSystemDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CStuInfoSystemDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CStuInfoSystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CStuInfoSystemDlg::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CStuInfoSystemDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	if (SQL.Connect_MySQL())
	{
		Info = SQL.Get_All_Info();
		//MessageBox(TEXT("123"));
		CString str;
		for (int i = 0;i < Info.size();i++)
		{
			str.Format(TEXT("%d"), Info[i].m_Sno);
			m_list.InsertItem(i, str);
			m_list.SetItemText(i, 0, str);
			m_list.SetItemText(i, 1, Info[i].m_Sname.c_str());
			m_list.SetItemText(i,2 ,Info[i].m_Ssex.c_str());
			str.Format(TEXT("%d"), Info[i].m_Sage);
			m_list.SetItemText(i, 3,str);
			str.Format(TEXT("%d"), Info[i].m_Dno);
			m_list.SetItemText(i, 4, str);
		}
	}
	else
	{
		return;
	}
}


void CStuInfoSystemDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	SeekDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		vector<StuInfo> result = SQL.Seek_Info(dlg.m_Sno);

		if (result.empty())
		{
			return;
		}

		m_list.DeleteAllItems();
		CString str;
		for (int i = 0;i < Info.size();i++)
		{
			str.Format(TEXT("%d"), result[i].m_Sno);
			m_list.InsertItem(i, str);
			m_list.SetItemText(i, 0, str);
			m_list.SetItemText(i, 1, result[i].m_Sname.c_str());
			m_list.SetItemText(i, 2, result[i].m_Ssex.c_str());
			str.Format(TEXT("%d"), result[i].m_Sage);
			m_list.SetItemText(i, 3, str);
			str.Format(TEXT("%d"), result[i].m_Dno);
			m_list.SetItemText(i, 4, str);
		}
	}
}


void CStuInfoSystemDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	MessageBox(TEXT("mySQL replies:17"));
}
