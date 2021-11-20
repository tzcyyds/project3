
// FlieserverDoc.cpp: CFlieserverDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Flieserver.h"
#endif

#include "FlieserverDoc.h"
#include <propkey.h>
#include <sstream>
//#include <cstdlib>
constexpr auto MAX_BUF_SIZE = 128;

using namespace std;
stringstream sstream;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFlieserverDoc

IMPLEMENT_DYNCREATE(CFlieserverDoc, CDocument)

BEGIN_MESSAGE_MAP(CFlieserverDoc, CDocument)
END_MESSAGE_MAP()


// CFlieserverDoc 构造/析构

CFlieserverDoc::CFlieserverDoc() noexcept
{
	// TODO: 在此添加一次性构造代码
	//CFlieserverDoc::BroseAllFiles("..\\m_filepath");
	SetTitle(TEXT("fileserver"));
	m_UserInfo.initDoc();//本地用户信息
	m_linkInfo.myMap.clear();//连接-用户名信息
	m_WaitAcc.myMap.clear();//等待回应
	m_WaitAns.myMap.clear();//等待回应
	m_UserOL.myMap.clear();//授权后，正式在线

}

CFlieserverDoc::~CFlieserverDoc()
{
}

void CFlieserverDoc::BrowseAllFiles(CString filepath, CTreeCtrl* treeCtrl)
{
	//检测路径是否正确并添加必要信息
	if (filepath == _T(""))
	{
		return;
	}
	else
	{
		if (filepath.Right(1) != _T(""))
		{
			filepath += _T("\\");
		}
		filepath += _T("*.*");
	}

	//递归枚举文件夹下的内容
	CFileFind find;
	CString strpath;
	CString str_fileName;
	BOOL IsFind = find.FindFile(filepath);
	CTreeCtrl* m_TreeCtrl = treeCtrl;
	while (IsFind)
	{
		IsFind = find.FindNextFile();
		strpath = find.GetFilePath();

		if (!find.IsDirectory() && !find.IsDots())
		{
			str_fileName = find.GetFileName();
			m_TreeCtrl->InsertItem((LPCSTR)str_fileName);
			//AfxMessageBox(str_fileName);
		}
		else if (find.IsDirectory() && !find.IsDots())
		{
			str_fileName = find.GetFileName();
			m_TreeCtrl->InsertItem((LPCSTR)str_fileName);
			//AfxMessageBox(str_fileName);

			BrowseAllFiles(strpath, m_TreeCtrl);
		}
		else
		{
			continue;
		}
	}
}

void CFlieserverDoc::fsm_Challenge(SOCKET hSocket, int event, char* buf, int strlen)
{
	char sendbuf[MAX_BUF_SIZE] = { 0 };
	char* temp = sendbuf;
	//函数：拿到用户名，查用户名对应的密码，随机出随机数，和密钥异或，存起来，然后发送随机数。最后，更改状态
	switch (event)
	{
	case 0://非法数据
		break;
	case 1://用户名到来
	{
		int namelen = buf[1];
		assert(namelen >= 0 && namelen <= MAX_BUF_SIZE);
		string username(&buf[2], namelen);
		if (m_UserInfo.myMap.count(username)) 
		{
			m_linkInfo.myMap[hSocket].username = username;
			//准备密码
			string password;
			password = m_UserInfo.myMap[username];
			int t_p = 0;
			sstream << password;
			sstream >> t_p;//转换成2字节整数
			sstream.clear();
			t_p = t_p % 65535;//防止超出最大值,我存疑

			//准备要发送的质询数据，N，N个随机数
			u_int seed;//保证随机数足够随机
			seed = (u_int)time(0);
			srand(seed);
			constexpr auto MIN_VALUE = 0;
			constexpr auto MAX_VALUE = 20;
			u_int num_N = rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;

			sendbuf[0] = 2;//填事件号
			sendbuf[1] = num_N;//整数个数
			temp = sendbuf + 2;//指针就位

			u_short correct_sum = 0;//本地计算值
			u_short correct_result = 0;
			u_short correct_password = (u_short)t_p;
			u_short x = 0;//两字节u_short;
			for (size_t i = 0; i < num_N; i++)
			{
				x = rand();//最大65535
				correct_sum += x;
				x = htons(x);
				memcpy(temp, &x, 2);
				temp += 2;

				
			}
			send(hSocket, sendbuf, MAX_BUF_SIZE, 0);//发送质询报文
			correct_result = correct_sum ^ correct_password;//异或
			m_Comparison.myMap.insert(pair<SOCKET, string>(hSocket, to_string(correct_result)));//保存下来
			m_WaitAns.myMap.insert(pair<SOCKET, string>(hSocket, username));//更改状态
			m_WaitAcc.myMap.erase(hSocket);
			TRACE("Challenge finish");
		}
		else//非法用户
		{

		}
		break;
	}
	case 2://质询结果到来,不应到来
	{
		break;
	}
	default:
		break;
	}
	
	return;
}

void CFlieserverDoc::fsm_HandleRes(SOCKET hSocket, int event, char* buf, int strlen)
{
	char sendbuf[MAX_BUF_SIZE] = { 0 };
	char* temp = buf + 1;
	//函数：提取有用信息，把信息和存的值比较，如果正确，返回认证结果，更改状态，用户在线。如果错误，就...
	switch (event)
	{
	case 0://非法数据
		break;
	case 1://用户名到来，不应到来
	{
		break;
	}
	case 2://质询结果到来
	{

		if (m_Comparison.myMap.count(hSocket))
		{
			u_short answer = ntohs(*(u_short*)temp);
			//用完要清空
			u_short t_result = 0;
			sstream << m_Comparison.myMap[hSocket];
			sstream >> t_result;//转换成2字节整数
			sstream.clear();
			if (answer == t_result)
			{
				sendbuf[0] = 4;//填事件号
				sendbuf[1] = 1;//认证成功
				send(hSocket, sendbuf, 3, 0);//发送
				m_UserOL.myMap.insert(pair<SOCKET, string>(hSocket, m_WaitAns.myMap[hSocket]));
				m_WaitAns.myMap.erase(hSocket);
				TRACE("%s user online", m_UserOL.myMap[hSocket]);
			}
			else// 质询结果出错
			{
				TRACE("质询结果错");
			}
		}
		else//非法套接字发送的质询结果
		{
			TRACE("非法套接字的质询结果");
		}
		break;
	}
	default:
		break;
	}
	
	return;
}

BOOL CFlieserverDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CFlieserverDoc 序列化

void CFlieserverDoc::Serialize(CArchive& ar)
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
void CFlieserverDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CFlieserverDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CFlieserverDoc::SetSearchContent(const CString& value)
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

// CFlieserverDoc 诊断

#ifdef _DEBUG
void CFlieserverDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFlieserverDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CFlieserverDoc 命令
