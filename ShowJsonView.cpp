#include "pch.h"
#include "resource.h"
#include "ShowJsonView.h"
#include "json_parse.h"

IMPLEMENT_DYNCREATE(CShowJsonView, CView)

BEGIN_MESSAGE_MAP(CShowJsonView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_VIEW, CShowJsonView::OnTreeItemSelChanged)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_VIEW, CShowJsonView::OnListItemClicked)
	ON_COMMAND(ID_MENU_COPY_FIELD, CShowJsonView::OnCopyField)
	ON_COMMAND(ID_MENU_COPY_VALUE, CShowJsonView::OnCopyValue)
END_MESSAGE_MAP()

UINT CShowJsonView::nImgIDs[6] = { IDI_ARRAY, IDI_OBJECT, IDI_STRING, IDI_BOOLEAN, IDI_NUMBER, IDI_NULL };

CShowJsonView::CShowJsonView()
{
	m_nSelListItem = -1;
}

CShowJsonView::~CShowJsonView()
{
	FreeTreeItemDatas();
	FreeListItemDatas();
}

void CShowJsonView::OnDraw(CDC* pDC)
{
	
}

BOOL CShowJsonView::PreCreateWindow(CREATESTRUCT& cs)
{
	// add your code
	return CView::PreCreateWindow(cs);
}

INT CShowJsonView::OnCreate(LPCREATESTRUCT lpcs)
{
	if (CView::OnCreate(lpcs) == -1) return -1;
	
	// create popup menu
	m_menu.LoadMenu(ID_LIST_VIEW_MENU);

	// create image list
	m_imgList.Create(24, 24, ILC_COLOR, 0, 1);
	for (int i = 0; i < sizeof(nImgIDs) / sizeof(UINT); i++)
	{
		m_imgList.Add(AfxGetApp()->LoadIcon(nImgIDs[i]));
	}

	// create controls
	m_treeCtrl.Create(TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS 
					 | WS_VISIBLE | WS_CHILD | WS_BORDER, CRect(0, 0, 0, 0), this, IDC_TREE_VIEW);
	//m_treeCtrl.SetBkColor(RGB(244, 244, 244));
	m_treeCtrl.SetImageList(&m_imgList, TVSIL_NORMAL);

	m_listCtrl.Create(WS_CHILD | WS_VISIBLE | LVS_REPORT,
					 CRect(0, 0, 0, 0), this, IDC_LIST_VIEW);
	// highlight a line while selecting a line
	DWORD dwListCtrlExStyles = m_listCtrl.GetExStyle();
	m_listCtrl.SetExtendedStyle(dwListCtrlExStyles | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_listCtrl.SetImageList(&m_imgList, LVSIL_NORMAL);
	m_listCtrl.SetImageList(&m_imgList, LVSIL_SMALL);
	// listctrl insert columns
	m_listCtrl.InsertColumn(0, _T("Field"), LVCFMT_CENTER, 250);
	m_listCtrl.InsertColumn(1, _T("Value"), LVCFMT_LEFT, 250);
	return TRUE;
}


void CShowJsonView::OnSize(UINT nType, int cx, int cy)
{
	// set controls' position
	m_treeCtrl.SetWindowPos(NULL, 0, 0, cx/2, cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);
	m_listCtrl.SetWindowPos(NULL, cx/2 + 2, 0, cx/2 - 2, cy, SWP_NOZORDER | SWP_NOREDRAW);
}

void CShowJsonView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (lHint == ID_HINT_SEND_JSON)
	{
		// clear
		m_treeCtrl.DeleteAllItems();
		m_listCtrl.DeleteAllItems();
		FreeTreeItemDatas();
		FreeListItemDatas();
		//TRACE(_T("2. refrence count: %d\n"), ((LPJSONSTRUCT)pHint)->m_jsonData.use_count());
		BuildTree((LPJSONSTRUCT) pHint);
	}
}

void CShowJsonView::FreeTreeItemDatas()
{
	for (int i = 0; i < m_treeItemDatas.size(); i++)
	{
		delete m_treeItemDatas[i];
	}
	m_treeItemDatas.clear();
}

void CShowJsonView::FreeListItemDatas()
{
	for (int i = 0; i < m_listItemDatas.size(); i++)
	{
		delete m_listItemDatas[i];
	}
	m_listItemDatas.clear();
}

