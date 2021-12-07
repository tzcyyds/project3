// CDispalyView.cpp: 实现文件
//

#include "pch.h"
#include "Client.h"
#include "CDisplayView.h"
#include "ClientDoc.h"


// CDispalyView
#define WM_SOCK WM_USER + 100// 自定义消息，在WM_USER的基础上进行
#define MAX_WSAE_TIMES 10// 单次发送或接收过程中所允许出现WSAEWOULDBLOCK的最大次数
constexpr auto MAX_BUF_SIZE = 100;


IMPLEMENT_DYNCREATE(CDisplayView, CFormView)

CDisplayView::CDisplayView()
	: CFormView(IDD_DISPLAYVIEW)
	, m_user(_T(""))
	, m_password(_T(""))
	,client_state(0)
	, m_ip(0x7f000001)
	, m_SPort(9190)
	, m_LPort(9191)
{
	hCommSock = 0;
	memset(&servAdr, 0, sizeof(servAdr));

}

CDisplayView::~CDisplayView()
{
}

void CDisplayView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_user);
	DDX_Text(pDX, IDC_EDIT3, m_password);
	DDX_Control(pDX, IDC_LIST1, FileName);
	DDX_Control(pDX, IDC_IPADDRESS1, ServerIP);
	DDX_IPAddress(pDX, IDC_IPADDRESS1, m_ip);
	DDX_Text(pDX, IDC_EDIT4, m_SPort);
	DDX_Text(pDX, IDC_EDIT5, m_LPort);
}

BEGIN_MESSAGE_MAP(CDisplayView, CFormView)
	ON_BN_CLICKED(IDC_CONNECT, &CDisplayView::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, &CDisplayView::OnBnClickedDisconnect)
	ON_BN_CLICKED(IDC_ENTERDIR, &CDisplayView::OnBnClickedEnterdir)
	ON_BN_CLICKED(IDC_GOBACK, &CDisplayView::OnBnClickedGoback)
	ON_BN_CLICKED(IDC_UPLOAD, &CDisplayView::OnBnClickedUpload)
	ON_BN_CLICKED(IDC_DOWNLOAD, &CDisplayView::OnBnClickedDownload)
	ON_BN_CLICKED(IDC_DELETE, &CDisplayView::OnBnClickedDelete)
END_MESSAGE_MAP()


// CDispalyView 诊断

#ifdef _DEBUG
void CDisplayView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDisplayView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDispalyView 消息处理程序







LRESULT CDisplayView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	SOCKET hSocket;
	int newEvent;
	CClientDoc* pDoc = (CClientDoc*)GetDocument();
	switch (message)
	{
	case WM_SOCK:
		hSocket = (SOCKET)LOWORD(wParam);
		newEvent = LOWORD(lParam);
		switch (newEvent)
		{
		case FD_READ:
			switch (client_state)
			{
			case 1://等待质询
				pDoc->socket_state1_fsm(hSocket);
				break;
			case 2://等待认证结果
				pDoc->socket_state2_fsm(hSocket);
				break;
			case 3://认证成功了！此时可以交互文件夹及文件相关信息
				//解析报文，提取事件号，调用不同处理函数
				pDoc->socket_state3_fsm(hSocket);
				break;
			case 4://等待上传确认状态
				pDoc->socket_state4_fsm(hSocket);
				break;
			case 5://等待上传数据确认状态
				pDoc->socket_state5_fsm(hSocket);
				break;
			case 6://等待下载确认状态
				pDoc->socket_state6_fsm(hSocket);
				break;
			case 7://等待下载数据状态
				pDoc->socket_state7_fsm(hSocket);
				break;
			case 8://等待目录删除结果状态
				pDoc->socket_state8_fsm(hSocket);
				break;
			case 9://等待目录列表状态
				pDoc->socket_state9_fsm(hSocket);
				break;
			default:
				break;
			}
			break;
		case FD_CLOSE:
			closesocket(hSocket);
			MessageBox("Connection closed", "Client", MB_OK);
			break;
		}
		break;
	}
	return CFormView::WindowProc(message, wParam, lParam);
}



void CDisplayView::OnBnClickedConnect()
{
	
	CClientDoc* pDoc = (CClientDoc*)GetDocument();
	char sendbuf[MAX_BUF_SIZE] = { 0 };

	// 判断异常情况
	if (m_ip == NULL)
	{
		AfxMessageBox((CString)"IP地址为空！");
		return;
	}
	else if (m_SPort == NULL)
	{
		AfxMessageBox((CString)"云端端口为空！");
		return;
	}
	else if (m_LPort == NULL)
	{
		AfxMessageBox((CString)"本地端口为空！");
		return;
	}

	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(m_ip);
	servAdr.sin_port = htons(m_SPort);

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		MessageBox("WSAStartup() failed", "Client", MB_OK);
		exit(1);
	}
	hCommSock = socket(AF_INET, SOCK_STREAM, 0);
	if (hCommSock == INVALID_SOCKET)
	{
		MessageBox("socket() failed", "Client", MB_OK);
		exit(1);
	}


	if (connect(hCommSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)//隐式绑定，连接服务器
	{
		MessageBox("connect() failed", "Client", MB_OK);
		exit(1);
	}
	
	if (WSAAsyncSelect(hCommSock, m_hWnd, WM_SOCK, FD_READ | FD_CLOSE) == SOCKET_ERROR)
	{
		MessageBox("WSAAsyncSelect() failed", "Client", MB_OK);
		exit(1);
	}

	//发送用户名报文
	sendbuf[0] = 1;//填写事件号
	UpdateData(TRUE);//刷新用户名和密码
	int strLen = m_user.GetLength();
	sendbuf[3] = strLen % 256;//填写字符串长度（用户名字符串长度需要小于256）
	memcpy(sendbuf + 4, m_user, strLen);//填写用户名字符串
	char* temp = &sendbuf[1];
	*(u_short*)temp = htons((4 + strLen));
	send(hCommSock, sendbuf, strLen + 4, 0);
	TRACE("send account");
	client_state = 1;//连接成功,已发送用户名，等待质询



}


