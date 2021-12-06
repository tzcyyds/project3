// CDispalyView.cpp: 实现文件
//

#include "pch.h"
#include "Client.h"
#include "CDisplayView.h"
#include "ClientDoc.h"


// CDispalyView
#define WM_SOCK WM_USER + 100// 自定义消息，在WM_USER的基础上进行
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
	sendbuf[1] = strLen % 256;//填写字符串长度（用户名字符串长度需要小于256）
	memcpy(sendbuf + 2, m_user, strLen);//填写用户名字符串
	send(hCommSock, sendbuf, strLen + 2, 0);
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
}


void CDisplayView::OnBnClickedGoback()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CDisplayView::OnBnClickedUpload()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CDisplayView::OnBnClickedDownload()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CDisplayView::OnBnClickedDelete()
{
	// TODO: 在此添加控件通知处理程序代码
}
