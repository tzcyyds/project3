// CDispalyView.cpp: 实现文件
//

#include "pch.h"
#include "Client.h"
#include "CDisplayView.h"
#include "ClientDoc.h"


// CDispalyView
#define WM_SOCK WM_USER + 1// 自定义消息，在WM_USER的基础上进行
constexpr auto MAX_BUF_SIZE = 100;


IMPLEMENT_DYNCREATE(CDisplayView, CFormView)

CDisplayView::CDisplayView()
	: CFormView(IDD_DISPLAYVIEW)
	, m_user(_T(""))
	, m_password(_T(""))
{

}

CDisplayView::~CDisplayView()
{
}

void CDisplayView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, m_user);
	DDX_Text(pDX, IDC_EDIT3, m_password);
}

BEGIN_MESSAGE_MAP(CDisplayView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CDisplayView::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDisplayView::OnBnClickedButton2)
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
			switch (pDoc->client_state)
			{
			case 1://等待质询
				pDoc->socket_state1_fsm(hSocket);
				break;
			case 2://等待认证结果
				pDoc->socket_state2_fsm(hSocket);
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




void CDisplayView::OnBnClickedButton1()//连接
{
	// TODO: 在此添加控件通知处理程序代码
	CClientDoc* pDoc = (CClientDoc*)GetDocument();
	char sendbuf[MAX_BUF_SIZE] = { 0 };

	SOCKET hCommSock;
	SOCKADDR_IN servAdr;
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(0x7f000001);//127.0.0.1
	servAdr.sin_port = htons(9190);//服务器端口9190

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		MessageBox("WSAStartup() failed", "Client", MB_OK);
		exit(1);
	}
	hCommSock = socket(AF_INET, SOCK_DGRAM, 0);
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
	send(hCommSock, sendbuf, strLen+2, 0);

	pDoc->client_state = 1;//连接并且发送用户名成功，套接字进入连接建立状态


}


void CDisplayView::OnBnClickedButton2()//断开
{
	// TODO: 在此添加控件通知处理程序代码
}



