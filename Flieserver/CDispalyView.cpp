// CDispalyView.cpp: 实现文件
//

#include "pch.h"
#include "Flieserver.h"
#include "CDispalyView.h"


// CDispalyView

IMPLEMENT_DYNCREATE(CDispalyView, CFormView)

CDispalyView::CDispalyView()
	: CFormView(IDD_DISPLAYVIEW)
{

}

CDispalyView::~CDispalyView()
{
}

void CDispalyView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDispalyView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CDispalyView::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDispalyView 诊断

#ifdef _DEBUG
void CDispalyView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDispalyView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDispalyView 消息处理程序


void CDispalyView::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
}
