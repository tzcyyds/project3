﻿// CDispalyView.cpp: 实现文件
//

#include "pch.h"
#include "Flieserver.h"
#include "CDisplayView.h"
#include "FlieserverDoc.h"


#define WM_SOCK WM_USER + 100// 自定义消息，为避免冲突，最好100以上
constexpr auto MAX_BUF_SIZE = 128;
#define CHUNK_SIZE 4096
#define MAX_WSAE_TIMES 10// 单次发送或接收过程中所允许出现WSAEWOULDBLOCK的最大次数

// CDispalyView

IMPLEMENT_DYNCREATE(CDisplayView, CFormView)

CDisplayView::CDisplayView()
	: CFormView(IDD_DISPLAYVIEW)
	, m_port(9190)
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
	DDX_Control(pDX, IDC_USEROL, UserName);
	DDX_Text(pDX, IDC_PORT, m_port);
}

BEGIN_MESSAGE_MAP(CDisplayView, CFormView)
	ON_BN_CLICKED(IDC_LISTEN, &CDisplayView::OnBnClickedListen)
	ON_BN_CLICKED(IDC_STOP, &CDisplayView::OnBnClickedStop)
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

LRESULT CDisplayView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	SOCKET hSocket;
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

				//hCommSock进入连接建立状态,等待用户名
				myUser user;
				user.ip = clntAdr.sin_addr;
				user.port = clntAdr.sin_port;
				user.username = "NULL";
				user.state = 1;//
				pDoc->m_linkInfo.myMap.insert(pair<SOCKET, myUser>(hCommSock, user));
				TRACE("wait account");
			}
			break;
		case FD_READ:
			{
				//不提取事件号了！
				//设定状态
				int m_state = pDoc->m_linkInfo.myMap[hSocket].state;
				//switch 根据状态
				switch (m_state)
				{
				case 0://质询出错
					break;
				case 1://等待用户名，并发送质询
					pDoc->fsm_Challenge(hSocket);
					//连接建立状态
					break;
				case 2://等待质询结果
					pDoc->fsm_HandleRes(hSocket);
					//等待质询结果状态
					break;
				case 3:
					//用户已在线，等待其它指令。
					pDoc->MainState_fsm(hSocket);//调用主状态处理函数
					break;
				case 4:
					//在接收上传文件数据状态，接收数据
					pDoc->Recvfile(hSocket);
					break;
				case 5:
					//已收到下载命令，等待开始传输
					pDoc->WaitUpload(hSocket);
					break;
				case 6:
					//等待下载数据确认
					pDoc->WaitAck(hSocket);
					break;
				default:
					break;
				}
			}
			break;
		case FD_CLOSE:
			closesocket(hSocket);
			break;
		}
		break;//中断的是case WM_SOCK:
	}
	return CFormView::WindowProc(message, wParam, lParam);
}

void CDisplayView::OnBnClickedListen()
{
	// TODO: 在此添加控件通知处理程序代码
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
	servAdr.sin_port = htons(m_port);
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
		exit(1);
	}
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
	TRACE("init finish");
}


void CDisplayView::OnBnClickedStop()
{
	// TODO: 在此添加控件通知处理程序代码
}
