
// FileServerDlg.h: 头文件
//

#pragma once


// CFileServerDlg 对话框
class CFileServerDlg : public CDialogEx
{
// 构造
public:
	CFileServerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILESERVER_DIALOG };
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
	DWORD state;// 状态记录

	// 控件变量
	CEdit ServerPort;
	CButton ServerListen;
	CButton ServerStop;
	CListBox UserName;

	afx_msg void OnListen();
	CString PathtoList(CString path); // 获取指定目录下的文件列表，文件之间用|隔开
	BOOL RecvOnce(char* buf, int length);// 单次接收内容的函数
	void UploadStateHandler();// 上传过程FSM
	BOOL UploadOnce(const char* buf, int length);// 单次上传内容的函数
	void DownloadStateHandler();// 上传过程FSM

	void DeleteStateHandler();// 下载过程FSM
	
	// Socket相关变量
	UINT m_port_server;
	SOCKET hCommSock;
	SOCKADDR_IN clntAdr;
	int clntAdrLen;

	CString m_send;

	// 上传、下载和删除通用
	int nameLength;
	ULONGLONG fileLength;
	CFileException errFile;

	// 上传文件相关
	CString uploadName;
	CFile uploadFile;
	ULONGLONG leftToRecv;

	// 下载文件相关
	CString downloadName;
	CFile downloadFile;
	ULONGLONG leftToSend;

	// 删除文件相关
	CString deleteName;

	int count;
};
