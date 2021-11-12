#include "pch.h"
#include "CFileView.h"
#include "ClientDoc.h"

//CFileView

IMPLEMENT_DYNCREATE(CFileView, CTreeView)

CFileView::CFileView()
{
	m_TreeCtrl = &GetTreeCtrl();
}

CFileView::~CFileView()
{
}

void CFileView::DoDataExchange(CDataExchange* pDX)
{
	CTreeView::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CFileView, CTreeView)

	ON_WM_CREATE()
END_MESSAGE_MAP()


// 消息处理程序

int CFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CString strPath("..\\m_filepath");
	//CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

	if (!PathIsDirectory(strPath))
	{
		CreateDirectory(strPath, 0);//不存在则创建
		//m_TreeCtrl.InsertItem();
	}
	else
	{

		CClientDoc* pm_doc = (CClientDoc*)this->GetDocument();
		pm_doc->BrowseAllFiles(strPath, m_TreeCtrl);
	}


	return 0;
}