void CShowJsonView::BuildTree(LPJSONSTRUCT lpcs)
{
	HTREEITEM htiRoot;
	if (lpcs->m_jsonData->m_type == zson::JsonType::OBJECT)
	{
		htiRoot = m_treeCtrl.InsertItem(_T("Root"), 1, 1, TVI_ROOT, TVI_SORT);
		zson::JsonObject obj = *((std::shared_ptr<zson::JsonObject>&)(lpcs->m_jsonData->m_value));
		BuildObjTree(obj, htiRoot);
	}
	else // array
	{
		htiRoot = m_treeCtrl.InsertItem(_T("Root"), 0, 0, TVI_ROOT, TVI_SORT);
		zson::JsonArray arr = *((std::shared_ptr<zson::JsonArray>&)(lpcs->m_jsonData->m_value));
		BuildArrTree(arr, htiRoot);
	}
	// set item data
	LPJSONITEMDATA lpjidItem = new JSONITEMDATA;
	lpjidItem->m_jsonData = lpcs->m_jsonData;
	m_treeCtrl.SetItemData(htiRoot, (DWORD_PTR)lpjidItem);
	m_treeItemDatas.push_back(lpjidItem);
}

void CShowJsonView::BuildObjTree(const zson::JsonObject& pObject, HTREEITEM htiParent)
{
	zson::JsonType type;
	HTREEITEM htiChild = NULL;
	for (int i = 0; i < pObject.GetLength(); i++)
	{
		type = pObject.GetTypeAt(i);
		CString strField;
		switch (type)
		{
		case zson::JsonType::ARRAY:
			strField = CA2T(pObject.GetFieldAt(i).c_str());
			htiChild = m_treeCtrl.InsertItem(strField, 0, 0, htiParent);
			BuildArrTree(pObject.GetArrayAt(i), htiChild);
			break;
		case zson::JsonType::OBJECT:
			strField = CA2T(pObject.GetFieldAt(i).c_str());
			htiChild = m_treeCtrl.InsertItem(strField, 1, 1, htiParent);
			BuildObjTree(pObject.GetObjectAt(i), htiChild);
			break;
		case zson::JsonType::STRING:
			strField = CA2T(pObject.GetFieldAt(i).c_str());
			htiChild = m_treeCtrl.InsertItem(strField, 2, 2, htiParent);
			break;
		case zson::JsonType::BOOL:
			strField = CA2T(pObject.GetFieldAt(i).c_str());
			htiChild = m_treeCtrl.InsertItem(strField, 3, 3, htiParent);
			break;
		case zson::JsonType::INTEGER:
		case zson::JsonType::DECIMAL:
			strField = CA2T(pObject.GetFieldAt(i).c_str());
			htiChild = m_treeCtrl.InsertItem(strField, 4, 4, htiParent);
			break;
		case zson::JsonType::NUL:
			strField = CA2T(pObject.GetFieldAt(i).c_str());
			htiChild = m_treeCtrl.InsertItem(strField, 5, 5, htiParent);
			break;
		default:
			htiChild = NULL;
		}
		// set item data
		if (htiChild)
		{
			LPJSONITEMDATA lpjidItem = new JSONITEMDATA;
			lpjidItem->m_jsonData = pObject.GetAt(i);
			m_treeCtrl.SetItemData(htiChild, (DWORD_PTR)lpjidItem);
			m_treeItemDatas.push_back(lpjidItem);
		}
	}
}

void CShowJsonView::BuildArrTree(const zson::JsonArray& pArray, HTREEITEM htiParent)
{
	HTREEITEM htiChild = NULL;
	for (int i = 0; i < pArray.GetLength(); i++)
	{
		CString strField;
		zson::JsonType type = pArray.GetTypeAt(i);
		switch (type)
		{
		case zson::JsonType::ARRAY:
			strField.Format(_T("[%d]"), i);
			htiChild = m_treeCtrl.InsertItem(strField, 0, 0, htiParent);
			BuildArrTree(pArray.GetArrayAt(i), htiChild);
			break;
		case zson::JsonType::OBJECT:
			strField.Format(_T("[%d]"), i);
			htiChild = m_treeCtrl.InsertItem(strField, 1, 1, htiParent);
			BuildObjTree(pArray.GetObjectAt(i), htiChild);
			break;
		case zson::JsonType::STRING:
			strField.Format(_T("[%d]"), i);
			htiChild = m_treeCtrl.InsertItem(strField, 2, 2, htiParent);
			break;
		case zson::JsonType::BOOL:
			strField.Format(_T("[%d]"), i);
			htiChild = m_treeCtrl.InsertItem(strField, 3, 3, htiParent);
			break;
		case zson::JsonType::INTEGER:
		case zson::JsonType::DECIMAL:
			strField.Format(_T("[%d]"), i);
			htiChild = m_treeCtrl.InsertItem(strField, 4, 4, htiParent);
			break;
		case zson::JsonType::NUL:
			strField.Format(_T("[%d]"), i);
			htiChild = m_treeCtrl.InsertItem(strField, 5, 5, htiParent);
			break;
		default:
			htiChild = NULL;
		}

		// set item data
		if (htiChild)
		{
			LPJSONITEMDATA lpjidItem = new JSONITEMDATA;
			lpjidItem->m_jsonData = pArray.GetAt(i);
			m_treeCtrl.SetItemData(htiChild, (DWORD_PTR)lpjidItem);
			m_treeItemDatas.push_back(lpjidItem);
		}
	}
}

