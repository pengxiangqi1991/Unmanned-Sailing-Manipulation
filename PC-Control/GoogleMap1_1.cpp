// GoogleMap1_1.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GoogleMap1_1.h"
#include "GoogleMap1_1Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGoogleMap1_1App

BEGIN_MESSAGE_MAP(CGoogleMap1_1App, CWinApp)
	//{{AFX_MSG_MAP(CGoogleMap1_1App)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGoogleMap1_1App construction

CGoogleMap1_1App::CGoogleMap1_1App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGoogleMap1_1App object

CGoogleMap1_1App theApp;

/////////////////////////////////////////////////////////////////////////////
// CGoogleMap1_1App initialization

BOOL CGoogleMap1_1App::InitInstance()
{
	//加载套接字库
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	
	wVersionRequested = MAKEWORD(2,2);
	
	err = WSAStartup(wVersionRequested,&wsaData);
	if(err!=0)
	{
		AfxMessageBox("err!=0");
		return false;
	}
	
	if(LOBYTE(wsaData.wVersion)!=2||HIBYTE(wsaData.wVersion!=2))
	{
		AfxMessageBox("套接字2加载不成功！");
		WSACleanup();
		return false;
	}
	AfxEnableControlContainer();
	
	::CoInitialize(NULL); //初始化COM库

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CGoogleMap1_1Dlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CGoogleMap1_1App::KillProcessByName(CString strProcesName)
{
	CString strTemp;
	
	HANDLE hSnapshot ; 
	hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, 0 ) ; 
	if ( hSnapshot == INVALID_HANDLE_VALUE)  
		return; 
	
	PROCESSENTRY32 pe; 
	pe.dwSize = sizeof ( PROCESSENTRY32 ); 
	
	for( BOOL fOk = Process32First ( hSnapshot, &pe ) ; fOk;  fOk = 
		Process32Next( hSnapshot, &pe ) ) 
	{ 
		strTemp=pe.szExeFile;
		strTemp.MakeLower();//将快照的进程名称变小写
		strProcesName.MakeLower();//将要杀死的进程名字变小写
		
		if(strTemp==strProcesName)
		{
			TerminateProcess(OpenProcess(PROCESS_ALL_ACCESS,false,pe.th32ProcessID),0);
			return;
		}
	}
	return;
}

int CGoogleMap1_1App::ExitInstance()
{
	CoUninitialize();//卸载COM库
	KillProcessByName("googleearth.exe");	
	return CWinApp::ExitInstance();
}
