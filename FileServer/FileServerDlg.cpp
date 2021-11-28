
// FileServerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "FileServer.h"
#include "FileServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_SOCK WM_USER + 1 //自定义消息，在WM_USER的基础上进行
#define MAX_BUF_SIZE 128

// CFileServerDlg 对话框


CFileServerDlg::CFileServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILESERVER_DIALOG, pParent)
	, m_port_server(9190)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	hCommSock = 0;
	strdirpath = "..\\m_filepath\\"; //初始化默认路径
	memset(&clntAdr, 0, sizeof(clntAdr));
	clntAdrLen = sizeof(clntAdr);

	state = 0;
}

void CFileServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	// 控制类
	DDX_Control(pDX, IDC_PORT, ServerPort);
	DDX_Control(pDX, IDC_LISTEN, ServerListen);
	DDX_Control(pDX, IDC_STOPLISTEN, ServerStop);
	DDX_Control(pDX, IDC_LIST1, UserName);

	// 变量类
	DDX_Text(pDX, IDC_PORT, m_port_server);
}

BEGIN_MESSAGE_MAP(CFileServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LISTEN, &CFileServerDlg::OnListen)
END_MESSAGE_MAP()


// CFileServerDlg 消息处理程序

BOOL CFileServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFileServerDlg::OnPaint()
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
HCURSOR CFileServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFileServerDlg::OnListen() // 监听
{
	ServerPort.EnableWindow(FALSE);
	
	WSADATA wsaData;
	SOCKADDR_IN servAdr;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		MessageBox("WSAStartup() failed", "Server", MB_OK);
		return;
	}
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(m_port_server);

	SOCKET hListenSock;
	hListenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (hListenSock == INVALID_SOCKET)
	{
		MessageBox("socket() failed", "Server", MB_OK);
		return;
	}
	if (bind(hListenSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
	{
		MessageBox("bind() failed", "Server", MB_OK);
		return;
	}
	if (listen(hListenSock, 5) == SOCKET_ERROR)
	{
		MessageBox("listen() failed", "Server", MB_OK);
		return;
	}
	if (WSAAsyncSelect(hListenSock, m_hWnd, WM_SOCK, FD_ACCEPT | FD_READ | FD_CLOSE) == SOCKET_ERROR)
	{
		MessageBox("WSAAsyncSelect() failed", "Server", MB_OK);
		return;
	}
}

BOOL CFileServerDlg::RecvOnce(char* buf, int length)
{
	int leftToRecv = length;
	int bytesRecv = 0;

	do//单次接收
	{
		char* recvBuf = buf + length - leftToRecv;
		bytesRecv = recv(hCommSock, recvBuf, leftToRecv, 0);
		if (bytesRecv == SOCKET_ERROR) return FALSE;
		leftToRecv -= bytesRecv;
	} while (leftToRecv > 0);

	return TRUE;
}

void CFileServerDlg::UploadStateHandler()
{
	switch (state)
	{
	case 752://收到upload指令
		state = 753;
		send(hCommSock, (char*)&state, sizeof(state), 0);//发送753状态（752状态确认）
		break;
	case 753://接收文件名长度
		if (RecvOnce((char*)&nameLength, sizeof(nameLength)) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while receiving file name length\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		state = 754;
		send(hCommSock, (char*)&state, sizeof(state), 0);//发送754状态（753状态确认）
		break;
	case 754://接收文件名
		if (RecvOnce(uploadName.GetBuffer(nameLength), nameLength) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while receiving file name\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		uploadName.ReleaseBuffer();
		if (!(uploadFile.Open(strdirpath + uploadName,
			CFile::modeCreate | CFile::modeWrite | CFile::typeBinary, &errFile)))
		{
			char errOpenFile[256];
			errFile.GetErrorMessage(errOpenFile, 255);
			TRACE("\nError occurred while receiving file length:\n"
				"\tFile name: %s\n\tCause: %s\n\tm_cause = %d\n\t m_IOsError = %d\n",
				errFile.m_strFileName, errOpenFile, errFile.m_cause, errFile.m_lOsError);
			ASSERT(TRUE);
		}
		state = 755;
		send(hCommSock, (char*)&state, sizeof(state), 0);//发送755状态（754状态确认）
		break;
	case 755://接收文件长度
		if (RecvOnce((char*)&fileLength, sizeof(fileLength)) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while receiving file chunk\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		leftToRecv = fileLength;
		state = 756;
		send(hCommSock, (char*)&state, sizeof(state), 0);//发送756状态（755状态确认）
		break;
	case 756://接收文件（单个chunk）
#define CHUNK_SIZE 4096
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
		uploadFile.Write(chunkBuf, writeChunkSize);
		if (leftToRecv > 0)
		{
			state = 756;
			send(hCommSock, (char*)&state, sizeof(state), 0);//发送756状态（对接收到上一块文件的确认，请求发送下一块）
		}
		else
		{
			state = 0;
			send(hCommSock, (char*)&state, sizeof(state), 0);//发送0状态（对文件接收完毕的确认）
		}
		break;
	}
}

LRESULT CFileServerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
		case FD_ACCEPT:
			hCommSock = accept(hSocket, (sockaddr*)&clntAdr, &clntAdrLen);
			if (hCommSock == SOCKET_ERROR)
			{
				closesocket(hSocket);
				break;
			}
			UserName.AddString(inet_ntoa(clntAdr.sin_addr)); // 添加在线用户的IP
			// 发送默认目录下的文件列表给连接成功的客户端
			m_send = PathtoList(strdirpath + '*');
			strLen = m_send.GetLength();
			send(hCommSock, m_send, strLen, 0);
			break;
		case FD_READ:
			if (state == 0)
			{
				strLen = recv(hSocket, buf, MAX_BUF_SIZE, 0);
				if (*(DWORD*)buf == 752) state = 752;//接收到upload命令（752状态）
				if (strLen <= 0)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						closesocket(hSocket);
						MessageBox("recv() failed", "Server", MB_OK);
						break;
					}
				}
				else
				{
					CString m_recv(buf);
					if (m_recv.Find("m_filepath") != -1) // 判断如果是默认目录，则不能返回上一级
					{
						m_send = PathtoList(m_recv); // 发送该目录下的文件列表给客户端
						strLen = m_send.GetLength();
						send(hCommSock, m_send, strLen, 0);
						strdirpath = m_recv.Left(m_recv.GetLength() - 1);// 让服务器用strdirpath记住用户正在看的目录（TODO：）
					}
				}
			}
			if (state >= 752 && state <= 756)
			{
				UploadStateHandler();
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

CString CFileServerDlg::PathtoList(CString path) // 获取指定目录下的文件列表，文件之间用|隔开
{
	CString file_list; // 文件列表
	CFileFind file_find; // 创建一个CFileFind实例

	BOOL bfind = file_find.FindFile(path);
	while (bfind)
	{
		bfind = file_find.FindNextFile(); // FindNextFile()必须放在循环中的最前面
		CString strpath;
		if (file_find.IsDots())
			continue;
		if (!file_find.IsDirectory())          //判断是目录还是文件
		{
			strpath = file_find.GetFileName(); //文件则读取文件名
			file_list += strpath;
			file_list += '|';
		}
		else
		{
			strpath = file_find.GetFilePath(); //获取到的是绝对路径
			file_list += strpath;
			file_list += '|';
		}

	}
	return file_list;
}
