#if !defined(AFX_GOOGLEEARTHWND_H__359E1F91_8F04_41C8_A536_214EBF65FDBD__INCLUDED_)
#define AFX_GOOGLEEARTHWND_H__359E1F91_8F04_41C8_A536_214EBF65FDBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GoogleEarthWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGoogleEarthWnd window

#define MYWND_CLASS    _T("MFCMyWndClass")

class CGoogleEarthWnd : public CWnd
{
// Construction
public:
	CGoogleEarthWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGoogleEarthWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGoogleEarthWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGoogleEarthWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	BOOL RegisterWindowClass();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOOGLEEARTHWND_H__359E1F91_8F04_41C8_A536_214EBF65FDBD__INCLUDED_)