void CShowJsonView::OnTreeItemSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	
	*pResult = FALSE;
	if (hItem)
	{
		LPJSONITEMDATA lpjid = (LPJSONITEMDATA) m_treeCtrl.GetItemData(hItem);
		// add items to listctrl
		AddItemsToListCtrl(lpjid);
		// update status bar
		CWnd* pMainWnd = AfxGetMainWnd();
		pMainWnd->SendMessage(WM_USER_UPDATE_STATUSBAR, (WPARAM) (&(*(lpjid->m_jsonData))));
	}
}

void CShowJsonView::OnListItemClicked(NMHDR* pnmh, LRESULT* pResult)
{
	DWORD dwPos = ::GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));
	CPoint ptScreen = point;
	m_listCtrl.ScreenToClient(&point);
	
	int nIndex;
	if ((nIndex = m_listCtrl.HitTest(point)) != -1)
	{
		// highlight this line
		m_listCtrl.SetItemState(nIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		// save selected index
		m_nSelListItem = nIndex;
		// show the context menu
		CMenu* pMenu = m_menu.GetSubMenu(0);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, ptScreen.x, ptScreen.y, this);
	}
	else
	{
		m_nSelListItem = -1;
	}
	*pResult = 0;
}

void CShowJsonView::AddItemsToListCtrl(LPJSONITEMDATA lpjid)
{
	// clear listctrl
	m_listCtrl.DeleteAllItems();
	FreeListItemDatas();

	zson::JsonType type = lpjid->m_jsonData->m_type;
	if (type == zson::JsonType::ARRAY)
	{
		/* new feature on 2024 - 11 - 01: add self */
		// insert item
		m_listCtrl.InsertItem(0, _T("_self_"), 0);
		m_listCtrl.SetItemText(0, 1, _T("Array"));
		// set item data
		LPJSONITEMDATA lpItemDataSelf = new JSONITEMDATA;
		lpItemDataSelf->m_jsonData = lpjid->m_jsonData;
		m_listItemDatas.push_back(lpItemDataSelf);
		/* add self over */ 

		std::shared_ptr<zson::JsonArray>& pArray = (std::shared_ptr<zson::JsonArray>&)(lpjid->m_jsonData->m_value);
		for (int i = 0; i < (*pArray).GetLength(); i++)
		{
			CString strValue;
			zson::JsonType typeInside = (*pArray).GetTypeAt(i);
			CString strField;
			strField.Format(_T("[%d]"), i);
			int nImgIndex = 0;
			switch (typeInside)
			{
			case zson::JsonType::ARRAY:
				strValue = _T("Array");
				nImgIndex = 0;
				break;
			case zson::JsonType::OBJECT:
				strValue = _T("Object");
				nImgIndex = 1;
				break;
			case zson::JsonType::STRING:
				strValue = CA2T(pArray->GetStringAt(i).c_str());
				nImgIndex = 2;
				break;
			case zson::JsonType::BOOL:
				//strValue = pArray->GetBoolAt(i) ? "true" : "false";
				strValue = CA2T(pArray->GetStringAt(i).c_str());
				nImgIndex = 3;
				break;
			case zson::JsonType::INTEGER:
			case zson::JsonType::DECIMAL:
				//strValue.Format(L"%.2f", pArray->GetNumberAt(i));
				strValue = CA2T(pArray->GetStringAt(i).c_str());
				nImgIndex = 4;
				break;
			case zson::JsonType::NUL:
				strValue = L"null";
				nImgIndex = 5;
				break;
			}
			// insert item
			m_listCtrl.InsertItem(i + 1, strField, nImgIndex);
			m_listCtrl.SetItemText(i + 1, 1, strValue);
			// set item data
			LPJSONITEMDATA lpItemData = new JSONITEMDATA;
			lpItemData->m_jsonData = (*pArray).GetAt(i);
			m_listItemDatas.push_back(lpItemData);
		}
	}
	else if (type == zson::JsonType::OBJECT)
	{
		/* new feature on 2024 - 11 - 01: add self */
		// insert item
		m_listCtrl.InsertItem(0, _T("_self_"), 1);
		m_listCtrl.SetItemText(0, 1, _T("Object"));
		// set item data
		LPJSONITEMDATA lpItemDataSelf = new JSONITEMDATA;
		lpItemDataSelf->m_jsonData = lpjid->m_jsonData;
		m_listItemDatas.push_back(lpItemDataSelf);
		/* add self over */

		std::shared_ptr<zson::JsonObject>& pObject = (std::shared_ptr<zson::JsonObject>&)(lpjid->m_jsonData->m_value);
		for (int i = 0; i < (*pObject).GetLength(); i++)
		{
			CString strValue;
			zson::JsonType typeInside = (*pObject).GetTypeAt(i);
			CString strField = CA2T(pObject->GetFieldAt(i).c_str());
			int nImgIndex = 0;
			switch (typeInside)
			{
			case zson::JsonType::ARRAY:
				strValue = _T("Array");
				nImgIndex = 0;
				break;
			case zson::JsonType::OBJECT:
				strValue = _T("Object");
				nImgIndex = 1;
				break;
			case zson::JsonType::STRING:
				strValue = CA2T(pObject->GetStringAt(i).c_str());
				nImgIndex = 2;
				break;
			case zson::JsonType::BOOL:
				//strValue = pObject->GetBoolAt(i) ? "true" : "false";
				strValue = CA2T(pObject->GetStringAt(i).c_str());
				nImgIndex = 3;
				break;
			case zson::JsonType::INTEGER:
			case zson::JsonType::DECIMAL:
				//strValue.Format(L"%.2f", pObject->GetNumberAt(i));
				strValue = CA2T(pObject->GetStringAt(i).c_str());
				nImgIndex = 4;
				break;
			case zson::JsonType::NUL:
				strValue = L"null";
				nImgIndex = 5;
				break;
			}
			// insert item
			m_listCtrl.InsertItem(i + 1, strField, nImgIndex);
			m_listCtrl.SetItemText(i + 1, 1, strValue);
			// set item data
			LPJSONITEMDATA lpItemData = new JSONITEMDATA;
			lpItemData->m_jsonData = (*pObject).GetAt(i);
			m_listItemDatas.push_back(lpItemData);
		}
	}
	else if (type == zson::JsonType::STRING)
	{
		std::shared_ptr<std::string>& pStr = (std::shared_ptr<std::string>&)(lpjid->m_jsonData->m_value);
		CString strValue = CA2T(pStr->c_str());
		//CString strField = lpjid->m_jsonField;
		CString strField = CA2T(lpjid->m_jsonData->m_field.c_str());
		// insert item
		m_listCtrl.InsertItem(0, strField, 2);
		m_listCtrl.SetItemText(0, 1, strValue);
		// set item data
		LPJSONITEMDATA lpItemData = new JSONITEMDATA;
		lpItemData->m_jsonData = lpjid->m_jsonData;
		m_listItemDatas.push_back(lpItemData);
	}
	else if (type == zson::JsonType::BOOL)
	{
		auto pBool = (std::shared_ptr<std::string>&)(lpjid->m_jsonData->m_value);
		//CString strValue = *pBool ? L"true" : L"false";
		CString strValue = CA2T(pBool->c_str());
		CString strField = CA2T(lpjid->m_jsonData->m_field.c_str());
		// insert item
		m_listCtrl.InsertItem(0, strField, 3);
		m_listCtrl.SetItemText(0, 1, strValue);
		// set item data
		LPJSONITEMDATA lpItemData = new JSONITEMDATA;
		lpItemData->m_jsonData = lpjid->m_jsonData;
		m_listItemDatas.push_back(lpItemData);
	}
	else if (type == zson::JsonType::INTEGER || type == zson::JsonType::DECIMAL)
	{
		auto pNum = (std::shared_ptr<std::string>&)(lpjid->m_jsonData->m_value);
		CString strValue = CA2T(pNum->c_str());;
		//strValue.Format(L"%.2f", *pNum);
		CString strField = CA2T(lpjid->m_jsonData->m_field.c_str());
		// insert item
		m_listCtrl.InsertItem(0, strField, 4);
		m_listCtrl.SetItemText(0, 1, strValue);
		// set item data
		LPJSONITEMDATA lpItemData = new JSONITEMDATA;
		lpItemData->m_jsonData = lpjid->m_jsonData;
		m_listItemDatas.push_back(lpItemData);
	}
	else if (type == zson::JsonType::NUL)
	{
		CString strValue = L"null";
		CString strField = CA2T(lpjid->m_jsonData->m_field.c_str());
		// insert item
		m_listCtrl.InsertItem(0, strField, 5);
		m_listCtrl.SetItemText(0, 1, strValue);
		// set item data
		LPJSONITEMDATA lpItemData = new JSONITEMDATA;
		lpItemData->m_jsonData = lpjid->m_jsonData;
		m_listItemDatas.push_back(lpItemData);
	}
}


