
// ZsonView.cpp : implementation of the CZsonView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Zson.h"
#endif

#include "ZsonDoc.h"
#include "ZsonView.h"
#include "PasteJsonView.h"
#include "ShowJsonView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CZsonView

IMPLEMENT_DYNCREATE(CZsonView, CTabView)

BEGIN_MESSAGE_MAP(CZsonView, CTabView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CTabView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CTabView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CTabView::OnFilePrintPreview)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CZsonView construction/destruction

CZsonView::CZsonView() noexcept
{
	// TODO: add construction code here

}

CZsonView::~CZsonView()
{
}

BOOL CZsonView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

INT CZsonView::OnCreate(LPCREATESTRUCT lpcs)
{
	if (CTabView::OnCreate(lpcs) == -1) return -1;

	// add views
	AddView(RUNTIME_CLASS(CPasteJsonView), _T("Paste json here"), 0, (CCreateContext*) lpcs->lpCreateParams);
	AddView(RUNTIME_CLASS(CShowJsonView), _T("Format json"), 1, (CCreateContext*)lpcs->lpCreateParams);

	this->GetTabControl().SetLocation(CMFCTabCtrl::LOCATION_TOP);    //方向上顶
	this->GetTabControl().ModifyTabStyle(CMFCTabCtrl::STYLE_FLAT_SHARED_HORZ_SCROLL);    //风格
	this->GetTabControl().EnableAutoColor(TRUE);	//自动着色
	this->GetTabControl().SetTabBorderSize(3);		//边框大小
	this->GetTabControl().HideSingleTab(FALSE);		//单个Tab时候不显示Tab标签
	this->GetTabControl().EnableTabSwap(FALSE);		//禁止拖动

	return TRUE;
}

void CZsonView::OnInitialUpdate()
{
	CTabView::OnInitialUpdate();
}

BOOL CZsonView::IsScrollBar() const
{
	return FALSE;
}


// CZsonView drawing

void CZsonView::OnDraw(CDC* /*pDC*/)
{
	CZsonDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CZsonView printing

BOOL CZsonView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CZsonView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CZsonView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CZsonView diagnostics

#ifdef _DEBUG
void CZsonView::AssertValid() const
{
	CView::AssertValid();
}

void CZsonView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CZsonDoc* CZsonView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CZsonDoc)));
	return (CZsonDoc*)m_pDocument;
}
#endif //_DEBUG


// CZsonView message handlers
