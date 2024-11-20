#include "pch.h"
#include "PasteJsonView.h"
#include <string>
#include "json_parse.h"
#include "json_util.h"

IMPLEMENT_DYNCREATE(CPasteJsonView, CEditView)

BEGIN_MESSAGE_MAP(CPasteJsonView, CEditView)
	ON_WM_CREATE()
	ON_WM_PASTE()
END_MESSAGE_MAP()

CPasteJsonView::CPasteJsonView()
{
	m_js.m_jsonData = nullptr;
}

CPasteJsonView::~CPasteJsonView()
{

}

INT CPasteJsonView::OnCreate(LPCREATESTRUCT lpcs)
{
	if (CEditView::OnCreate(lpcs) == -1) return -1;

	m_font.CreatePointFont(120, _T("Microsoft YaHei"));
	SetFont(&m_font);

	return TRUE;
}

void CPasteJsonView::OnPaste()
{
	// get content from clipboard and format it
	CString ustr;
	if (::OpenClipboard(NULL))
	{
		UINT nFormat = sizeof(TCHAR) == sizeof(WCHAR) ? CF_UNICODETEXT : CF_TEXT;
		HGLOBAL hClip = ::GetClipboardData(nFormat);
		if (hClip)
		{
			TCHAR* pBuff = (TCHAR*) GlobalLock(hClip);
			GlobalUnlock(hClip);
			ustr.Format(_T("%s"), pBuff);
		}
		::CloseClipboard();
	}
	if (ustr.IsEmpty()) return;

	// Add text to editctrl
	GetEditCtrl().Clear();
	GetEditCtrl().SetWindowText(ustr);

	// format json
	std::string astr = CT2A(ustr.GetString());
	int nFlag = zson::JsonUtils::IsJson(astr);
	if (!nFlag)
	{
		MessageBox(_T("Please paste a json!"));
		return;
	}

	// release json object
	TRACE(_T("1. refrence count: %d\n"), m_js.m_jsonData.use_count());
	m_js.m_jsonData = nullptr;
	try {
		if (nFlag == 1)	// nFlag == 1, object
		{
			m_js.m_jsonData = zson::JsonParse::ParseObject(astr, nullptr, "Root");
		}
		else // nFlag == 2, array
		{
			m_js.m_jsonData = zson::JsonParse::ParseArray(astr, nullptr, "Root");
		}
		// send data to other views
		GetDocument()->UpdateAllViews(this, ID_HINT_SEND_JSON, (CObject*)(&m_js));
	} 
	catch (zson::JsonException& err)
	{
		MessageBoxA(this->m_hWnd, err.what(), "´íÎó", MB_OK);
		// reset m_js
		m_js.m_jsonData = nullptr;
	}
}