void CShowJsonView::OnCopyField()
{
	if (m_nSelListItem == -1) return;
	if (m_listItemDatas.empty()) return;

	CString source;
	source = CA2T(m_listItemDatas[m_nSelListItem]->m_jsonData->m_field.c_str());
	CopyDataToClipboard(source);
}

void CShowJsonView::OnCopyValue()
{
	if (m_nSelListItem == -1) return;
	if (m_listItemDatas.empty()) return;

	zson::JsonType type = m_listItemDatas[m_nSelListItem]->m_jsonData->m_type;
	CString source;
	std::shared_ptr<std::string> pstr;
	switch (type)
	{
	case zson::JsonType::ARRAY:
		pstr = zson::JsonParse::ReverseArray(*((std::shared_ptr<zson::JsonArray>&) m_listItemDatas[m_nSelListItem]->m_jsonData->m_value));
		source = CA2T(pstr->c_str());
		break;
	case zson::JsonType::OBJECT:
		pstr = zson::JsonParse::ReverseObject(*((std::shared_ptr<zson::JsonObject>&) m_listItemDatas[m_nSelListItem]->m_jsonData ->m_value));
		source = CA2T(pstr->c_str());
		break;
	case zson::JsonType::STRING:
		pstr = (std::shared_ptr<std::string>&) m_listItemDatas[m_nSelListItem]->m_jsonData->m_value;
		source = CA2T(pstr->c_str());
		break;
	case zson::JsonType::BOOL:
		//pbool = (std::shared_ptr<std>&) m_listItemDatas[m_nSelListItem]->m_jsonData;
		//source = (*pbool) ? L"true" : L"false";
		pstr = (std::shared_ptr<std::string>&) m_listItemDatas[m_nSelListItem]->m_jsonData->m_value;
		source = CA2T(pstr->c_str());
		break;
	case zson::JsonType::INTEGER:
	case zson::JsonType::DECIMAL:
		/*pnum = (std::shared_ptr<double>&) m_listItemDatas[m_nSelListItem]->m_jsonData;
		source.Format(L"%.2f", *pnum);*/
		pstr = (std::shared_ptr<std::string>&) m_listItemDatas[m_nSelListItem]->m_jsonData->m_value;
		source = CA2T(pstr->c_str());
		break;
	case zson::JsonType::NUL:
		source = L"null";
		break;
	}

	CopyDataToClipboard(source);
}

void CShowJsonView::CopyDataToClipboard(CString& source)
{
	if (source.IsEmpty()) return;
	if (!::OpenClipboard(NULL)) return;

	::EmptyClipboard();
	int len = source.GetLength();
	int size = (len + 1) * sizeof(TCHAR);
	HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, size);
	if (!clipbuffer)
	{
		::CloseClipboard();
		return;
	}
	char* buffer = (char*)::GlobalLock(clipbuffer);
	memcpy(buffer, source.GetBuffer(), size);
	::GlobalUnlock(clipbuffer);
	::SetClipboardData(sizeof(TCHAR) == sizeof(WCHAR) ? CF_UNICODETEXT : CF_TEXT, clipbuffer);
	::CloseClipboard();
}