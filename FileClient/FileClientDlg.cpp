
// FileClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "FileClient.h"
#include "FileClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_SOCK WM_USER + 1// 自定义消息，在WM_USER的基础上进行
#define MAX_BUF_SIZE 128

// CFileClientDlg 对话框


CFileClientDlg::CFileClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILECLIENT_DIALOG, pParent)
	, m_ip(0x7f000001)
	, m_port_client(9191)
	, m_port_server(9190)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	hCommSock = 0;
	strdirpath = ""; //初始化默认路径
}

void CFileClientDlg::DoDataExchange(CDataExchange* pDX) //变量和控件绑定
{
	CDialogEx::DoDataExchange(pDX);

	// 控制类
	DDX_Control(pDX, IDC_CONNECT, ServerLogin);
	DDX_Control(pDX, IDC_DISCONNECT, ServerDisconnect);
	DDX_Control(pDX, IDC_UPLOAD, FileUpload);
	DDX_Control(pDX, IDC_DOWNLOAD, FileDownload);
	DDX_Control(pDX, IDC_DELETE, FileDelete);
	DDX_Control(pDX, IDC_IPADDRESS1, ServerIP);
	DDX_Control(pDX, IDC_EDIT4, ServerPort);
	DDX_Control(pDX, IDC_EDIT5, ClientPort);
	DDX_Control(pDX, IDC_EDIT3, ServerUsername);
	DDX_Control(pDX, IDC_EDIT2, ServerPassword);
	DDX_Control(pDX, IDC_ENTERDIR, FileInside);
	DDX_Control(pDX, IDC_GOBACK, FileOutside);
	DDX_Control(pDX, IDC_LIST1, FileName);

	// 变量类
	DDX_IPAddress(pDX, IDC_IPADDRESS1, m_ip);
	DDX_Text(pDX, IDC_EDIT5, m_port_client);
	DDX_Text(pDX, IDC_EDIT4, m_port_server);

}

BEGIN_MESSAGE_MAP(CFileClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECT, &CFileClientDlg::OnConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, &CFileClientDlg::OnDisconnect)
	ON_BN_CLICKED(IDC_ENTERDIR, &CFileClientDlg::OnEnterDir)
	ON_BN_CLICKED(IDC_GOBACK, &CFileClientDlg::OnGoBack)
	ON_BN_CLICKED(IDC_UPLOAD, &CFileClientDlg::OnUpLoad)
	ON_BN_CLICKED(IDC_DOWNLOAD, &CFileClientDlg::OnDownload)
	ON_BN_CLICKED(IDC_DELETE, &CFileClientDlg::OnDelete)
END_MESSAGE_MAP()


// CFileClientDlg 消息处理程序

BOOL CFileClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFileClientDlg::OnPaint()
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
HCURSOR CFileClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFileClientDlg::OnConnect() // 连接
{
	if (ConnectServ()) // 连接服务器
	{
		ServerIP.EnableWindow(FALSE);
		ServerLogin.EnableWindow(FALSE);
		ServerDisconnect.EnableWindow(TRUE);
		FileInside.EnableWindow(TRUE);
		FileUpload.EnableWindow(TRUE);
		FileDownload.EnableWindow(TRUE);
		FileDelete.EnableWindow(TRUE);
		FileOutside.EnableWindow(TRUE);
	}
}


void CFileClientDlg::OnDisconnect() //断开连接
{
	//pInternetSession->Close();
	FileName.ResetContent();
	FileName.AddString("连接已经断开！！");
	ServerIP.EnableWindow(TRUE);
	ServerLogin.EnableWindow(TRUE);
	ServerDisconnect.EnableWindow(FALSE);
	FileInside.EnableWindow(FALSE);
	FileOutside.EnableWindow(FALSE);
	FileUpload.EnableWindow(FALSE);
	FileDownload.EnableWindow(FALSE);
	FileDelete.EnableWindow(FALSE);

	strdirpath = "";
}


LRESULT CFileClientDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	SOCKET hSocket;
	char buf[MAX_BUF_SIZE] = { 0 };
	int strLen;
	int newEvent;
	switch (message)
	{
	case WM_SOCK:
		hSocket = (SOCKET)LOWORD(wParam);
		newEvent = LOWORD(lParam);
		switch (newEvent)
		{
		case FD_READ:			
			strLen = recv(hSocket, buf, MAX_BUF_SIZE, 0);
			state = *(DWORD*)buf;
			StateHandler();
			if (strLen <= 0)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					closesocket(hSocket);
					MessageBox("recv() failed", "Client", MB_OK);
					break;
				}
			}
			else
			{
				CString m_recv(buf);
				UpdateDir(m_recv);
			}
			break;
		case FD_CLOSE:
			closesocket(hSocket);
			break;
		}
		break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}


