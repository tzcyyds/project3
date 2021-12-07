#pragma once

#include "pch.h"

#define CHUNK_SIZE 4096
// CDispalyView 窗体视图

class CDisplayView : public CFormView
{
	DECLARE_DYNCREATE(CDisplayView)

protected:
	CDisplayView();           // 动态创建所使用的受保护的构造函数
	virtual ~CDisplayView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DISPLAYVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	CString m_user;
	CString m_password;
	
	CListBox FileName;
	CIPAddressCtrl ServerIP;
	DWORD m_ip;
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedDisconnect();
	// server port
	UINT m_SPort;
	// local port
	UINT m_LPort;

	int client_state;
	SOCKET hCommSock;
	SOCKADDR_IN servAdr;
	CString m_send;
	CString strdirpath; // 文件路径

	// 上传、下载和删除通用
	int nameLength;
	ULONGLONG fileLength;
	CFileException errFile;
	CHAR sequence;

	// 上传文件相关
	CString uploadName;
	CFile uploadFile;
	ULONGLONG leftToSend;
	char databuf[CHUNK_SIZE] = { 0 };
	u_int databuf_size = 0;

	// 下载文件相关
	CString downloadName;
	CFile downloadFile;
	ULONGLONG leftToRecv;

	// 删除文件相关
	CString deleteName;


	afx_msg void OnBnClickedEnterdir();
	afx_msg void OnBnClickedGoback();
	afx_msg void OnBnClickedUpload();
	afx_msg void OnBnClickedDownload();
	afx_msg void OnBnClickedDelete();

	BOOL UploadOnce(const char* buf, int length);
	BOOL RecvOnce(char* buf, int length);
	void UpdateDir(CString recv);
	void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);

};


