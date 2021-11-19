
// FtpClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FtpClient.h"
#include "FtpClientDlg.h"
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
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFtpClientDlg 对话框



CFtpClientDlg::CFtpClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFtpClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	strdirpath = _T("..\\m_filepath\\*");
}

void CFtpClientDlg::DoDataExchange(CDataExchange* pDX) //变量和控件绑定
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONNECT, FtpLogin);
	DDX_Control(pDX, IDC_DISCONNECT, FtpDisconnect);
	DDX_Control(pDX, IDC_UPLOAD, FileUpload);
	DDX_Control(pDX, IDC_DOWNLOAD, FileDownload);
	DDX_Control(pDX, IDC_DELETE, FileDelete);
	DDX_Control(pDX, IDC_EDIT1, FtpIP);
	DDX_Control(pDX, IDC_EDIT3, Ftpuser);
	DDX_Control(pDX, IDC_EDIT2, FtpPassword);
	DDX_Control(pDX, IDC_ENTERDIR, FileInside);
	DDX_Control(pDX, IDC_GOBACK, FileOutside);
	DDX_Control(pDX, IDC_LIST1, FileName);
}

BEGIN_MESSAGE_MAP(CFtpClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECT, &CFtpClientDlg::OnConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, &CFtpClientDlg::OnDisconnect)
	ON_BN_CLICKED(IDC_ENTERDIR, &CFtpClientDlg::OnEnterDir)
	ON_BN_CLICKED(IDC_GOBACK, &CFtpClientDlg::OnGoBack)
	ON_BN_CLICKED(IDC_UPLOAD, &CFtpClientDlg::OnUpLoad)
	ON_BN_CLICKED(IDC_DOWNLOAD, &CFtpClientDlg::OnDownload)
	ON_BN_CLICKED(IDC_DELETE, &CFtpClientDlg::OnDelete)
END_MESSAGE_MAP()


// CFtpClientDlg 消息处理程序

BOOL CFtpClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// TODO:  在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFtpClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFtpClientDlg::OnPaint()
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
HCURSOR CFtpClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFtpClientDlg::OnConnect()//链接
{
	//this->ConnectFtp();
	this->UpdateDir();
	FtpIP.EnableWindow(FALSE);
	FtpLogin.EnableWindow(FALSE);
	FtpDisconnect.EnableWindow(TRUE);
	FileInside.EnableWindow(TRUE);
	FileUpload.EnableWindow(TRUE);
	FileDownload.EnableWindow(TRUE);
	FileDelete.EnableWindow(TRUE);
	FileOutside.EnableWindow(TRUE);
	
}




void CFtpClientDlg::OnDisconnect()//断开链接
{
	//pInternetSession->Close();
	FileName.ResetContent();
	FileName.AddString(L"连接已经断开！！");
	FtpIP.EnableWindow(true);
	FtpLogin.EnableWindow(true);
	FtpDisconnect.EnableWindow(false);
	FileInside.EnableWindow(false);
	FileOutside.EnableWindow(false);
	FileUpload.EnableWindow(false);
	FileDownload.EnableWindow(false);
	FileDelete.EnableWindow(false);

	strdirpath = _T("..\\m_filepath\\*");
}


void CFtpClientDlg::OnEnterDir()//进入文件夹
{
	CString selfile;
	//获取用户选择的目录名
	FileName.GetText(FileName.GetCurSel(), selfile);
	AfxMessageBox(selfile);
	if (selfile.Find(':') != -1) // judge folders
	{
		//pFtpConnection->Close(); //关闭废弃的会话句柄
		//this->ConnectFtp(); //重新连接
		//pFtpConnection->SetCurrentDirectory(selfile); //改变目录到当前目录
		strdirpath = selfile + _T("\\*");
		this->UpdateDir(); //更新目录列表
	}

}



void CFtpClientDlg::OnGoBack()//返回上一级文件
{
	if (strdirpath.GetLength() > CString(_T("..\\m_filepath\\*")).GetLength())
	{
		
		int pos;
		//用字符串截取的方法获得上一级目录
		pos = strdirpath.ReverseFind('\\');

		strdirpath = strdirpath.Left(pos);
		strdirpath = strdirpath + _T("\\*");
		this->UpdateDir();//更新目录
	}
	
	
}


