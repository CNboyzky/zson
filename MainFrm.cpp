
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "Zson.h"

#include "MainFrm.h"
#include "ZsonCommon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE(WM_USER_UPDATE_STATUSBAR, OnUpdateStatusBar)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	//ID_SEPARATOR,           // status line indicator
	//ID_INDICATOR_CAPS,
	//ID_INDICATOR_NUM,
	//ID_INDICATOR_SCRL,
	IDS_INDICATOR_PATH,
	IDS_INDICATOR_TYPE,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	TEXTMETRIC tm;
	CClientDC dc(this);
	CFont* pFont = m_wndStatusBar.GetFont();
	CFont* pOldFont = dc.SelectObject(pFont);
	dc.GetTextMetrics(&tm);
	dc.SelectObject(pOldFont);

	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	m_wndStatusBar.SetPaneInfo(0, IDS_INDICATOR_PATH, SBPS_NOBORDERS | SBPS_STRETCH, 0);
	m_wndStatusBar.SetPaneInfo(1, IDS_INDICATOR_TYPE, SBPS_POPOUT, 12 * tm.tmAveCharWidth);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style &= ~FWS_ADDTOTITLE;
	return TRUE;
}

LRESULT CMainFrame::OnUpdateStatusBar(WPARAM wParam, LPARAM lParam)
{
	const zson::JsonElement* pEle = (zson::JsonElement*)wParam;
	if (!pEle) return 0;
	SetType(pEle->m_type);
	SetPath(pEle);

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::SetType(zson::JsonType jsonType)
{
	CString str;
	switch (jsonType)
	{
	case zson::JsonType::UNKNDOWN:
		str = _T("Unknown");
		break;
	case zson::JsonType::ARRAY:
		str = _T("Array");
		break;
	case zson::JsonType::OBJECT:
		str = _T("Object");
		break;
	case zson::JsonType::STRING:
		str = _T("String");
		break;
	case zson::JsonType::BOOL:
		str = _T("Bool");
		break;
	case zson::JsonType::INTEGER:
		str = _T("Integer");
		break;
	case zson::JsonType::DECIMAL:
		str = _T("Decimal");
		break;
	case zson::JsonType::NUL:
		str = _T("Null");
		break;
	default:
		break;
	}
	m_wndStatusBar.SetPaneText(1, str);
}

void CMainFrame::SetPath(const zson::JsonElement* pEle)
{
	if (!pEle)
	{
		m_wndStatusBar.SetPaneText(0, _T(""));
		return;
	}

	CString str;
	while (pEle)
	{
		CString str1 = CA2T(pEle->m_field.c_str());
		/*if (str.IsEmpty())
		{
			str = str1;
		}
		else
		{
			if (str.Left(1) == _T("["))
			{
				str = str1 + str;
			}
			else
			{
				str1 = str1 + _T(".");
				str = str1 + str;
			}
		}*/
		if (str.IsEmpty())
		{
			str = str1;
		}
		else
		{
			str1 = str1 + _T(" -> ");
			str = str1 + str;
		}

		pEle = pEle->m_parent;
	}
	m_wndStatusBar.SetPaneText(0, str);
}

