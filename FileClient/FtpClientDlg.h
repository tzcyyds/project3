
// FtpClientDlg.h : ͷ�ļ�
//

#pragma once


#include <afxinet.h>
#include <vector>
#include "afxwin.h"

// CFtpClientDlg �Ի���
class CFtpClientDlg : public CDialogEx
{
// ����
public:
	CFtpClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FTPCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	
	// ���ɵ���Ϣӳ�亯��
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
