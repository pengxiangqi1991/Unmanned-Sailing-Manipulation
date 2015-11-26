// GoogleMap1_1Dlg.h : header file
//

#if !defined(AFX_GOOGLEMAP1_1DLG_H__63A56F41_D7B6_405B_8BBE_8F7C7D9432A7__INCLUDED_)
#define AFX_GOOGLEMAP1_1DLG_H__63A56F41_D7B6_405B_8BBE_8F7C7D9432A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CGoogleMap1_1Dlg dialog

#include "googleearth.h"
#include "GoogleEarthWnd.h"
#include "CSpreadSheet.h" //excel

#define UM_LISTEN		WM_USER+1
#define UM_SOCK		    WM_USER+2

class CGoogleMap1_1Dlg : public CDialog
{
// Construction
public:
	CGoogleMap1_1Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CGoogleMap1_1Dlg)
	enum { IDD = IDD_GOOGLEMAP1_1_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGoogleMap1_1Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CGoogleMap1_1Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnBtnPos();
	afx_msg void OnSock(WPARAM wParam,LPARAM lParam);
	afx_msg void OnListen(WPARAM wParam,LPARAM lParam);
	afx_msg void OnBtnSend();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnButton1();
	afx_msg void OnButtonPosture();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	void setExcel(CString slon,CString slat,CString sCompass,CString sGyro);
	CString sPos2Pos;
	void SetPos2Pos(CString sLonAndLat);
	void SetPostureBMP(CString strPosture);
	void SetPosKml(CString slon,CString slat);
	double LatitudeConvert(CString strLat1,CString strLat2,CString strLat3);
	double LongitudeConvert(CString strLon1,CString strLon2,CString strLon3);
	void SetGooglePos(CString strRecv);
	
	BOOL InitSocket();
	void CloseSock(SOCKET sock);
	int lon1;
	int lon2;
	int lon3;
	int lat1;
	int lat2;
	int lat3;
	IApplicationGE m_geApplication;
	void GetGoogleEarthWindow();
	void MoveGoogleEarthWindow();

	CBitmap m_cBmp;
	CStatic	m_ctrBmp;
	CString strSend;


private:
	SOCKET m_socket; //监听套接字
	SOCKET sock; //收发数据套接字
protected:
	CGoogleEarthWnd m_googleEarthWnd;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GOOGLEMAP1_1DLG_H__63A56F41_D7B6_405B_8BBE_8F7C7D9432A7__INCLUDED_)
