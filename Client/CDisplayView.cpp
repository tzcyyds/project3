// CDispalyView.cpp: 实现文件
//

#include "pch.h"
#include "Client.h"
#include "CDisplayView.h"


// CDispalyView
#define WM_SOCK WM_USER + 1// 自定义消息，在WM_USER的基础上进行
#define MAX_BUF_SIZE 100


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
	ON_EN_CHANGE(IDC_EDIT2, &CDisplayView::OnEnChangeEdit2)
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
SOCKET client_socket;
SOCKET hSocket;
SOCKADDR_IN client_addr;
int client_state = 0;
char password[MAX_BUF_SIZE] = { 0 };
int password_strLen = 0;
char sendbuf[MAX_BUF_SIZE] = { 0 };
char recvbuf[MAX_BUF_SIZE] = { 0 };

LRESULT CDisplayView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	int newEvent;
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
			case 1:
				socket_state1_fsm(hSocket);
				break;
			case 2:
				socket_state2_fsm(hSocket);
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

	client_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (client_socket == INVALID_SOCKET)
	{
		MessageBox("socket() failed", "Client", MB_OK);
		exit(1);
	}
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	client_addr.sin_port = htons(9191);//用的默认端口9191，可以更改

	if (connect(client_socket, (SOCKADDR*)&client_addr, sizeof(client_addr)) == SOCKET_ERROR)
	{
		MessageBox("connect() failed", "Client", MB_OK);
		exit(1);
	}

	memset(sendbuf, 0, sizeof(sendbuf));
	sendbuf[0] = 1;//填写事件号
	UpdateData(TRUE);
	int strLen = m_user.GetLength();
	sendbuf[1] = strLen%256;//填写字符串长度（用户名字符串长度需要小于256）
	memcpy(sendbuf + 2, m_user, strLen);//填写用户名字符串
	send(client_socket, sendbuf, strLen+2, 0);
	m_user.Empty();

	memset(password, 0, sizeof(password));
	password_strLen = m_password.GetLength();
	memcpy(password, m_password, password_strLen);
	password[password_strLen] = '\0';
	m_password.Empty();

	UpdateData(FALSE);
	client_state = 1;//连接并且发送用户名成功，套接字进入连接建立状态

	if (WSAAsyncSelect(client_socket, m_hWnd, WM_SOCK, FD_READ | FD_CLOSE) == SOCKET_ERROR)
	{
		MessageBox("WSAAsyncSelect() failed", "Client", MB_OK);
		exit(1);
	}
}


void CDisplayView::OnBnClickedButton2()//断开
{
	// TODO: 在此添加控件通知处理程序代码
}


void CDisplayView::OnEnChangeEdit2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CFormView::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

int socket_state1_fsm(SOCKET s)
{
	int strLen;
	u_int password_value;
	int password_strLen;
	int cnt = 0;
	int num_N = 0;
	char* temp = recvbuf;
	u_short correct_result = 0;
	u_short correct_password = 0;
	u_short correct_sum = 0;
	memset(recvbuf, 0, sizeof(recvbuf));
	strLen = recv(s, recvbuf, MAX_BUF_SIZE, 0);
	if (strLen <= 0)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			closesocket(s);
			return -1;
		}
	}
	else
	{
		switch (recvbuf[0])
		{
		case 2:
			num_N = recvbuf[1];
			if ((num_N >= 0)&&(num_N <= 50))
			{
				temp = temp + 2;
				cnt = strLen - 2;
				while (cnt > 0)
				{
					correct_sum += ntohs(*(u_short*)temp);;
					temp = temp + 2;
					cnt -= 2;
				}
				if ((password_strLen > 0) && (password_strLen <= 5))
				{
					password_value = (u_int)strtoul(password, NULL, 0);
					correct_password = password_value % 65536;
					correct_result = correct_sum ^ correct_password;

					memset(sendbuf, 0, sizeof(sendbuf));
					sendbuf[0] = 3;//填写事件号
					temp = sendbuf + 1;
					*temp = htons(correct_result);
					send(client_socket, sendbuf, 2, 0);

					client_state = 2;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
			break;
		default:
			break;
		}
	}
}

int socket_state2_fsm(SOCKET s)
{
	int strLen;
	int cnt = 0;
	memset(recvbuf, 0, sizeof(recvbuf));
	strLen = recv(s, recvbuf, MAX_BUF_SIZE, 0);
	if (strLen <= 0)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			closesocket(s);
			return -1;
		}
	}
	else
	{
		switch (recvbuf[0])
		{
		case 4:
			cnt = recvbuf[1];
			if (cnt == 1)
			{
				client_state = 3;//认证成功，进入等待操作状态
			}
			else
			{
				break;
			}
			break;
		default:
			break;
		}
	}
}