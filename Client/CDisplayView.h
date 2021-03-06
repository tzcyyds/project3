#pragma once

#include "pch.h"


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
	//CIPAddressCtrl ServerIP;
	DWORD m_ip;
	// server port
	UINT m_SPort;
	// local port
	UINT m_LPort;


	int client_state;
	SOCKET hCommSock;
	SOCKADDR_IN servAdr;

	CString strdirpath; // 文件路径

	// 上传、下载和删除通用
	CFileException errFile;
	CHAR sequence;

	// 上传文件相关
	CFile uploadFile;
	ULONGLONG leftToSend;

	// 下载文件相关
	CFile downloadFile;
	ULONGLONG leftToRecv;

	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedDisconnect();
	afx_msg void OnBnClickedEnterdir();
	afx_msg void OnBnClickedGoback();
	afx_msg void OnBnClickedUpload();
	afx_msg void OnBnClickedDownload();
	afx_msg void OnBnClickedDelete();


	void UpdateDir(CString recv);
	void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);

};


