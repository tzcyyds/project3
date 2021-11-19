
// FtpClientDlg.h : 头文件
//

#pragma once


#include <afxinet.h>
#include <vector>
#include "afxwin.h"

// CFtpClientDlg 对话框
class CFtpClientDlg : public CDialogEx
{
// 构造
public:
	CFtpClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FTPCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString strdirpath;
	
	CInternetSession *pInternetSession;
	CFtpConnection *pFtpConnection;
	BOOL bconnect;

	CButton FtpLogin;
	CButton FtpDisconnect;
	CButton FileUpload;
	CButton FileDownload;
	CButton FileDelete;
	CButton FtpNonameLog;
	CEdit FtpIP;
	CEdit Ftpuser;
	CEdit FtpPassword;
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
	afx_msg void ConnectFtp();
	afx_msg void UpdateDir();
	

};
