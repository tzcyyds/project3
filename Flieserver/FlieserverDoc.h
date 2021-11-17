
// FlieserverDoc.h: CFlieserverDoc 类的接口
//


#pragma once
#include "MyUser.h"

class CFlieserverDoc : public CDocument
{
protected: // 仅从序列化创建
	CFlieserverDoc() noexcept;
	DECLARE_DYNCREATE(CFlieserverDoc)

// 特性
public:

// 操作
public:
	void BrowseAllFiles(CString filepath, CTreeCtrl* treeCtrl);
	void fsm_Challenge(SOCKET hSocket,int event,char* buf, int strlen);
	void fsm_HandleRes(SOCKET hSocket, int event, char* buf, int strlen);
// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CFlieserverDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	UserDoc m_UserInfo;//本地用户信息
	UserList m_WaitAcc;//等待用户名
	UserList m_WaitAns;//等待质询的回应
	UserList m_UserOL;//授权后，正式在线
	UserList m_Sendrand;//保存已经发送的质询随机数，socket-string数据形式
// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};

