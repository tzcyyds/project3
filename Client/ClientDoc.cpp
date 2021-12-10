
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


#define MAX_WSAE_TIMES 10// 单次发送或接收过程中所允许出现WSAEWOULDBLOCK的最大次数
#define CHUNK_SIZE 4096
constexpr auto MAX_BUF_SIZE = 100;
// CClientDoc

IMPLEMENT_DYNCREATE(CClientDoc, CDocument)

BEGIN_MESSAGE_MAP(CClientDoc, CDocument)
END_MESSAGE_MAP()


// CClientDoc 构造/析构

CClientDoc::CClientDoc() noexcept
{
	// TODO: 在此添加一次性构造代码
	pView = nullptr;
}

CClientDoc::~CClientDoc()
{
}


void CClientDoc::socket_state1_fsm(SOCKET s)
{
	POSITION pos = GetFirstViewPosition();
	pView = (CDisplayView*)GetNextView(pos);

	char sendbuf[MAX_BUF_SIZE] = { 0 };
	char recvbuf[MAX_BUF_SIZE] = { 0 };
	char* temp = nullptr;
	char event;
	u_short packet_len;
	int strLen = recv(s, recvbuf, 3, 0);
	if (strLen == 3) {
		event = recvbuf[0];
		temp = &recvbuf[1];
		packet_len = ntohs(*(u_short*)temp);
		assert(packet_len > 3);
		strLen = recv(s, recvbuf + 3, packet_len - 3, 0);
		assert(strLen == packet_len - 3);
	}
	else return;

	switch (event)
	{
	case 2://收到质询报文
		{
			u_int num_N = recvbuf[3];
			temp = &recvbuf[4];

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
					correct_sum += ntohs(*(u_short*)temp);
					temp = temp + 2;
				}

				if ((password_len > 0) && (password_len <= 5))//两字节，最大65535
				{
					password_value = (u_int)_ttoi(pView->m_password);
					correct_password = (u_short)(password_value % 65536);
					correct_result = correct_sum ^ correct_password;

					
					sendbuf[0] = 3;//填写事件号
					temp = &sendbuf[1];
					*(u_short*)temp = htons(5);//packet_len=5
					temp = &sendbuf[3];
					*(u_short*)temp = htons(correct_result);//写入赋值，挺复杂的写法
					send(s, sendbuf, 5, 0);
					TRACE("respond challenge");
					pView->client_state = 2;//状态转换，已返回质询结果，等待确认
				}
				else
				{//密码长度不对
				}
			}
			else
			{//整数个数不对
			}
		}
		break;
	default:
		break;
	}
	return;
}

void CClientDoc::socket_state2_fsm(SOCKET s)
{
	POSITION pos = GetFirstViewPosition();
	pView = (CDisplayView*)GetNextView(pos);

	char sendbuf[MAX_BUF_SIZE] = { 0 };
	char recvbuf[MAX_BUF_SIZE] = { 0 };
	char* temp = nullptr;
	char event;
	u_short packet_len;
	int strLen = recv(s, recvbuf, 3, 0);
	if (strLen == 3) {
		event = recvbuf[0];
		temp = &recvbuf[1];
		packet_len = ntohs(*(u_short*)temp); // 这里应该等于packet_len = 4
		assert(packet_len > 3);
		strLen = recv(s, recvbuf + 3, packet_len - 3, 0);
		assert(strLen == packet_len - 3);
	}
	else return;

	switch (event)
	{
	case 4://认证结果报文
		if (recvbuf[3] == 1) {
			pView->client_state = 3;//认证成功，进入等待操作状态
			TRACE("认证成功");
		}
		else;//认证失败
		break;
	default:
		break;
	}
	return;
}

void CClientDoc::socket_state3_fsm(SOCKET s)
{
	POSITION pos = GetFirstViewPosition();
	pView = (CDisplayView*)GetNextView(pos);

	char sendbuf[MAX_BUF_SIZE] = { 0 };
	char recvbuf[MAX_BUF_SIZE] = { 0 };
	char* temp = nullptr;
	char event;
	u_short packet_len;
	int strLen = recv(s, recvbuf, 3, 0);
	if (strLen == 3) {
		event = recvbuf[0];
		temp = &recvbuf[1];
		packet_len = ntohs(*(u_short*)temp); 
		assert(packet_len > 3);
		strLen = recv(s, recvbuf + 3, packet_len - 3, 0);
		assert(strLen == packet_len - 3);
	}
	else return;
	switch (event)
	{
	case 6://接收返回目录
		{
		//std::string file_list(&recvbuf[3], packet_len - 3);
		CString file_list(&recvbuf[3], packet_len - 3);
		pView->UpdateDir(file_list);
		}
		break;
	default:
		break;
	}

}