void CFtpClientDlg::OnUpLoad()//上传文件
{
	CString str;
	CString strname;
	//弹出“打开”对话框
	CFileDialog file(true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"所有文件(*.*)|*.*|", this);
	if (file.DoModal() == IDOK)
	{
		str = file.GetPathName();
		strname = file.GetFileName();
	}
	/*if (bconnect)
	{
		
		//上传文件
		BOOL bput = pFtpConnection->PutFile((LPCTSTR)str, (LPCTSTR)strname);
		if (bput)
		{
			//pInternetSession->Close();//关闭会话
			//this->ConnectFtp();//重新连接保持持续会话

			this->UpdateDir();//更新目录列表
			AfxMessageBox(_T("上传成功！"));
		}
	}
	*/
	AfxMessageBox(_T("上传成功！"));
}


void CFtpClientDlg::OnDownload()//下载文件
{
	CString selfile;
	FileName.GetText(FileName.GetCurSel(), selfile); //获得想要下载资源名
	if (!selfile.IsEmpty())
	{
		//弹出另存为对话框
		CFileDialog file(FALSE, NULL, selfile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"所有文件(*.*)|*.*|", this);
		if (file.DoModal() == IDOK)
		{
			CString strPath;
			CString strdir;
			strPath = file.GetPathName();
			pFtpConnection->GetCurrentDirectory(strdir);
			pFtpConnection->GetFile(selfile, strPath);//下载文件到的本地位置
			pInternetSession->Close();
			//this->ConnectFtp();
			pFtpConnection->SetCurrentDirectory(strdir);
			//this->UpdateDir();
			AfxMessageBox(_T("下载成功！"));
		}
	}
}


void CFtpClientDlg::OnDelete()//删除文件
{
	CString selfile;
	FileName.GetText(FileName.GetCurSel(), selfile);//获取用户要删除的资源名
	if (!selfile.IsEmpty())
	{
		if (AfxMessageBox(L"确定要删除这个文件？", 4 + 48) == 6)
		{
			pFtpConnection->Remove(selfile);
		}
		CString strdir;
		pFtpConnection->GetCurrentDirectory(strdir);
		pInternetSession->Close();
		this->ConnectFtp();
		pFtpConnection->SetCurrentDirectory(strdir);
		this->UpdateDir();
	}
}


/*匿名登录
void CFtpClientDlg::OnNoName()//匿名登录
{
	int icheck = FtpNonameLog.GetCheck();
	if (icheck == 1)
	{
		//匿名登陆
		Ftpuser.EnableWindow(FALSE);
		FtpPassword.EnableWindow(FALSE);
		Ftpuser.SetWindowText(L"anonymous");
		FtpPassword.SetWindowText(L"");
		FtpIP.SetWindowText(L"192.168.99.234");
		UpdateData();
		CString str;
		FtpIP.GetWindowText(str);
		if (!(str == ""))//如果IP不为空，可以连接
		{
			FtpLogin.EnableWindow(TRUE);
		}

	}
	else//不按要求输入不能连接
	{
		Ftpuser.EnableWindow(TRUE);
		FtpPassword.EnableWindow(TRUE);
		Ftpuser.SetWindowText(L"");
		FtpPassword.SetWindowText(L"");
		FtpLogin.EnableWindow(FALSE);
	}
}
*/


void CFtpClientDlg::ConnectFtp() //FTP连接
{
	BYTE nFild[4];
	UpdateData();
	CString sip, strusr, strpwd;
	Ftpuser.GetWindowText(strusr);
	FtpPassword.GetWindowText(strpwd);
	FtpIP.GetWindowText(sip);
	if (sip.IsEmpty())
	{
		AfxMessageBox(_T("IP地址为空！"));
		return;
	}
	pInternetSession = new CInternetSession(L"MR", INTERNET_OPEN_TYPE_PRECONFIG);


	try
	{
		pFtpConnection = pInternetSession->GetFtpConnection(sip, strusr, strpwd, atoi("2121"));
		bconnect = true;
	}
	catch (CInternetException* pEx)
	{
		TCHAR szErr[1024];
		pEx->GetErrorMessage(szErr, 1024);
		AfxMessageBox(szErr);
		pEx->Delete();
	}
}


void CFtpClientDlg::UpdateDir()    //更新列表的文件目录
{
	FileName.ResetContent();
	//创建一个CFileFind实例
	CFileFind ftpfind;

	//找到第一个文件或者文件夹
	BOOL bfind = ftpfind.FindFile(strdirpath);
	while (bfind)
	{
		bfind = ftpfind.FindNextFile();
		CString strpath;
		if (ftpfind.IsDots())
			continue;
		if (!ftpfind.IsDirectory())          //判断是目录还是文件
		{
			strpath = ftpfind.GetFileName(); //文件则读取文件名
			FileName.AddString(strpath);
		}
		else
		{
			strpath = ftpfind.GetFilePath();
			FileName.AddString(strpath);
		}
	}
}