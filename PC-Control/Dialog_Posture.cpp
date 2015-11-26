// Dialog_Posture.cpp : implementation file
//

#include "stdafx.h"
#include "GoogleMap1_1.h"
#include "Dialog_Posture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialog_Posture dialog


CDialog_Posture::CDialog_Posture(CWnd* pParent /*=NULL*/)
	: CDialog(CDialog_Posture::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialog_Posture)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialog_Posture::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialog_Posture)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog_Posture, CDialog)
	//{{AFX_MSG_MAP(CDialog_Posture)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog_Posture message handlers
