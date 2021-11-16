
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
	//UserList m_UserOL;//还未质询
	UserList m_UserWait;//等待回应
	//UserList m_UserOL;//授权之前
	UserList m_UserOL;//授权后，正式在线
// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};

