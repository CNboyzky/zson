
// ZsonView.h : interface of the CZsonView class
//

#pragma once


class CZsonView : public CTabView
{
protected: // create from serialization only
	CZsonView() noexcept;
	DECLARE_DYNCREATE(CZsonView)

// Attributes
public:
	CZsonDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL IsScrollBar() const;
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	
// Implementation
public:
	virtual ~CZsonView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg INT OnCreate(LPCREATESTRUCT lpcs);

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ZsonView.cpp
inline CZsonDoc* CZsonView::GetDocument() const
   { return reinterpret_cast<CZsonDoc*>(m_pDocument); }
#endif

