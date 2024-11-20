#pragma once
#include "ZsonCommon.h"
#include "json_core.h"

class CShowJsonView : public CView
{
public:
	DECLARE_DYNCREATE(CShowJsonView)
	CShowJsonView();
	~CShowJsonView();

protected:
	static UINT nImgIDs[6];
	CListCtrl m_listCtrl;
	CTreeCtrl m_treeCtrl;
	CImageList m_imgList;
	CMenu     m_menu;
	std::vector<LPJSONITEMDATA> m_treeItemDatas;
	std::vector<LPJSONITEMDATA> m_listItemDatas;
	int		  m_nSelListItem;

// operations
protected:
	void BuildTree(LPJSONSTRUCT lpjs);
	void BuildObjTree(const zson::JsonObject& pObject, HTREEITEM htiParent);
	void BuildArrTree(const zson::JsonArray& pArray, HTREEITEM htiParent);
	void AddItemsToListCtrl(LPJSONITEMDATA lpjs);
	void FreeTreeItemDatas();
	void FreeListItemDatas();
	void CopyDataToClipboard(CString& source);
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
protected:
	afx_msg INT OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTreeItemSelChanged(NMHDR*, LRESULT*);
	afx_msg void OnListItemClicked(NMHDR*, LRESULT*);
	afx_msg void OnCopyField();
	afx_msg void OnCopyValue();

	DECLARE_MESSAGE_MAP()
};