void CFileClientDlg::OnEnterDir() //进入文件夹
{
	CString selFile;
	
	FileName.GetText(FileName.GetCurSel(), selFile); //获取用户选择的目录名
	
	if (selFile.Find('.') == -1) // 判断是否为文件夹，原理：文件名有'.'
	{
		m_send = selFile + "\\*";
		strdirpath = m_send; // 本地保存当前的文件夹路径，在返回上一级文件夹时会使用到
		int strLen = m_send.GetLength();
		send(hCommSock, m_send, strLen, 0);
	}
}


void CFileClientDlg::OnGoBack() //返回上一级文件夹（TODO：暂时不可以下载文件夹）
{
	if (strdirpath.GetLength() != 0) // 判断不是初始化时的目录
	{
		int pos;
		//用字符串截取的方法获得上一级目录
		pos = strdirpath.ReverseFind('\\');
		strdirpath = strdirpath.Left(pos);
		pos = strdirpath.ReverseFind('\\');
		strdirpath = strdirpath.Left(pos);
		
		strdirpath = strdirpath + "\\*";

		int strLen = strdirpath.GetLength();
		send(hCommSock, strdirpath, strLen, 0);
	}
}

BOOL CFileClientDlg::UploadOnce(const char* buf, int length)
{
	int leftToSend = length;
	int bytesSend = 0;

	do// 单次发送
	{
		const char* sendBuf = buf + length - leftToSend;
		bytesSend = send(hCommSock, sendBuf, leftToSend, 0);
		if (bytesSend == SOCKET_ERROR) return FALSE;
		leftToSend -= bytesSend;
	} while (leftToSend > 0);
	
	return TRUE;
}

