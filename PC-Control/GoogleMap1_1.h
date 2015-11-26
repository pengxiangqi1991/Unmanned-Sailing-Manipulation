// GoogleMap1_1.h : main header file for the GOOGLEMAP1_1 application
//

#if !defined(AFX_GOOGLEMAP1_1_H__79A1E9E3_40FD_4A5A_B0FB_DEF8F5623E27__INCLUDED_)
#define AFX_GOOGLEMAP1_1_H__79A1E9E3_40FD_4A5A_B0FB_DEF8F5623E27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CGoogleMap1_1App:
// See GoogleMap1_1.cpp for the implementation of this class
//

class CGoogleMap1_1App : public CWinApp
{
public:
	CGoogleMap1_1App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGoogleMap1_1App)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGoogleMap1_1App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void KillProcessByName(CString strProcesName);//根据进程的名字杀死进程
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOOGLEMAP1_1_H__79A1E9E3_40FD_4A5A_B0FB_DEF8F5623E27__INCLUDED_)
