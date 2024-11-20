#pragma once

#include <memory>
#include "json_core.h"

#define IDC_LIST_VIEW     401
#define IDC_TREE_VIEW     402
#define ID_HINT_SEND_JSON  0X5A
#define WM_USER_UPDATE_STATUSBAR	WM_USER + 1001


typedef struct {
	std::shared_ptr<zson::JsonElement> m_jsonData;
	//zson::JsonType m_jsonType;
} JSONSTRUCT, *LPJSONSTRUCT;

typedef struct {
	/*CString m_jsonField;
	zson::JsonType m_jsonType;
	std::shared_ptr<void> m_jsonData;*/
	std::shared_ptr<zson::JsonElement> m_jsonData;
} JSONITEMDATA, *LPJSONITEMDATA;

