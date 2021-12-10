
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
	m_Comparison.myMap.clear();
	pView = nullptr;

}

CFlieserverDoc::~CFlieserverDoc()
{
}

CString CFlieserverDoc::PathtoList(CString path = "..\\m_filepath\\*") // 获取指定目录下的文件列表，文件之间用|隔开
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
void CFlieserverDoc::fsm_Challenge(SOCKET hSocket)
{
	char sendbuf[MAX_BUF_SIZE] = { 0 };
	char recvbuf[MAX_BUF_SIZE] = { 0 };
	char* temp= nullptr;
	char event;
	u_short packet_len;
	int strLen = recv(hSocket, recvbuf, 3, 0);
	if (strLen == 3) {
		event = recvbuf[0];
		temp = &recvbuf[1];
		packet_len = ntohs(*(u_short*)temp);
		assert(packet_len > 3);
		strLen = recv(hSocket, recvbuf + 3, packet_len - 3, 0);
		assert(strLen == packet_len - 3);
	}
	else return;
	//函数：拿到用户名，查用户名对应的密码，随机出随机数，和密钥异或，存起来，然后发送随机数。最后，更改状态
	switch (event)
	{
	case 0://非法数据
		break;
	case 1://用户名到来
		{
			int namelen = recvbuf[3];
			assert(namelen >= 0 && namelen <= MAX_BUF_SIZE);
			string username(&recvbuf[4], namelen);
			if (m_UserInfo.myMap.count(username))
			{
				m_linkInfo.myMap[hSocket].username = username;
				//准备密码
				string password;
				password = m_UserInfo.myMap[username];
				int t_p = 0;
				sstream << password;
				sstream >> t_p;//转换成整数
				sstream.clear();

				//准备要发送的质询数据，N，N个随机数
				u_int seed;//保证随机数足够随机
				seed = (u_int)time(0);
				srand(seed);
				constexpr auto MIN_VALUE = 1;
				constexpr auto MAX_VALUE = 20;
				u_int num_N = rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;

				sendbuf[0] = 2;//填事件号
				//send_packet_len=4+num_N*2
				temp = &sendbuf[1];
				*(u_short*)temp = htons(4 + num_N * 2);
				sendbuf[3] = num_N;//整数个数
				temp = sendbuf + 4;//指针就位

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
				send(hSocket, sendbuf, 4 + num_N * 2, 0);//发送质询报文

				correct_result = correct_sum ^ correct_password;//异或
				m_Comparison.myMap.insert(pair<SOCKET, string>(hSocket, to_string(correct_result)));//保存下来
				m_linkInfo.myMap[hSocket].state = 2;//状态转移
				TRACE("Challenge finish");
			}
			else;//非法用户
		}
		break;
	case 2://质询结果到来,不应到来
		break;
	default:
		break;
	}
	return;
}

void CFlieserverDoc::fsm_HandleRes(SOCKET hSocket)
{
	POSITION pos = GetFirstViewPosition();
	pView = (CDisplayView*)GetNextView(pos);

	char sendbuf[MAX_BUF_SIZE] = { 0 };
	char recvbuf[MAX_BUF_SIZE] = { 0 };
	char* temp = nullptr;
	char event;
	u_short packet_len;
	int strLen = recv(hSocket, recvbuf, 3, 0);
	if (strLen == 3) {
		event = recvbuf[0];
		temp = &recvbuf[1];
		packet_len = ntohs(*(u_short*)temp);//若是正确报文，此处total_length=5
		assert(packet_len > 3);
		strLen = recv(hSocket, recvbuf + 3, packet_len - 3, 0);
		assert(strLen == packet_len - 3);
	}
	else return;
	//函数：提取有用信息，把信息和存的值比较，如果正确，返回认证结果，更改状态，用户在线。如果错误，就...
	switch (event)
	{
	case 3://质询结果到来
		{
			if (m_Comparison.myMap.count(hSocket))
			{
				temp = recvbuf + 3;
				u_short answer = ntohs(*(u_short*)temp);
				//用完要清空
				u_short t_result = 0;
				sstream << m_Comparison.myMap[hSocket];
				sstream >> t_result;//转换成2字节整数
				sstream.clear();
				if (answer == t_result)
				{
					sendbuf[0] = 4;//填事件号
					temp = &sendbuf[1];
					*(u_short*)temp = htons(4);//packet_len=4
					sendbuf[3] = 1;//认证成功
					send(hSocket, sendbuf, 4, 0);//发送
					m_linkInfo.myMap[hSocket].state = 3;//进入主状态
					TRACE("user online");
					pView->UserName.AddString(inet_ntoa(m_linkInfo.myMap[hSocket].ip)); // 添加在线用户的IP
					
					//用户在线之后立即给用户发送一份目录，这是唯一一次主动发送目录。这里利用recvbuf发送报文
					memset(recvbuf, '\0', 5);//清空字符数组
					recvbuf[0] = 6;
					temp = &recvbuf[1];
					CString m_list = PathtoList();//发送默认路径下的目录
					strLen = m_list.GetLength();
					*(u_short*)temp = htons(strLen + 3);//packet_len=strLen + 3
					strcpy_s(recvbuf + 3, strLen + 1, m_list);
					send(hSocket, recvbuf, strLen + 3, 0);
				}
				else TRACE("质询结果错");
			}
			else TRACE("非法套接字的质询结果");
		}
		break;
	default:
		break;
	}
	return;
}

void CFlieserverDoc::MainState_fsm(SOCKET hSocket)
{
	//POSITION pos = GetFirstViewPosition();
	//pView = (CDisplayView*)GetNextView(pos);

	char sendbuf[MAX_BUF_SIZE] = { 0 };
	char recvbuf[MAX_BUF_SIZE] = { 0 };
	char* temp = nullptr;
	char event;
	u_short packet_len;
	int strLen = recv(hSocket, recvbuf, 3, 0);
	if (strLen == 3) {
		event = recvbuf[0];
		temp = &recvbuf[1];
		packet_len = ntohs(*(u_short*)temp);
		assert(packet_len > 3);
		strLen = recv(hSocket, recvbuf + 3, packet_len - 3, 0);
		assert(strLen == packet_len - 3);
	}
	else return;
	switch (event)
	{
	case 5://请求目录
		{
			CString m_recvdir(&recvbuf[3], packet_len - 3);
			if (m_recvdir.Find("m_filepath") != -1) //如果请求的目录合法
			{
				sendbuf[0] = 6;
				temp = &sendbuf[1];
				
				CString m_send = PathtoList(m_recvdir); // 发送该目录下的文件列表给客户端，目录已经有"\\*"了
				strLen = m_send.GetLength();//重新使用strLen
				*(u_short*)temp = htons(strLen + 3);
				//temp = m_send.GetBuffer();
				//使用strcpy,长度全都需要+1！
				strcpy_s(&sendbuf[3], strLen + 1, m_send);
				//m_send.ReleaseBuffer();
				send(hSocket, sendbuf, strLen + 3, 0);
				m_linkInfo.myMap[hSocket].strdirpath = m_recvdir.Left(m_recvdir.GetLength() - 1);// 让服务器用strdirpath记住用户正在看的目录
			}
			else{}//请求目录不合法
		}
		break;
	case 11://请求下载
		break;
	case 15://请求上传
		break;
	case 19://请求删除
		break;
	default:
		break;
	}
}

void CFlieserverDoc::Recvfile(SOCKET hSocket)
{

}

void CFlieserverDoc::WaitUpload(SOCKET hSocket)
{

}

void CFlieserverDoc::WaitAck(SOCKET hSocket)
{

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