void CClientDoc::socket_state4_fsm(SOCKET s)
{
	POSITION pos = GetFirstViewPosition();
	pView = (CDisplayView*)GetNextView(pos);

	char recvbuf[3] = { 0 };
	char chunk_recv_buf[CHUNK_SIZE] = { 0 };
	char chunk_send_buf[CHUNK_SIZE] = { 0 };
	int strLen = recv(s, recvbuf, 3, 0);
	if (strLen <= 0) {
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			closesocket(s);
			return;
		}
	}
	else {
		if (strLen != 3) {
			printf("state4 not receive enough data!!!/n");
		}
		u_int event = recvbuf[0];
		char* temp = &recvbuf[1];
		u_short pkt_len = ntohs(*(u_short*)temp);
		switch (event)
		{
		case 16://收到回应上传请求
			strLen = recv(s, chunk_recv_buf, (pkt_len - 3), 0);
			if (strLen <= 0) {
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					closesocket(s);
					return;
				}
			}
			else {
				u_int upload_permission = chunk_recv_buf[0];
				if (upload_permission == 1) {
					if (pView->leftToSend > 0)
					{
						int readChunkSize = pView->uploadFile.Read(chunk_send_buf+6, CHUNK_SIZE-6);
						temp = chunk_send_buf;
						*(char*)temp = 7;
						temp = temp + 1;
						*(u_short*)temp = 6 + (u_short)readChunkSize;
						temp = temp + 2;
						pView->sequence = 0;
						*(char*)temp = pView->sequence;
						temp = temp + 1;
						*(u_short*)temp = (u_short)readChunkSize;

						if (pView->UploadOnce(chunk_send_buf, readChunkSize+6) == FALSE)
						{
							DWORD errSend = WSAGetLastError();
							TRACE("\nError occurred while sending file chunks\n"
								"\tGetLastError = %d\n", errSend);
							ASSERT(errSend != WSAEWOULDBLOCK);
						}
						memcpy(pView->databuf, chunk_send_buf, readChunkSize + 6);
						pView->databuf_size = readChunkSize + 6;
						pView->leftToSend -= readChunkSize;
					}
					pView->client_state = 5;
				}
				else {
					
				}
			}

			break;
		default:
			break;
		}
	}


}

