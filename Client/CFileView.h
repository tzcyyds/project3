#pragma once
#include <afxcview.h>
class CFileView :
    public CTreeView
{
    DECLARE_DYNCREATE(CFileView)
protected:
    CFileView();           // 动态创建所使用的受保护的构造函数
    virtual ~CFileView();
    CTreeCtrl* m_TreeCtrl;

public:

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

