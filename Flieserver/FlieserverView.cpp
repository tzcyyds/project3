
// FlieserverView.cpp: CFlieserverView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Flieserver.h"
#endif

#include "FlieserverDoc.h"
#include "FlieserverView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFlieserverView

IMPLEMENT_DYNCREATE(CFlieserverView, CView)

BEGIN_MESSAGE_MAP(CFlieserverView, CView)
END_MESSAGE_MAP()

// CFlieserverView 构造/析构

CFlieserverView::CFlieserverView() noexcept
{
	// TODO: 在此处添加构造代码

}

CFlieserverView::~CFlieserverView()
{
}

BOOL CFlieserverView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CFlieserverView 绘图

void CFlieserverView::OnDraw(CDC* /*pDC*/)
{
	CFlieserverDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CFlieserverView 诊断

#ifdef _DEBUG
void CFlieserverView::AssertValid() const
{
	CView::AssertValid();
}

void CFlieserverView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFlieserverDoc* CFlieserverView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFlieserverDoc)));
	return (CFlieserverDoc*)m_pDocument;
}
#endif //_DEBUG


// CFlieserverView 消息处理程序
