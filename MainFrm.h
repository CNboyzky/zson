
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "json_core.h"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void SetType(zson::JsonType);
	void SetPath(const zson::JsonElement* pEle);

protected:  // control bar embedded members
	//CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnUpdateStatusBar(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

};


