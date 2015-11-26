#if !defined(AFX_DIALOG_POSTURE_H__E319287F_82F7_4354_84D5_2D040E640E43__INCLUDED_)
#define AFX_DIALOG_POSTURE_H__E319287F_82F7_4354_84D5_2D040E640E43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialog_Posture.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialog_Posture dialog

class CDialog_Posture : public CDialog
{
// Construction
public:
	CDialog_Posture(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialog_Posture)
	enum { IDD = IDD_DIALOG_POSTURE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialog_Posture)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialog_Posture)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOG_POSTURE_H__E319287F_82F7_4354_84D5_2D040E640E43__INCLUDED_)
