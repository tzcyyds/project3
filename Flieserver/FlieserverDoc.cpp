
// FlieserverDoc.cpp: CFlieserverDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Flieserver.h"
#endif

#include "FlieserverDoc.h"

#include <propkey.h>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFlieserverDoc

IMPLEMENT_DYNCREATE(CFlieserverDoc, CDocument)

BEGIN_MESSAGE_MAP(CFlieserverDoc, CDocument)
END_MESSAGE_MAP()


// CFlieserverDoc 构造/析构

CFlieserverDoc::CFlieserverDoc() noexcept
{
	// TODO: 在此添加一次性构造代码
	//CFlieserverDoc::BroseAllFiles("..\\m_filepath");
	SetTitle(TEXT("fileserver"));
	m_UserInfo.initDoc();//本地用户信息
	m_UserWait.myMap.clear();//等待回应
	m_UserOL.myMap.clear();//授权后，正式在线

}

CFlieserverDoc::~CFlieserverDoc()
{
}

void CFlieserverDoc::BrowseAllFiles(CString filepath, CTreeCtrl* treeCtrl)
{
	//检测路径是否正确并添加必要信息
	if (filepath == _T(""))
	{
		return;
	}
	else
	{
		if (filepath.Right(1) != _T(""))
		{
			filepath += _T("\\");
		}
		filepath += _T("*.*");
	}

	//递归枚举文件夹下的内容
	CFileFind find;
	CString strpath;
	CString str_fileName;
	BOOL IsFind = find.FindFile(filepath);
	CTreeCtrl* m_TreeCtrl = treeCtrl;
	while (IsFind)
	{
		IsFind = find.FindNextFile();
		strpath = find.GetFilePath();

		if (!find.IsDirectory() && !find.IsDots())
		{
			str_fileName = find.GetFileName();
			m_TreeCtrl->InsertItem((LPCSTR)str_fileName);
			//AfxMessageBox(str_fileName);
		}
		else if (find.IsDirectory() && !find.IsDots())
		{
			str_fileName = find.GetFileName();
			m_TreeCtrl->InsertItem((LPCSTR)str_fileName);
			//AfxMessageBox(str_fileName);

			BrowseAllFiles(strpath, m_TreeCtrl);
		}
		else
		{
			continue;
		}
	}
}

BOOL CFlieserverDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CFlieserverDoc 序列化

void CFlieserverDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CFlieserverDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CFlieserverDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CFlieserverDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CFlieserverDoc 诊断

#ifdef _DEBUG
void CFlieserverDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFlieserverDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CFlieserverDoc 命令
