
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
#define CHUNK_SIZE 4096
#define MAX_WSAE_TIMES 10// 单次发送或接收过程中所允许出现WSAEWOULDBLOCK的最大次数

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

	state = 0;
	count = 0;
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


BOOL CFileClientDlg::UploadOnce(const char* buf, int length)
{
	int leftToSend = length;
	int bytesSend = 0;
	int WSAECount = 0;

	do// 单次发送
	{
		const char* sendBuf = buf + length - leftToSend;
		bytesSend = send(hCommSock, sendBuf, leftToSend, 0);
		if (bytesSend == SOCKET_ERROR)
		{
			ASSERT(WSAGetLastError() == WSAEWOULDBLOCK);
			bytesSend = 0;
			WSAECount++;
			if (WSAECount > MAX_WSAE_TIMES) return FALSE;
		}
		leftToSend -= bytesSend;
	} while (leftToSend > 0);

	return TRUE;
}


void CFileClientDlg::UploadStateHandler()
{
	switch (state)
	{
	case 752://upload开始
		send(hCommSock, (char*)&state, sizeof(state), 0);//发送752状态（upload指令）
		break;
	case 753://收到753状态（752状态确认），发送文件名长度
		nameLength = uploadName.GetLength();
		if (UploadOnce((char*)&nameLength, sizeof(nameLength)) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while sending file name length\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		break;
	case 754://收到754状态（753状态确认），发送文件名
		if (UploadOnce(uploadName.GetBuffer(nameLength), uploadName.GetLength()) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while sending file name\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		uploadName.ReleaseBuffer();
		break;
	case 755://收到755状态（754状态确认），发送文件长度
		fileLength = uploadFile.GetLength();//约定文件长度用ULONGLONG存储，长度是8个字节
		leftToSend = fileLength;
		if (UploadOnce((char*)&fileLength, sizeof(fileLength)) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while sending file length\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		break;
	case 756://收到756状态（755状态确认），发送文件（一个chunk）
		if (leftToSend > 0)
		{
			char chunkBuf[CHUNK_SIZE] = { 0 };//#define CHUNK_SIZE 4096
			int readChunkSize = uploadFile.Read(chunkBuf, CHUNK_SIZE);//#define CHUNK_SIZE 4096
			if (UploadOnce(chunkBuf, readChunkSize) == FALSE)
			{
				DWORD errSend = WSAGetLastError();
				TRACE("\nError occurred while sending file chunks\n"
					"\tGetLastError = %d\n", errSend);
				ASSERT(errSend != WSAEWOULDBLOCK);
			}
			leftToSend -= readChunkSize;
		}
		break;
	}
}


BOOL CFileClientDlg::RecvOnce(char* buf, int length)
{
	int leftToRecv = length;
	int bytesRecv = 0;
	int WSAECount = 0;

	do// 单次接收
	{
		char* recvBuf = buf + length - leftToRecv;
		bytesRecv = recv(hCommSock, recvBuf, leftToRecv, 0);
		if (bytesRecv == SOCKET_ERROR)
		{
			ASSERT(WSAGetLastError() == WSAEWOULDBLOCK);
			bytesRecv = 0;
			WSAECount++;
			//if (WSAECount > MAX_WSAE_TIMES) return FALSE;
		}
		leftToRecv -= bytesRecv;
	} while (leftToRecv > 0);

	return TRUE;
}


void CFileClientDlg::DownloadStateHandler()
{
	switch (state)
	{
	case 209://download开始
		send(hCommSock, (char*)&state, sizeof(state), 0);//发送209状态（download指令）
		break;
	case 210://收到210状态（209状态确认），发送要下载的文件名长度
		nameLength = downloadName.GetLength();
		if (UploadOnce((char*)&nameLength, sizeof(nameLength)) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while sending file name length\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		break;
	case 211://收到211状态（210状态确认），发送要下载的文件名
		if (UploadOnce(downloadName.GetBuffer(nameLength), downloadName.GetLength()) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while sending file name\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		downloadName.ReleaseBuffer();
		break;
	case 212://收到212状态（211状态确认），いまだ!!发送方チェンジ!!
		state = 213;
		send(hCommSock, (char*)&state, sizeof(state), 0);//发送213状态（212状态确认）
		break;
	case 213://接收要下载的文件长度
		if (RecvOnce((char*)&fileLength, sizeof(fileLength)) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while receiving file length\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		leftToRecv = fileLength;
		state = 214;
		send(hCommSock, (char*)&state, sizeof(state), 0);//发送214状态（213状态确认）
		break;
	case 214://接收文件（单个chunk）
		char chunkBuf[CHUNK_SIZE] = { 0 };//#define CHUNK_SIZE 4096
		int writeChunkSize = (leftToRecv < CHUNK_SIZE) ? leftToRecv : CHUNK_SIZE;//#define CHUNK_SIZE 4096
		if (RecvOnce(chunkBuf, writeChunkSize) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while receiving file chunks\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		leftToRecv -= writeChunkSize;
		downloadFile.Write(chunkBuf, writeChunkSize);
		if (leftToRecv > 0)
		{
			state = 214;
			send(hCommSock, (char*)&state, sizeof(state), 0);//发送214状态（对接收到上一块文件的确认，请求发送下一块）
		}
		else
		{
			state = 0;
			send(hCommSock, (char*)&state, sizeof(state), 0);//发送0状态（对文件接收完毕的确认）
			downloadFile.Close();
		}
		break;
	}
}


void CFileClientDlg::DeleteStateHandler()
{
	switch (state)
	{
	case 15687://delete开始
		send(hCommSock, (char*)&state, sizeof(state), 0);//发送15687状态（delete指令）
		break;
	case 15688://收到15688状态（15687状态确认），发送要删除的文件名长度
		nameLength = deleteName.GetLength();
		if (UploadOnce((char*)&nameLength, sizeof(nameLength)) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while sending file name length\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		break;
	case 15689://收到15689状态（15688状态确认），发送要删除的文件名
		if (UploadOnce(deleteName.GetBuffer(nameLength), deleteName.GetLength()) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while sending file name\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		deleteName.ReleaseBuffer();
		break;
	}
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
			if (state == 0)
			{
				strLen = recv(hSocket, buf, MAX_BUF_SIZE, 0);
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
			}
			if (state >= 752 && state <= 756)//上传过程，########最优先#########
			{
				recv(hSocket, (char*)&state, sizeof(state), 0);//接收下一状态（前一状态确认）
				if (state == 0)
				{
					uploadFile.Close();
					FileName.AddString(uploadName);
					AfxMessageBox((CString)"上传成功！");
				}
				else
				{
					UploadStateHandler();
				}
			}
			else if (state >= 209 && state <= 211)//下载过程（前半部分，需要传送文件长度和文件名），########次优先#########
			{
				recv(hSocket, (char*)&state, sizeof(state), 0);//接收下一状态（前一状态确认）
				DownloadStateHandler();
			}
			else if (state >= 15687 && state <= 15689)//删除过程，，########再次优先#########
			{
				recv(hSocket, (char*)&state, sizeof(state), 0);//接收下一状态（前一状态确认）
				if (state == 0)
				{
					FileName.DeleteString(FileName.FindStringExact(-1, deleteName));
					AfxMessageBox((CString)"删除成功！");
				}
				else
				{
					DeleteStateHandler();
				}
			}
			else if (state >= 212 && state <= 214)//下载过程（后半部分，客户端向服务器请求文件长度和文件数据）
			{
				DownloadStateHandler();
				if (state == 0)
				{
					AfxMessageBox((CString)"下载成功！");
				}
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
		CString fileAbsPath = fileDlg.GetPathName();
		uploadName = fileDlg.GetFileName();
		if (!(uploadFile.Open(fileAbsPath.GetString(),
			CFile::modeRead | CFile::typeBinary, &errFile)))
		{
			char errOpenFile[256];
			errFile.GetErrorMessage(errOpenFile, 255);
			TRACE("\nError occurred while opening file:\n"
				"\tFile name: %s\n\tCause: %s\n\tm_cause = %d\n\t m_IOsError = %d\n",
				errFile.m_strFileName, errOpenFile, errFile.m_cause, errFile.m_lOsError);
			ASSERT(FALSE);
		}

		state = 752;//upload状态码
		UploadStateHandler();
	}
}


void CFileClientDlg::OnDownload()//下载文件（TODO：暂时不可以下载文件夹）
{
	FileName.GetText(FileName.GetCurSel(), downloadName); //获得想要下载资源名
	if (!downloadName.IsEmpty())
	{
		//弹出另存为对话框
		CString fileExt = downloadName.Right(downloadName.GetLength() - downloadName.ReverseFind('.'));
		char szFilters[32] = { 0 };
		sprintf_s(szFilters, "(*%s)|*%s||", fileExt.GetString(), fileExt.GetString());
		CFileDialog fileDlg(FALSE, NULL, downloadName,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters);

		char desktop[MAX_PATH] = { 0 };
		SHGetSpecialFolderPath(NULL, desktop, CSIDL_DESKTOP, FALSE);
		fileDlg.m_ofn.lpstrInitialDir = desktop;//把默认路径设置为桌面

		if (fileDlg.DoModal() == IDOK)
		{
			CString fileAbsPath = fileDlg.GetPathName();
			if (fileDlg.GetFileExt() == "")
			{
				fileAbsPath += fileExt;
			}
			if (!(downloadFile.Open(fileAbsPath.GetString(),
				CFile::modeCreate | CFile::modeWrite | CFile::typeBinary, &errFile)))
			{
				char errOpenFile[256];
				errFile.GetErrorMessage(errOpenFile, 255);
				TRACE("\nError occurred while opening file:\n"
					"\tFile name: %s\n\tCause: %s\n\tm_cause = %d\n\t m_IOsError = %d\n",
					errFile.m_strFileName, errOpenFile, errFile.m_cause, errFile.m_lOsError);
				ASSERT(FALSE);
			}

			state = 209;//download状态码
			DownloadStateHandler();
		}
	}
}


void CFileClientDlg::OnDelete() // 删除文件
{
	FileName.GetText(FileName.GetCurSel(), deleteName); // 获取用户要删除的文件名
	if (!deleteName.IsEmpty())
	{
		if (AfxMessageBox((CString)"确定要删除这个文件？", 4 + 48) == 6)
		{
			state = 15687;//delete状态码
			DeleteStateHandler();
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