#pragma once
#include <afxcview.h>
class CFileView :
    public CTreeView
{
    DECLARE_DYNCREATE(CFileView)
protected:
    CFileView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
    virtual ~CFileView();
    CTreeCtrl* m_TreeCtrl;

public:

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()
public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

