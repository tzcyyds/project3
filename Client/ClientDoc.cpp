
// ClientDoc.cpp: CClientDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Client.h"
#endif

#include "ClientDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "CDisplayView.h"

constexpr auto MAX_BUF_SIZE = 100;
// CClientDoc

IMPLEMENT_DYNCREATE(CClientDoc, CDocument)

BEGIN_MESSAGE_MAP(CClientDoc, CDocument)
END_MESSAGE_MAP()


// CClientDoc 构造/析构

CClientDoc::CClientDoc() noexcept
{
	// TODO: 在此添加一次性构造代码
	
}

CClientDoc::~CClientDoc()
{
}


void CClientDoc::socket_state1_fsm(SOCKET s)
{
	char sendbuf[MAX_BUF_SIZE] = { 0 };
	char recvbuf[MAX_BUF_SIZE] = { 0 };
	CDisplayView* pView;
	POSITION pos = GetFirstViewPosition();
	pView = (CDisplayView*)GetNextView(pos);

	
	int strLen = recv(s, recvbuf, MAX_BUF_SIZE, 0);
	if (strLen <= 0)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			closesocket(s);
			return;
		}
	}
	else
	{
		u_int event= recvbuf[0];
		//提取事件号
		switch (event)
		{
		case 2://收到质询报文
		{
			u_int num_N = recvbuf[1];
			char* temp = &recvbuf[2];

			u_int password_value = 0;
			u_int password_len = pView->m_password.GetLength();//只有点击连接时，才会刷新用户名和密码，此时一定可以获取到上次正确的密码
			u_short correct_result = 0;
			u_short correct_password = 0;
			u_short correct_sum = 0;

			if (num_N <= 20)//整数个数小于20,u_int一定大于0
			{
				for (size_t i = 0; i < num_N; i++)
				{
					//这两行待修改
					correct_sum += ntohs(*(u_short*)temp);;
					temp = temp + 2;
				}

				if ((password_len > 0) && (password_len <= 5))//两字节，最大65535
				{
					password_value = (u_int)_ttoi(pView->m_password);
					correct_password = (u_short)(password_value % 65536);
					correct_result = correct_sum ^ correct_password;

					
					sendbuf[0] = 3;//填写事件号
					temp = &sendbuf[1];
					*(u_short*)temp = htons(correct_result);//写入赋值，挺复杂的写法
					send(s, sendbuf, 4, 0);
					TRACE("respond challenge");
					pView->client_state = 2;//状态转换，已返回质询结果，等待确认
				}
				else//密码长度不对
				{
					break;
				}
			}
			else//整数个数不对
			{
				break;
			}
		}
			break;
		default:
			break;
		}
	}
}

void CClientDoc::socket_state2_fsm(SOCKET s)
{
	char recvbuf[MAX_BUF_SIZE] = { 0 };
	int temp = 0;
	CDisplayView* pView;
	POSITION pos = GetFirstViewPosition();
	pView = (CDisplayView*)GetNextView(pos);
	int strLen = recv(s, recvbuf, MAX_BUF_SIZE, 0);
	if (strLen <= 0)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			closesocket(s);
			return;
		}
	}
	else
	{
		int event = recvbuf[0];
		switch (event)
		{
		case 4://认证结果报文
			temp = recvbuf[1];
			if (temp == 1)//认证成功
			{
				pView->client_state = 3;//认证成功，进入等待操作状态
				TRACE("认证成功");
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

BOOL CClientDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CClientDoc 序列化

void CClientDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CClientDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CClientDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CClientDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CClientDoc 诊断

#ifdef _DEBUG
void CClientDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CClientDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CClientDoc 命令
