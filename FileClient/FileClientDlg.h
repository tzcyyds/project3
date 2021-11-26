
// FtpClientDlg.h : 头文件
//

#pragma once
#include "pch.h"

// CFileClientDlg 对话框
class CFileClientDlg : public CDialogEx
{
// 构造
public:
	CFileClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILECLIENT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CString strdirpath; // 文件路径

	CButton ServerLogin;
	CButton ServerDisconnect;
	CButton FileUpload;
	CButton FileDownload;
	CButton FileDelete;
	CIPAddressCtrl ServerIP;
	CEdit ServerPort;
	CEdit ClientPort;
	CEdit ServerUsername;
	CEdit ServerPassword;
	CButton FileInside;
	CButton FileOutside;
	CListBox FileName;

	afx_msg void OnConnect();
	afx_msg void OnDisconnect();
	afx_msg void OnEnterDir();
	afx_msg void OnGoBack();
	afx_msg void OnUpLoad();
	afx_msg void OnDownload();
	afx_msg void OnDelete();
	afx_msg bool ConnectServ();
	afx_msg void UpdateDir(CString recv);
	void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c); // 字符串分割函数
	BOOL UploadOnce(const char* buf, int length);// 单次上传内容的函数

	// Socket相关变量
	DWORD m_ip;
	UINT m_port_client;
	UINT m_port_server;
	SOCKADDR_IN servAdr;
	SOCKET hCommSock;

	CString m_send;
};