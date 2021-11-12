#include "pch.h"
#include "CFileView.h"
#include "FlieserverDoc.h"

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


// ��Ϣ�������

int CFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CString strPath("..\\m_filepath");
	//CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	
	if (!PathIsDirectory(strPath))
	{
		CreateDirectory(strPath, 0);//�������򴴽�
		//m_TreeCtrl.InsertItem();
	}
	else
	{
		
		CFlieserverDoc* pm_doc = (CFlieserverDoc*)this->GetDocument();
		pm_doc->BrowseAllFiles(strPath, m_TreeCtrl);
	}


	return 0;
}
