// CDispalyView.cpp: 实现文件
//

#include "pch.h"
#include "Flieserver.h"
#include "CDisplayView.h"

#include "FlieserverDoc.h"


#define WM_SOCK WM_USER + 100// 自定义消息，为避免冲突，最好100以上
constexpr auto MAX_BUF_SIZE = 128;

// CDispalyView

IMPLEMENT_DYNCREATE(CDisplayView, CFormView)

CDisplayView::CDisplayView()
	: CFormView(IDD_DISPLAYVIEW)
{
	hCommSock = 0;
	memset(&clntAdr, 0, sizeof(clntAdr));
	clntAdrLen = sizeof(clntAdr);
}

CDisplayView::~CDisplayView()
{
}

void CDisplayView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
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


void CDisplayView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
}


void CDisplayView::OnBnClickedButton1()//启动
{

	//clntAdr.sin_family = AF_INET;
	//clntAdr.sin_addr.s_addr = htonl(0x7f000001);
	//clntAdr.sin_port = htons(9191);

	WSADATA wsaData;
	SOCKADDR_IN servAdr;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		MessageBox("WSAStartup() failed", "Server", MB_OK);
		exit(1);
	}
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(9190);
	SOCKET hListenSock;
	hListenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (hListenSock == INVALID_SOCKET)
	{
			MessageBox("socket() failed", "Server", MB_OK);
			exit(1);
	}
	if (bind(hListenSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
	{
		MessageBox("bind() failed", "Server", MB_OK);
		exit(1);		}
	if (listen(hListenSock, 5) == SOCKET_ERROR)
	{
		MessageBox("listen() failed", "Server", MB_OK);
		exit(1);
	}
	if (WSAAsyncSelect(hListenSock, m_hWnd, WM_SOCK, FD_ACCEPT | FD_READ | FD_CLOSE) == SOCKET_ERROR)
	{
		MessageBox("WSAAsyncSelect() failed", "Server", MB_OK);
		exit(1);
	}

}


void CDisplayView::OnBnClickedButton2()	//停止
{
	// TODO: 在此添加控件通知处理程序代码
}


LRESULT CDisplayView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	SOCKET hSocket;
	char buf[MAX_BUF_SIZE] = { 0 };
	int strLen;
	int newEvent;
	CFlieserverDoc* pDoc = (CFlieserverDoc*)GetDocument();
	switch (message)
	{
	case WM_SOCK:
		hSocket = (SOCKET)LOWORD(wParam);
		newEvent = LOWORD(lParam);
		switch (newEvent)
		{
		case FD_ACCEPT:
		{
			hCommSock = accept(hSocket, (sockaddr*)&clntAdr, &clntAdrLen);
			if (hCommSock == SOCKET_ERROR)
			{
				closesocket(hSocket);
				break;
			}

			//hCommSock进入连接建立状态
			pDoc->m_WaitAcc.myMap.insert(pair<SOCKET, string>(hCommSock, "test"));
			myUser user;
			user.ip = clntAdr.sin_addr.S_un.S_addr;
			user.port = clntAdr.sin_port;
			user.username = "NULL";
			pDoc->m_linkInfo.myMap.insert(pair<SOCKET, myUser>(hCommSock, user));

			break;
		}
		case FD_READ:
		{
			//首先接收数据
			strLen = recv(hSocket, buf, MAX_BUF_SIZE, 0);
			if (strLen <= 0)
			{

				break;
			}
			int event = 0;
			//数据的分类处理，提取事件号
			if ((int)buf[0] == 1)
			{
				event = 1;//用户名到来
			}
			else if ((int)buf[0] == 3)
			{
				event = 2;//质询结果到来
			}
			else//非法数据
			{

			}
			//设定状态
			int state = 0;
			if (pDoc->m_WaitAcc.myMap.count(hSocket))
			{
				state = 1;//已连接，正在等待用户名
			}
			else if (pDoc->m_WaitAns.myMap.count(hSocket))
			{
				state = 2;//用户名存在，正在认证密码，等待回应
			}
			else if (pDoc->m_UserOL.myMap.count(hSocket))
			{
				state = 3;//用户已在线，等待其它类型数据
			}
			else if (0)
			{

			}
			else if (0)
			{

			}
			else;//质询出错
			//switch 根据状态
			switch (state)
			{
			case 0://质询出错
				break;
			case 1://等待用户名，并发送质询
				pDoc->fsm_Challenge(hSocket, event, buf, strLen);
				//连接建立状态，调函数，传hSocket，事件号，报文。
				break;
			case 2://等待质询结果
				pDoc->fsm_HandleRes(hSocket, event, buf, strLen);
				//等待质询结果状态，调函数，传参。
				break;
			case 3:
				//用户已在线，等待其它指令。
				break;
			case 4:

				break;
			default:

				break;
			}
			break;
		}
		case FD_CLOSE:
			closesocket(hSocket);
			break;
		}
		break;
	}
	return CFormView::WindowProc(message, wParam, lParam);
}