void CDisplayView::OnBnClickedDisconnect()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CDisplayView::OnBnClickedEnterdir()
{
	// TODO: 在此添加控件通知处理程序代码
	CString selFile;

	FileName.GetText(FileName.GetCurSel(), selFile); //获取用户选择的目录名

	if (selFile.Find('.') == -1) // 判断是否为文件夹，原理：文件名有'.'
	{
		m_send = selFile + "\\*";
		strdirpath = m_send; // 本地保存当前的文件夹路径，在返回上一级文件夹时会使用到
		int strLen = m_send.GetLength();
		if (UploadOnce(m_send, strLen) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while sending file name\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		client_state = 9;
	}
}


void CDisplayView::OnBnClickedGoback()
{
	// TODO: 在此添加控件通知处理程序代码
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
		if (UploadOnce(strdirpath, strLen) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while sending file name\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		client_state = 9;
	}
}


void CDisplayView::OnBnClickedUpload()
{
	// TODO: 在此添加控件通知处理程序代码
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

		char sendbuf[MAX_BUF_SIZE] = { 0 };
		char* temp = sendbuf;
		sendbuf[0] = 15;
		temp = &sendbuf[3];
		nameLength = uploadName.GetLength();
		*(u_short*)temp = ntohs((u_short)nameLength);
		memcpy(sendbuf+5, uploadName.GetBuffer(nameLength), nameLength);
		temp = &sendbuf[(5 + nameLength)];
		fileLength = uploadFile.GetLength();
		*(u_long*)temp = ntohl((u_long)fileLength);
		temp = &sendbuf[1];
		*(u_short*)temp = ntohs((u_short)(9 + nameLength));
		leftToSend = fileLength;
		if (UploadOnce(sendbuf, 9 + nameLength) == FALSE)
		{
			DWORD errSend = WSAGetLastError();
			TRACE("\nError occurred while sending file chunks\n"
				"\tGetLastError = %d\n", errSend);
			ASSERT(errSend != WSAEWOULDBLOCK);
		}
		uploadName.ReleaseBuffer();
		client_state = 4;//变为等待上传确认状态
	}
}

BOOL CDisplayView::UploadOnce(const char* buf, int length)
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

BOOL CDisplayView::RecvOnce(char* buf, int length)
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
			if (WSAECount > MAX_WSAE_TIMES) return FALSE;
		}
		leftToRecv -= bytesRecv;
	} while (leftToRecv > 0);

	return TRUE;
}

void CDisplayView::OnBnClickedDownload()
{
	// TODO: 在此添加控件通知处理程序代码
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
			nameLength = downloadName.GetLength();
			char sendbuf[MAX_BUF_SIZE] = { 0 };
			char* temp = sendbuf;
			*(char*)temp = 11;
			temp = temp + 1;
			*(u_short*)temp = htons((u_short)(5 + nameLength));
			temp = temp + 2;
			*(u_short*)temp = htons((u_short)nameLength);
			temp = temp + 2;
			memcpy(temp, downloadName.GetBuffer(nameLength), nameLength);
			if (UploadOnce(sendbuf, 5 + nameLength) == FALSE)
			{
				DWORD errSend = WSAGetLastError();
				TRACE("\nError occurred while sending file chunks\n"
					"\tGetLastError = %d\n", errSend);
				ASSERT(errSend != WSAEWOULDBLOCK);
			}
			downloadName.ReleaseBuffer();
			client_state = 6;//变为等待下载确认状态

		}
	}
}


void CDisplayView::OnBnClickedDelete()
{
	// TODO: 在此添加控件通知处理程序代码
	char sendbuf[MAX_BUF_SIZE] = { 0 };
	char* temp = sendbuf;
	FileName.GetText(FileName.GetCurSel(), deleteName); // 获取用户要删除的文件名
	if (!deleteName.IsEmpty())
	{
		if (AfxMessageBox((CString)"确定要删除这个文件？", 4 + 48) == 6)
		{
			nameLength = deleteName.GetLength();
			*(char*)temp = 19;
			temp = temp + 1;
			*(u_short*)temp = htons((5 + nameLength));
			temp = temp + 2;
			*(u_short*)temp = htons(nameLength);
			temp = temp + 2;
			memcpy(temp, deleteName.GetBuffer(nameLength), nameLength);
			if (UploadOnce(sendbuf, (5 + nameLength)) == FALSE)
			{
				DWORD errSend = WSAGetLastError();
				TRACE("\nError occurred while sending file name\n"
					"\tGetLastError = %d\n", errSend);
				ASSERT(errSend != WSAEWOULDBLOCK);
			}
			deleteName.ReleaseBuffer();

			client_state = 8;//变为等待删除结果状态
		}
	}
}

void CDisplayView::UpdateDir(CString recv)  // 更新列表显示的文件目录
{
	FileName.ResetContent();

	std::vector<std::string> v;
	std::string strStr = recv.GetBuffer(0);
	SplitString(strStr, v, "|"); //可按多个字符来分隔;

	for (auto i = 0; i != v.size(); ++i)
		FileName.AddString(v[i].c_str());
}


void CDisplayView::SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c) // 字符串分割函数
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