void CClientDoc::socket_state5_fsm(SOCKET s)
{
	POSITION pos = GetFirstViewPosition();
	pView = (CDisplayView*)GetNextView(pos);

	char recvbuf[3] = { 0 };
	char chunk_recv_buf[CHUNK_SIZE] = { 0 };
	char chunk_send_buf[CHUNK_SIZE] = { 0 };
	int strLen = recv(s, recvbuf, 3, 0);
	if (strLen <= 0) {
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			closesocket(s);
			return;
		}
	}
	else {
		if (strLen != 3) {
			printf("state5 not receive enough data!!!/n");
		}
		u_int event = recvbuf[0];
		char* temp = &recvbuf[1];
		u_short pkt_len = ntohs(*(u_short*)temp);
		switch (event)
		{
		case 8://收到正确确认
			strLen = recv(s, chunk_recv_buf, (pkt_len - 3), 0);
			if (strLen <= 0) {
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					closesocket(s);
					return;
				}
			}
			else {
				if (chunk_recv_buf[0] == pView->sequence) {
					memset(pView->databuf, 0, pView->databuf_size);//释放缓存
					if (pView->leftToSend > 0)
					{
						int readChunkSize = pView->uploadFile.Read(chunk_send_buf + 6, CHUNK_SIZE - 6);
						temp = chunk_send_buf;
						*(char*)temp = 7;
						temp = temp + 1;
						*(u_short*)temp = 6 + (u_short)readChunkSize;
						temp = temp + 2;
						pView->sequence ^= 0x01;//取反，0变1，1变0
						*(char*)temp = pView->sequence;
						temp = temp + 1;
						*(u_short*)temp = (u_short)readChunkSize;

						if (pView->UploadOnce(chunk_send_buf, readChunkSize + 6) == FALSE)
						{
							DWORD errSend = WSAGetLastError();
							TRACE("\nError occurred while sending file chunks\n"
								"\tGetLastError = %d\n", errSend);
							ASSERT(errSend != WSAEWOULDBLOCK);
						}
						pView->leftToSend -= readChunkSize;
						pView->client_state = 5;
					}
					else if (pView->leftToSend == 0) {
						printf("Upload finished! Total bytes:%lld\n", pView->fileLength);
						pView->client_state = 3;
					}
					else {
						printf("leftToSend error!!!/n");
					}
				}
				else {
					//报文序号错误，不予发送下一个
				}
			}
			break;
		case 9://收到重传确认
			strLen = recv(s, chunk_recv_buf, (pkt_len - 3), 0);
			if (strLen <= 0) {
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					closesocket(s);
					return;
				}
			}
			else {
				if (chunk_recv_buf[0] == pView->sequence) {
					char* temp = pView->databuf;
					temp = temp + 3;
					pView->sequence ^= 0x01;//序号递增
					*(char*)temp = pView->sequence;

					if (pView->UploadOnce(pView->databuf, pView->databuf_size) == FALSE)
					{
						DWORD errSend = WSAGetLastError();
						TRACE("\nError occurred while sending file chunks\n"
							"\tGetLastError = %d\n", errSend);
						ASSERT(errSend != WSAEWOULDBLOCK);
					}
					pView->client_state = 5;
				}
				else {
					//报文序号错误，不予发送下一个
				}
			}

			break;
		default:
			break;
		}
	}

}

void CClientDoc::socket_state6_fsm(SOCKET s)
{
	POSITION pos = GetFirstViewPosition();
	pView = (CDisplayView*)GetNextView(pos);

	char recvbuf[3] = { 0 };
	char chunk_recv_buf[CHUNK_SIZE] = { 0 };
	char chunk_send_buf[CHUNK_SIZE] = { 0 };
	int strLen = recv(s, recvbuf, 3, 0);
	if (strLen <= 0) {
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			closesocket(s);
			return;
		}
	}
	else {
		if (strLen != 3) {
			printf("state6 not receive enough data!!!/n");
		}
		u_int event = recvbuf[0];
		char* temp = &recvbuf[1];
		u_short pkt_len = ntohs(*(u_short*)temp);
		switch (event)
		{
		case 12://收到回应下载请求
			strLen = recv(s, chunk_recv_buf, (pkt_len - 3), 0);
			if (strLen <= 0) {
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					closesocket(s);
					return;
				}
			}
			else {
				if (chunk_recv_buf[0] == 1) {
					char* temp = chunk_recv_buf;
					temp = temp + 1;
					pView->fileLength = ntohl(*(u_long *)temp);
					pView->leftToRecv = pView->fileLength;
					char sendbuf[MAX_BUF_SIZE] = { 0 };
					temp = sendbuf;
					*(char*)temp = 13;
					temp = temp + 1;
					*(u_short*)temp = htons(4);
					temp = temp + 2;
					*(char*)temp = 1;
					if (pView->UploadOnce(sendbuf, 4) == FALSE)
					{
						DWORD errSend = WSAGetLastError();
						TRACE("\nError occurred while sending file chunks\n"
							"\tGetLastError = %d\n", errSend);
						ASSERT(errSend != WSAEWOULDBLOCK);
					}
					pView->sequence = 0;
					pView->client_state = 7;
				}
				else {
					//服务器拒绝下载请求
				}
			}

			break;
		default:
			break;
		}
	}
}

