
// Zson.h : main header file for the Zson application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CZsonApp:
// See Zson.cpp for the implementation of this class
//

class CZsonApp : public CWinApp
{
public:
	CZsonApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CZsonApp theApp;
