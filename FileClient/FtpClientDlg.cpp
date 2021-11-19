
// FtpClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FtpClient.h"
#include "FtpClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CFtpClientDlg �Ի���



CFtpClientDlg::CFtpClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFtpClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	strdirpath = _T("..\\m_filepath\\*");
}

void CFtpClientDlg::DoDataExchange(CDataExchange* pDX) //�����Ϳؼ���
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


// CFtpClientDlg ��Ϣ�������

BOOL CFtpClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFtpClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFtpClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFtpClientDlg::OnConnect()//����
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




void CFtpClientDlg::OnDisconnect()//�Ͽ�����
{
	//pInternetSession->Close();
	FileName.ResetContent();
	FileName.AddString(L"�����Ѿ��Ͽ�����");
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


void CFtpClientDlg::OnEnterDir()//�����ļ���
{
	CString selfile;
	//��ȡ�û�ѡ���Ŀ¼��
	FileName.GetText(FileName.GetCurSel(), selfile);
	AfxMessageBox(selfile);
	if (selfile.Find(':') != -1) // judge folders
	{
		//pFtpConnection->Close(); //�رշ����ĻỰ���
		//this->ConnectFtp(); //��������
		//pFtpConnection->SetCurrentDirectory(selfile); //�ı�Ŀ¼����ǰĿ¼
		strdirpath = selfile + _T("\\*");
		this->UpdateDir(); //����Ŀ¼�б�
	}

}



void CFtpClientDlg::OnGoBack()//������һ���ļ�
{
	if (strdirpath.GetLength() > CString(_T("..\\m_filepath\\*")).GetLength())
	{
		
		int pos;
		//���ַ�����ȡ�ķ��������һ��Ŀ¼
		pos = strdirpath.ReverseFind('\\');

		strdirpath = strdirpath.Left(pos);
		strdirpath = strdirpath + _T("\\*");
		this->UpdateDir();//����Ŀ¼
	}
	
	
}


void CFtpClientDlg::OnUpLoad()//�ϴ��ļ�
{
	CString str;
	CString strname;
	//�������򿪡��Ի���
	CFileDialog file(true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"�����ļ�(*.*)|*.*|", this);
	if (file.DoModal() == IDOK)
	{
		str = file.GetPathName();
		strname = file.GetFileName();
	}
	/*if (bconnect)
	{
		
		//�ϴ��ļ�
		BOOL bput = pFtpConnection->PutFile((LPCTSTR)str, (LPCTSTR)strname);
		if (bput)
		{
			//pInternetSession->Close();//�رջỰ
			//this->ConnectFtp();//�������ӱ��ֳ����Ự

			this->UpdateDir();//����Ŀ¼�б�
			AfxMessageBox(_T("�ϴ��ɹ���"));
		}
	}
	*/
	AfxMessageBox(_T("�ϴ��ɹ���"));
}


void CFtpClientDlg::OnDownload()//�����ļ�
{
	CString selfile;
	FileName.GetText(FileName.GetCurSel(), selfile); //�����Ҫ������Դ��
	if (!selfile.IsEmpty())
	{
		//�������Ϊ�Ի���
		CFileDialog file(FALSE, NULL, selfile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"�����ļ�(*.*)|*.*|", this);
		if (file.DoModal() == IDOK)
		{
			CString strPath;
			CString strdir;
			strPath = file.GetPathName();
			pFtpConnection->GetCurrentDirectory(strdir);
			pFtpConnection->GetFile(selfile, strPath);//�����ļ����ı���λ��
			pInternetSession->Close();
			//this->ConnectFtp();
			pFtpConnection->SetCurrentDirectory(strdir);
			//this->UpdateDir();
			AfxMessageBox(_T("���سɹ���"));
		}
	}
}


void CFtpClientDlg::OnDelete()//ɾ���ļ�
{
	CString selfile;
	FileName.GetText(FileName.GetCurSel(), selfile);//��ȡ�û�Ҫɾ������Դ��
	if (!selfile.IsEmpty())
	{
		if (AfxMessageBox(L"ȷ��Ҫɾ������ļ���", 4 + 48) == 6)
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


/*������¼
void CFtpClientDlg::OnNoName()//������¼
{
	int icheck = FtpNonameLog.GetCheck();
	if (icheck == 1)
	{
		//������½
		Ftpuser.EnableWindow(FALSE);
		FtpPassword.EnableWindow(FALSE);
		Ftpuser.SetWindowText(L"anonymous");
		FtpPassword.SetWindowText(L"");
		FtpIP.SetWindowText(L"192.168.99.234");
		UpdateData();
		CString str;
		FtpIP.GetWindowText(str);
		if (!(str == ""))//���IP��Ϊ�գ���������
		{
			FtpLogin.EnableWindow(TRUE);
		}

	}
	else//����Ҫ�����벻������
	{
		Ftpuser.EnableWindow(TRUE);
		FtpPassword.EnableWindow(TRUE);
		Ftpuser.SetWindowText(L"");
		FtpPassword.SetWindowText(L"");
		FtpLogin.EnableWindow(FALSE);
	}
}
*/


void CFtpClientDlg::ConnectFtp() //FTP����
{
	BYTE nFild[4];
	UpdateData();
	CString sip, strusr, strpwd;
	Ftpuser.GetWindowText(strusr);
	FtpPassword.GetWindowText(strpwd);
	FtpIP.GetWindowText(sip);
	if (sip.IsEmpty())
	{
		AfxMessageBox(_T("IP��ַΪ�գ�"));
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


void CFtpClientDlg::UpdateDir()    //�����б���ļ�Ŀ¼
{
	FileName.ResetContent();
	//����һ��CFileFindʵ��
	CFileFind ftpfind;

	//�ҵ���һ���ļ������ļ���
	BOOL bfind = ftpfind.FindFile(strdirpath);
	while (bfind)
	{
		bfind = ftpfind.FindNextFile();
		CString strpath;
		if (ftpfind.IsDots())
			continue;
		if (!ftpfind.IsDirectory())          //�ж���Ŀ¼�����ļ�
		{
			strpath = ftpfind.GetFileName(); //�ļ����ȡ�ļ���
			FileName.AddString(strpath);
		}
		else
		{
			strpath = ftpfind.GetFilePath();
			FileName.AddString(strpath);
		}
	}
}