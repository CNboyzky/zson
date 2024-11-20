#pragma once
#include "ZsonCommon.h"
#include "json_core.h"

class CPasteJsonView : public CEditView
{
public:
	DECLARE_DYNCREATE(CPasteJsonView)
	CPasteJsonView();
	~CPasteJsonView();

protected:
	CFont m_font;
	JSONSTRUCT m_js;

protected:
	afx_msg INT OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnPaste();
	DECLARE_MESSAGE_MAP()
};