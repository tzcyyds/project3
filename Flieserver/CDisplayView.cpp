// CDispalyView.cpp: 实现文件
//

#include "pch.h"
#include "Flieserver.h"
#include "CDisplayView.h"

#include "FlieserverDoc.h"


#define WM_SOCK WM_USER + 100// 自定义消息，为避免冲突，最好100以上
#define MAX_BUF_SIZE 128

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

/*	clntAdr.sin_family = AF_INET;
	clntAdr.sin_addr.s_addr = htonl(0x7f000001);
	clntAdr.sin_port = htons(9191);*/

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
			hCommSock = accept(hSocket, (sockaddr*)&clntAdr, &clntAdrLen);
			if (hCommSock == SOCKET_ERROR)
			{
				closesocket(hSocket);
				break;
			}
			//连接成功后，立即发送质询消息
			
			pDoc->m_UserWait.myMap.insert(pair<SOCKET, string>(hCommSock, "test"));

			break;
		case FD_READ:
			/*strLen = recv(hSocket, buf, MAX_BUF_SIZE, 0);
			if (strLen <= 0);
			else;
			*/
			//下面要做数据的分类处理，chap认证数据，文件传输命令，or日常聊天数据。报文格式要设计好
			if (pDoc->m_UserOL.myMap.count(hSocket))//如果认证已通过，就处理消息
			{

			}
			else//如果还未通过，就只能接收认证报文
			{

			}
			break;
		case FD_CLOSE:
			closesocket(hSocket);
			break;
		}
		break;
	}
	return CFormView::WindowProc(message, wParam, lParam);
}