void CClientDoc::socket_state7_fsm(SOCKET s)
{
	POSITION pos = GetFirstViewPosition();
	pView = (CDisplayView*)GetNextView(pos);

	char recvbuf[3] = { 0 };
	char chunk_recv_buf[CHUNK_SIZE] = { 0 };
	char sendbuf[4] = { 0 };
	int strLen = recv(s, recvbuf, 3, 0);
	if (strLen <= 0) {
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			closesocket(s);
			return;
		}
	}
	else {
		if (strLen != 3) {
			printf("state7 not receive enough data!!!/n");
		}
		u_int event = recvbuf[0];
		char* temp = &recvbuf[1];
		u_short pkt_len = ntohs(*(u_short*)temp);
		switch (event)
		{
		case 7://收到下载数据
			{
				u_int writeChunkSize = (pView->leftToRecv < CHUNK_SIZE) ? pView->leftToRecv : CHUNK_SIZE;//#define CHUNK_SIZE 4096
				if (pView->RecvOnce(chunk_recv_buf, writeChunkSize) == FALSE)
				{
					DWORD errSend = WSAGetLastError();
					TRACE("\nError occurred while receiving file chunks\n"
						"\tGetLastError = %d\n", errSend);
					ASSERT(errSend != WSAEWOULDBLOCK);
				}
				temp = chunk_recv_buf;
				pView->sequence = *(char*)temp;
				temp = temp + 1;
				u_int data_len = ntohs(*(u_short*)temp);
				temp = temp + 2;
				pView->leftToRecv -= data_len;
				pView->downloadFile.Write(temp, data_len);

				temp = sendbuf;
				*(char*)temp = 8;
				temp = temp + 1;
				*(u_short*)temp = htons(4);
				temp = temp + 2;
				*(char*)temp = pView->sequence;
				send(s, sendbuf, 4, 0);

				if (pView->leftToRecv > 0) {
					pView->client_state = 7;
				}
				else {
					pView->client_state = 3;
				}
			}
			break;
		default:
			break;
		}
	}
}

void CClientDoc::socket_state8_fsm(SOCKET s)
{
	POSITION pos = GetFirstViewPosition();
	pView = (CDisplayView*)GetNextView(pos);

	char recvbuf[3] = { 0 };
	char chunk_recv_buf[MAX_BUF_SIZE] = { 0 };
	char sendbuf[4] = { 0 };
	int strLen = recv(s, recvbuf, 3, 0);
	if (strLen <= 0) {
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			closesocket(s);
			return;
		}
	}
	else {
		if (strLen != 3) {
			printf("state8 not receive enough data!!!/n");
		}
		u_int event = recvbuf[0];
		char* temp = &recvbuf[1];
		u_short pkt_len = ntohs(*(u_short*)temp);
		switch (event)
		{
		case 20://收到回应删除请求
			if (pView->RecvOnce(chunk_recv_buf, pkt_len - 3) == FALSE)
			{
				DWORD errSend = WSAGetLastError();
				TRACE("\nError occurred while receiving file chunks\n"
					"\tGetLastError = %d\n", errSend);
				ASSERT(errSend != WSAEWOULDBLOCK);
			}
			temp = chunk_recv_buf;
			if (*(char*)temp == 1) {
				printf("delete success!!!\n");
				pView->client_state = 3;
			}
			else {
				//删除失败
			}
			break;
		default:
			break;
		}
	}
}

void CClientDoc::socket_state9_fsm(SOCKET s)
{
	POSITION pos = GetFirstViewPosition();
	pView = (CDisplayView*)GetNextView(pos);

	char recvbuf[3] = { 0 };
	char chunk_recv_buf[CHUNK_SIZE] = { 0 };
	char sendbuf[4] = { 0 };
	int strLen = recv(s, recvbuf, 3, 0);
	if (strLen <= 0) {
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			closesocket(s);
			return;
		}
	}
	else {
		if (strLen != 3) {
			printf("state8 not receive enough data!!!/n");
		}
		u_int event = recvbuf[0];
		char* temp = &recvbuf[1];
		u_short pkt_len = ntohs(*(u_short*)temp);
		switch (event)
		{
		case 6: {//收到返回目录
			if (pView->RecvOnce(chunk_recv_buf, pkt_len - 3) == FALSE)
			{
				DWORD errSend = WSAGetLastError();
				TRACE("\nError occurred while receiving file chunks\n"
					"\tGetLastError = %d\n", errSend);
				ASSERT(errSend != WSAEWOULDBLOCK);
			}
			CString recvdialog(chunk_recv_buf);
			pView->UpdateDir(recvdialog);
			pView->client_state = 3;
			break;
		}
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