void CFileClientDlg::StateHandler()
{
	switch (state)
	{
	case 752:
		send(hCommSock, (char*)&state, sizeof(state), 0);
		break;
	case 753:
		nameLength = uploadName.GetLength();
		if (UploadOnce((char*)&nameLength, sizeof(nameLength)) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while sending file name length\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		break;
	case 754:
		send(hCommSock, (char*)&state, sizeof(state), 0);
		if (UploadOnce(uploadName.GetBuffer(), uploadName.GetLength()) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while sending file name\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		uploadName.ReleaseBuffer();
		break;
	case 755:
		send(hCommSock, (char*)&state, sizeof(state), 0);
		fileLength = uploadFile.GetLength();//约定文件长度用ULONGLONG存储，长度是8个字节
		if (UploadOnce((char*)&fileLength, sizeof(fileLength)) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while sending file length\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		state = 0;
		break;
	}
}

void CFileClientDlg::OnUpLoad()//上传文件（TODO：暂时不可以下载文件夹）
{
	//弹出“打开”对话框
	char szFilters[] = "所有文件 (*.*)|*.*||";
	CFileDialog fileDlg(TRUE, NULL, NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters);

	char desktop[MAX_PATH] = { 0 };
	SHGetSpecialFolderPath(NULL, desktop, CSIDL_DESKTOP, FALSE);
	fileDlg.m_ofn.lpstrInitialDir = desktop;//把默认路径设置为桌面
	
	if (fileDlg.DoModal() == IDOK)
	{
		fileAbsPath = fileDlg.GetPathName();
		uploadName = fileDlg.GetFileName();
		CFileException errFile;

		if (!(uploadFile.Open(fileAbsPath.GetString(),
			CFile::modeRead | CFile::typeBinary, &errFile)))
		{
			char errOpenFile[256];
			errFile.GetErrorMessage(errOpenFile, 255);
			TRACE("\nError occurred while uploading file:\n"
				"\tFile name: %s\n\tCause: %s\n\tm_cause = %d\n\t m_IOsError = %d\n",
				errFile.m_strFileName, errOpenFile, errFile.m_cause, errFile.m_lOsError);
			ASSERT(1);
		}

		state = 752;//upload状态码
		StateHandler();

//#define SEND_CHUNK_SIZE 4096
//
//		char chunkBuf[SEND_CHUNK_SIZE] = { 0 };//#define SEND_CHUNK_SIZE 4096
//		int leftToSend = fileLength;
//
//		do//发送整个文件
//		{
//			int readChunkSize = uploadFile.Read(chunkBuf, SEND_CHUNK_SIZE);//#define SEND_CHUNK_SIZE 4096
//			if (UploadOnce(chunkBuf, readChunkSize) == FALSE)
//			{
//				DWORD errSend = WSAGetLastError();
//				TRACE("\nError occurred while sending file chunks\n"
//					"\tGetLastError = %d\n", errSend);
//				ASSERT(errSend != WSAEWOULDBLOCK);
//			}
//			leftToSend -= readChunkSize;
//		} while (leftToSend > 0);

		//AfxMessageBox((CString)"上传成功！");
	}
}


void CFileClientDlg::OnDownload()//下载文件（TODO：暂时不可以下载文件夹）
{
	CString selFile;
	FileName.GetText(FileName.GetCurSel(), selFile); //获得想要下载资源名
	if (!selFile.IsEmpty())
	{
		//弹出另存为对话框
		CString fileExt = selFile.Right(selFile.GetLength() - selFile.Find('.'));
		char szFilters[32] = { 0 };
		sprintf_s(szFilters, "(*%s)|*%s||", fileExt.GetString(), fileExt.GetString());
		CFileDialog fileDlg(FALSE, NULL, selFile,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters);

		char desktop[MAX_PATH] = { 0 };
		SHGetSpecialFolderPath(NULL, desktop, CSIDL_DESKTOP, FALSE);
		fileDlg.m_ofn.lpstrInitialDir = desktop;//把默认路径设置为桌面

		if (fileDlg.DoModal() == IDOK)
		{
			CString fileName = fileDlg.GetPathName();
			if (fileDlg.GetFileExt() == "")
			{
				fileName += fileExt;
			}
			//CString strdirpath; 当前所在的云端目录
			//selFile 想要下载的文件名
			//filename 下载的目标文件名（含绝对路径，含扩展名）
			//hCommSock 用来通信的套接字

			AfxMessageBox((CString)"下载成功！");
		}
	}
}

void CFileClientDlg::OnDelete() // 删除文件
{
	CString selFile;
	FileName.GetText(FileName.GetCurSel(), selFile); // 获取用户要删除的文件名
	if (!selFile.IsEmpty())
	{
		if (AfxMessageBox((CString)"确定要删除这个文件？", 4 + 48) == 6)
		{
			
		}
	}
}


bool CFileClientDlg::ConnectServ() // 连接服务器
{	
	// 判断异常情况
	if (m_ip == NULL)
	{
		AfxMessageBox((CString)"IP地址为空！");
		return false;
	}
	else if (m_port_server == NULL)
	{
		AfxMessageBox((CString)"云端端口为空！");
		return false;
	}
	else if (m_port_client == NULL)
	{
		AfxMessageBox((CString)"本地端口为空！");
		return false;
	}
	
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(m_ip);
	servAdr.sin_port = htons(m_port_server);
	WSADATA wsaData;
	//SOCKADDR_IN clntAdr;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		MessageBox("WSAStartup() failed", "Client", MB_OK);
		return false;
	}
	//memset(&clntAdr, 0, sizeof(clntAdr));
	//clntAdr.sin_family = AF_INET;
	//clntAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	//clntAdr.sin_port = htons(m_port_client);
	hCommSock = socket(AF_INET, SOCK_STREAM, 0);
	if (hCommSock == INVALID_SOCKET)
	{
		MessageBox("socket() failed", "Client", MB_OK);
		return false;
	}
	if (connect(hCommSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
	{
		MessageBox("connect() failed", "Client", MB_OK);
		return false;
	}
	if (WSAAsyncSelect(hCommSock, m_hWnd, WM_SOCK, FD_READ | FD_CLOSE) == SOCKET_ERROR)
	{
		MessageBox("WSAAsyncSelect() failed", "Client", MB_OK);
		return false;
	}

	return true;
}


void CFileClientDlg::UpdateDir(CString recv)  // 更新列表显示的文件目录
{
	FileName.ResetContent();

	std::vector<std::string> v;
	std::string strStr = recv.GetBuffer(0);
	SplitString(strStr, v, "|"); //可按多个字符来分隔;

	for (auto i = 0; i != v.size(); ++i)
		FileName.AddString(v[i].c_str());
}


void CFileClientDlg::SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c) // 字符串分割函数
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}