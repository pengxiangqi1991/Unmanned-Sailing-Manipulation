// GoogleMap1_1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "GoogleMap1_1.h"
#include "GoogleMap1_1Dlg.h"
#include "Dialog_Posture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
static const CLSID CLSID_ApplicationGE = {0x8097D7E9,0xDB9E,0x4AEF, {0x9B,0x28,0x61,0xD8,0x2A,0x1D,0xF7,0x84}};

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGoogleMap1_1Dlg dialog

CGoogleMap1_1Dlg::CGoogleMap1_1Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGoogleMap1_1Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGoogleMap1_1Dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGoogleMap1_1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGoogleMap1_1Dlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_CUSTOM, m_googleEarthWnd);
	DDX_Control(pDX, IDC_STATIC_BMP, m_ctrBmp);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGoogleMap1_1Dlg, CDialog)
	//{{AFX_MSG_MAP(CGoogleMap1_1Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_POS, OnBtnPos)
	ON_MESSAGE(UM_SOCK,OnSock) //UM_SOCK��Ϣӳ�� UM_SOCK����OnSock()����
	ON_MESSAGE(UM_LISTEN,OnListen)
	ON_BN_CLICKED(IDC_BTN_SEND, OnBtnSend)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON_POSTURE, OnButtonPosture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGoogleMap1_1Dlg message handlers

BOOL CGoogleMap1_1Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	GetGoogleEarthWindow();  //�ȸ�����ʼ��
	MoveGoogleEarthWindow(); //�ƶ��ȸ���򴰿�
	InitSocket(); //�׽��ֳ�ʼ��
	strSend="";
	sPos2Pos="";


	//CSpreadSheet excel("E:\\C++\\test\\Excel\\gps.xls","gpsSheet");
	CSpreadSheet excel("E:\\gps.xls","gpsSheet");
	CStringArray excelArray;
	excel.BeginTransaction();
	excelArray.RemoveAll();
	excelArray.Add("����");
	excelArray.Add("γ��");
	excelArray.Add("ָ����");
	excelArray.Add("������");
	excel.AddHeaders(excelArray);
	excel.Commit();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGoogleMap1_1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGoogleMap1_1Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGoogleMap1_1Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CGoogleMap1_1Dlg::GetGoogleEarthWindow()
{
	m_geApplication.CreateDispatch(CLSID_ApplicationGE);
	Sleep(1000);
	
	OLE_HANDLE mainwnd = m_geApplication.GetMainHwnd();
	
	
	while(!::FindWindow(NULL,"Google Earth"));
	
	::SetWindowPos((HWND)mainwnd,HWND_BOTTOM,0,0,0,0,SWP_NOSIZE|SWP_HIDEWINDOW);//��GE���������ɼ���Χ֮��
	OLE_HANDLE renderhwnd = m_geApplication.GetRenderHwnd();
	while(!renderhwnd)
	{
		renderhwnd = m_geApplication.GetRenderHwnd();
	}
	
	::SetParent((HWND)renderhwnd, m_googleEarthWnd.GetSafeHwnd());
	
	CenterWindow();	
}

void CGoogleMap1_1Dlg::MoveGoogleEarthWindow()
{
	int iYCaption = GetSystemMetrics(SM_CYCAPTION);
	int iYMenu = GetSystemMetrics(SM_CYMENU);
	int iXBorder = GetSystemMetrics(SM_CXEDGE);
	int iYBorder = GetSystemMetrics(SM_CYEDGE);
	HWND hGEWnd,hWidgetWnd,hMainWindowHSplitterWnd,hLeftPanelVSplitterWnd,hMain_StackWnd;
	
	hGEWnd=::FindWindow(NULL,"Google Earth");	
	
	hWidgetWnd=FindWindowEx(hGEWnd,NULL,NULL,"widget");
	
	hMainWindowHSplitterWnd=FindWindowEx(hWidgetWnd,NULL,NULL,"MainWindowHSplitter");
	
	hLeftPanelVSplitterWnd=FindWindowEx(hMainWindowHSplitterWnd,NULL,NULL,"LeftPanelVSplitter");
	hMain_StackWnd=FindWindowEx(hMainWindowHSplitterWnd,NULL,NULL,"main_stack_");
	
	CRect rectCustom;
	m_googleEarthWnd.GetWindowRect(&rectCustom);
	
	CWnd *pWndLeftPanel;
	pWndLeftPanel=CWnd::FromHandle(hLeftPanelVSplitterWnd); 
	CRect rectLeftPanel;
	pWndLeftPanel->GetWindowRect(&rectLeftPanel);
	
	CWnd *pWndMain_Stack;
	pWndMain_Stack=CWnd::FromHandle(hMain_StackWnd); 
	CRect rectMain_Stack;
	pWndMain_Stack->GetWindowRect(&rectMain_Stack);
	
	OLE_HANDLE mainwnd = m_geApplication.GetMainHwnd();
	CWnd *pMain=CWnd::FromHandle((HWND)mainwnd);
	CRect rectGE;
	pMain->GetWindowRect(&rectGE);
	
	int iXMainWindow,iYMainWindow;
	
	OLE_HANDLE renderhwnd = m_geApplication.GetRenderHwnd();
	CWnd *pRender=CWnd::FromHandle((HWND)renderhwnd);
	CRect rectRender;
	pRender->GetWindowRect(&rectRender);
	
	iXMainWindow = rectLeftPanel.Width() + rectCustom.Width() + 2*iXBorder +5;
	if(rectLeftPanel.Height()==rectRender.Height())
		iYMainWindow = rectCustom.Height() + iYCaption + iYMenu + 8;
	else
		iYMainWindow = rectCustom.Height() + iYCaption + iYMenu + 8 +25;
	
	pMain->MoveWindow(0,0,iXMainWindow,iYMainWindow);
	
}

void CGoogleMap1_1Dlg::OnDestroy()
{
	CDialog::OnDestroy();
	m_geApplication.ReleaseDispatch();
}

void CGoogleMap1_1Dlg::OnBtnPos() 
{
	// TODO: Add your control notification handler code here
	double dlon,dlat;
	int ilon,ilat;
	//���峣��GEPI�����ڰѾ�γ��ת��ΪС����ʽ
	const double GEPI=3600.0;
	char chlon1[10],chlon2[10],chlon3[10];
	char chlat1[10],chlat2[10],chlat3[10];
	//���ı��༭���е����ݴ��ݸ��ַ�������
	GetDlgItem(IDC_EDIT1)->GetWindowText(chlon1,10);
	GetDlgItem(IDC_EDIT2)->GetWindowText(chlon2,10);
	GetDlgItem(IDC_EDIT3)->GetWindowText(chlon3,10);
	GetDlgItem(IDC_EDIT4)->GetWindowText(chlat1,10);
	GetDlgItem(IDC_EDIT5)->GetWindowText(chlat2,10);
	GetDlgItem(IDC_EDIT6)->GetWindowText(chlat3,10);
	//���ַ�������ת��Ϊ��������
	lon1=atoi(chlon1);
	lon2=atoi(chlon2);
	lon3=atoi(chlon3);
	lat1=atoi(chlat1);
	lat2=atoi(chlat2);
	lat3=atoi(chlat3);
	//���ı��༭���������м��
	if(abs(lon1)>179)
	{
		AfxMessageBox("�����ȡ��Ƿ����룬������-180��180֮���������");
	}
	if(abs(lat1)>89)
	{
		AfxMessageBox("��γ�ȡ��Ƿ����룬������-90��90֮���������");
	}
	if(lon2>59 || lon2<0 || lat2>59 || lat2<0)
	{
		AfxMessageBox("���֡��Ƿ����룬������0��60֮���������");
	}
	if(lon3>59 || lon3<0 || lat3>59 || lat3<0)
	{
		AfxMessageBox("���롱�Ƿ����룬������0��60֮���������");
	}
	//������Ϸ����ݵ�ǰ���½��е�ͼ��λ����
	if(abs(lon1)<=179 && abs(lat1)<=89 && lon2<=59 && lon2>=0 && lon3<=59 && lat2<=59
		&& lat2>=0 && lat3<=59 && lat3>=0)
	{
		//����γ�ȵ�������ʽת��ΪС����ʽ
		ilon=lon1*3600+lon2*60+lon3;
		dlon=ilon/GEPI;
		ilat=lat1*3600+lat2*60+lat3;
		dlat=ilat/GEPI;
		//����SetCameraParams()����
		m_geApplication.SetCameraParams(dlat,dlon,1050.0,1,513,1.0,0.0,2.0);
	}
}

void CGoogleMap1_1Dlg::CloseSock(SOCKET sock)
{
	closesocket(sock);
}

BOOL CGoogleMap1_1Dlg::InitSocket()
{
	m_socket=WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,0); //SOCK_STREAM�������ݰ������׽��� TCPЭ��
	if(INVALID_SOCKET==m_socket)
	{
		MessageBox("�����׽���ʧ�ܣ�");
		return false;
	}
	SOCKADDR_IN addrSrv; //�����������˵�ַ
	addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(12345); //�������˵�ַ����˿ں�
	
	//���׽���
	if(SOCKET_ERROR==bind(m_socket,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR)))
	{
		closesocket(m_socket);
		MessageBox("��ʧ�ܣ�");
		return false;
		//closesocket(m_socket);
	}
	//���׽�������Ϊ����ģʽ ׼�����տͻ�����
	if(SOCKET_ERROR==listen(m_socket,50))
	{
		AfxMessageBox("�׽��ּ���ʧ��!");
		return false;
	}
	if(SOCKET_ERROR==WSAAsyncSelect(m_socket,m_hWnd,UM_LISTEN,FD_ACCEPT))
	{
		AfxMessageBox("ע�������¼�ʧ��!");
		return false;
	}
	GetDlgItem(IDC_STATIC_ADDR)->SetWindowText("�����������Ѿ�������");
	return true;
}

void CGoogleMap1_1Dlg::OnListen(WPARAM wParam, LPARAM lParam)
{
	SOCKADDR_IN addrClient;
	int len=sizeof(SOCKADDR);
	switch(LOWORD(lParam))
	{
	case FD_ACCEPT:
		sock=accept(m_socket,(SOCKADDR*)&addrClient,&len);
		CString strIP;
		strIP.Format(_T("�ͻ���IP��ַ��%s"),inet_ntoa(addrClient.sin_addr));
		SetDlgItemText(IDC_STATIC_ADDR,strIP);
		WSAAsyncSelect(sock,m_hWnd,UM_SOCK,FD_READ|FD_CLOSE);
		break;
	}
}

void CGoogleMap1_1Dlg::OnSock(WPARAM wParam, LPARAM lParam)
{
	CString strRecv;
	char chBuf[200];
	memset(chBuf,0,200);
	
	CString str;
	CString strTemp;
	
	switch(LOWORD(lParam))
	{
	case FD_READ:
		{
			recv(wParam,chBuf,sizeof(chBuf),0);
			strRecv=chBuf;
			SetGooglePos(strRecv);
			break;
		}
	case FD_CLOSE:
		CloseSock(wParam);
		break;
	}
}

void CGoogleMap1_1Dlg::SetGooglePos(CString strRecv)
{
	SetDlgItemText(IDC_STATIC_RECV,strRecv);
	int iEnd;
	iEnd=strRecv.Find("!"); //��ȡ����λ��
	strRecv=strRecv.Left(iEnd); //ȡ����ȷ�ַ���

	int x;
	x=strRecv.Find(':'); //��ÿ�ʼλ��
	strRecv=strRecv.Right(strRecv.GetLength()-x-1); //��ȡȫ����Ϣ
	
	int iPosture;
	iPosture=strRecv.Find('D'); //�����̬��Ϣλ��
	CString strPosture;
	strPosture=strRecv.Right(strRecv.GetLength()-iPosture-1); //��ȡ��̬��Ϣ
	SetDlgItemText(IDC_STATIC_POSTURE,strPosture);
	//SetPostureBMP(strPosture); //������̬��Ϣ��ʾ
	
	int iC; //��̬��Ϣ�ָ�
	CString sCompass;
	iC=strPosture.Find("C"); //��ȡָ������Ϣλ��
	sCompass=strPosture.Right(strPosture.GetLength()-iC-1); //��ȡָ������Ϣ
	SetDlgItemText(IDC_STATIC_C,sCompass);
	//SetDlgItemText(IDC_STATIC_D_C,sCompass);
	CString sGyro;
	sGyro=strPosture.Left(iC); //��ȡ��������Ϣ
	SetDlgItemText(IDC_STATIC_D,sGyro);
	//SetDlgItemText(IDC_STATIC_D_D,sGyro);
	
	
	CString sLatAndLon;
	sLatAndLon=strRecv.Left(iPosture); //��ȡ��γ����Ϣ
	CString strLat,strLon;
	CString slat1,slat2,slat3,slon1,slon2,slon3;
	CString slatLeft,slonLeft;
	int latorlon,ilat,ilon;
	latorlon=sLatAndLon.Find(','); 
	strLat=sLatAndLon.Left(latorlon); //��ȡγ����Ϣ
	strLon=sLatAndLon.Right(sLatAndLon.GetLength()-latorlon-1); //��ȡ������Ϣ
	SetDlgItemText(IDC_LAT,strLat); //��ʾ��ȡγ����Ϣ
	SetDlgItemText(IDC_LON,strLon); //��ʾ��ȡ������Ϣ
	
	ilat=strLat.Find('.'); //γ�ȷָ��־λ
	slat1=strLat.Left(2); //��ȡγ�� ��
	SetDlgItemText(IDC_STATIC_LAT1,slat1);
	slatLeft=strLat.Left(ilat);
	slat2=slatLeft.Right(2); //��ȡγ�� ��
	SetDlgItemText(IDC_STATIC_LAT2,slat2);
	slat3=strLat.Right(strLat.GetLength()-ilat-1); //��ȡγ�� ��
	SetDlgItemText(IDC_STATIC_LAT3,slat3);

	ilon=strLon.Find('.'); //���ȷָ��־λ
	slon1=strLon.Left(3); //��ȡ���� ��
	SetDlgItemText(IDC_STATIC_LON1,slon1);
	slonLeft=strLon.Left(ilon); 
	slon2=slonLeft.Right(2); //��ȡ���� ��
	SetDlgItemText(IDC_STATIC_LON2,slon2);
	slon3=strLon.Right(strLon.GetLength()-ilon-1); //��ȡ���� ��
	SetDlgItemText(IDC_STATIC_LON3,slon3);

	//��ȡ�����γ����Ϣ
	double dlon,dlat;
	dlon=LongitudeConvert(slon1,slon2,slon3);
	dlat=LatitudeConvert(slat1,slat2,slat3);

	CString slon,slat;
	slon.Format("%f",dlon); //����
	slat.Format("%f",dlat); //γ��
	SetDlgItemText(IDC_STATIC_SLON,slon);
	SetDlgItemText(IDC_STATIC_SLAT,slat);
	//SetPosKml(slon,slat);


	//sPos2Pos+=slon+","+slat+","+"0"+" ";
	SetDlgItemText(IDC_STATIC_POS2POS,sPos2Pos);
	//SetPos2Pos(sPos2Pos);

	//sCompassָ���� sGyro������ slon���� slatγ��
	//setExcel(slon,slat,sCompass,sGyro);

	if(dlon<122&&dlon>120&&dlat<40&&dlat>38)
	{
		SetPosKml(slon,slat);
		sPos2Pos+=slon+","+slat+","+"0"+" ";
		SetPos2Pos(sPos2Pos);
		setExcel(slon,slat,sCompass,sGyro);
	}
}

double CGoogleMap1_1Dlg::LongitudeConvert(CString strLon1, CString strLon2, CString strLon3)
{
	double dlon;
	const double GEPI=3600.0;
	double dlon3,lon1,lon2,lon3;
	CString d2slon3;
	
	lon1=atof(strLon1);
	lon2=atof(strLon2);
	lon3=atof(strLon3);
	dlon3=lon3/100000*60;
	d2slon3.Format(_T("%.4lf"),dlon3);
	SetDlgItemText(IDC_STATIC_DLON3,d2slon3);
	if(lon1>179 || lon1 <-179)
	{
		AfxMessageBox("�����ȡ��Ƿ����룬������-180��180֮���������");
	}
	if(lon2>59 || lon2<0)
	{
		AfxMessageBox("�����ȷ֡��Ƿ����룬������0��60֮���������");
	}
	if(dlon3>59 || dlon3<0)
	{
		AfxMessageBox("�������롱�Ƿ����룬������0��60֮���������");
	}
	if(lon1<=179 || lon1>=-179 && lon2<=59 && lon2>=0 && dlon3<=59 && dlon3>=0)
	{
		dlon=lon1*3600+lon2*60+dlon3;
		dlon=dlon/GEPI;
	}
	return dlon;
}

double CGoogleMap1_1Dlg::LatitudeConvert(CString strLat1, CString strLat2, CString strLat3)
{
	double dlat;
	double dlat3,lat1,lat2,lat3;
	const double GEPI=3600.0;
	//���ַ�������ת��Ϊ��������
	lat1=atof(strLat1);
	lat2=atof(strLat2);
	lat3=atof(strLat3);
	dlat3=lat3/100000*60;
	CString d2slat3;
	d2slat3.Format(_T("%.4lf"),dlat3);
	SetDlgItemText(IDC_STATIC_DLAT3,d2slat3);
	//������γ�Ƚ��м��
	if(lat1>89 || lat1<-89)
	{
		AfxMessageBox("��γ�ȡ��Ƿ����룬������-90��90֮���������");
	}
	if(lat2>59 || lat2<0)
	{
		AfxMessageBox("��γ�ȷ֡��Ƿ����룬������0��60֮���������");
	}
	
	if(dlat3>59 || dlat3<0)
	{
		AfxMessageBox("��γ���롱�Ƿ����룬������0��60֮���������");
	}
	
	//������Ϸ����ݵ�ǰ���½��л���
	if(lat1<=89 || lat1>-89 && lat2<=59 && lat2>=0 && dlat3<=59 && dlat3>=0)
	{
		//����γ�ȵ�������ʽת��ΪС����ʽ
		dlat=lat1*3600+lat2*60+dlat3;
		dlat=dlat/GEPI;
	}
	return dlat;
}

void CGoogleMap1_1Dlg::SetPosKml(CString slon, CString slat)
{
	char CurrentPath[MAX_PATH];
	LPSTR FileCurrentName;	
	
	GetCurrentDirectory(MAX_PATH,CurrentPath);
	FileCurrentName=lstrcat(CurrentPath, "\\pos.kml");
	
	CFile file(FileCurrentName,CFile::modeCreate | CFile::modeWrite);
	
	CString strKml;
	strKml = "<?xml version=\"1.0\" standalone=\"yes\"?>";
	strKml += "<kml xmlns=\"\">";
	strKml += "<Document>";
	strKml += "<Placemark xmlns=\"\">";
	strKml += "<name>pos</name>";
	strKml += "<LookAt>";
	strKml += "<longitude>" + slon + "</longitude>";
	strKml += "<latitude>" + slat + "</latitude>";
	strKml += "<range>300</range>";
	strKml += "</LookAt>";
	strKml += "<Point>";
	strKml += "<coordinates>" + slon + "," + slat + "," + "0" + "</coordinates>";
	strKml += "</Point>";
	strKml += "</Placemark>";
	strKml += "</Document>";
	strKml += "</kml>";
	
	file.Write(strKml,strKml.GetLength());
	file.Close();
	m_geApplication.OpenKmlFile(FileCurrentName,0);
}

void CGoogleMap1_1Dlg::SetPos2Pos(CString sLonAndLat)
{
	char CurrentPath[MAX_PATH];
	LPSTR FileCurrentName;	
	
	GetCurrentDirectory(MAX_PATH,CurrentPath);
	FileCurrentName=lstrcat(CurrentPath, "\\pos2pos.kml");
	
	CFile file(FileCurrentName,CFile::modeCreate | CFile::modeWrite);
	
	CString strKml;
	strKml = "<?xml version=\"1.0\" standalone=\"yes\"?>";
	strKml += "<kml xmlns=\"\">";
	strKml += "<Document>";
	strKml += "<Style id=\"s1\">";
	strKml += "<LineStyle><color>ff00ff55</color><width>3</width></LineStyle>";
	strKml += "</Style>";
	strKml += "<Style id=\"s2\">";
	strKml += "<LineStyle><color>ff00ff55</color><width>3</width></LineStyle>";
	strKml += "</Style>";
	strKml += "<StyleMap id=\"msn_ylw-pushpin_copy18\">";
	strKml += "<Pair><key>normal</key><styleUrl>#s1</styleUrl></Pair>";
	strKml += "<Pair><key>highlight</key><styleUrl>#s2</styleUrl></Pair>";
	strKml += "</StyleMap>";
	strKml += "<Placemark xmlns=\"\">";
	strKml += "<name>pos2pos</name>";
	strKml += "<styleUrl>#msn_ylw-pushpin_copy18</styleUrl>";
	strKml += "<LineString>";
	strKml += "<tessellate>1</tessellate>";
	strKml += "<coordinates>" + sLonAndLat + "</coordinates>";
	strKml += "</LineString>";
	strKml += "</Placemark>";
	strKml += "</Document>";
	strKml += "</kml>";

	
	file.Write(strKml,strKml.GetLength());
	file.Close();
	m_geApplication.OpenKmlFile(FileCurrentName,0);
}

void CGoogleMap1_1Dlg::SetPostureBMP(CString strPosture)
{
	//CString strIDBBmp=_T("IDB_BMP");
	int iPosture=atoi(strPosture);
	if (iPosture==1)
	{
		SetDlgItemText(IDC_STATIC_POS,strPosture);
		m_cBmp.DeleteObject();  //���ڴ���ɾ����m_cBmp������GDI�ṹ
		m_cBmp.LoadBitmap(IDB_BITMAP1);   //����λͼ1
		m_ctrBmp.SetBitmap(HBITMAP(m_cBmp)); //��ͼƬ�ؼ�����ʾλͼ1
	}
	if (iPosture==2)
	{
		SetDlgItemText(IDC_STATIC_POS,strPosture);
		m_cBmp.DeleteObject();  //���ڴ���ɾ����m_cBmp������GDI�ṹ
		m_cBmp.LoadBitmap(IDB_BITMAP2);   //����λͼ2
		m_ctrBmp.SetBitmap(HBITMAP(m_cBmp)); //��ͼƬ�ؼ�����ʾλͼ2
	}
}

void CGoogleMap1_1Dlg::OnBtnSend() 
{
	// TODO: Add your control notification handler code here
	if (strSend=="")
	{
		AfxMessageBox("��ѡ��Ҫ���͵Ŀ�����Ϣ��");
	}
	else
	{
		if (::send(sock,strSend,sizeof(strSend),0)!=SOCKET_ERROR)
		{
			GetDlgItem(IDC_STATIC_TEXT)->SetWindowText("������Ϣ�Ѿ����͵��ͻ��ˣ�");
		} 
		else
		{
			GetDlgItem(IDC_STATIC_TEXT)->SetWindowText("������Ϣ����ʧ�ܣ�");
		}
	}
}

void CGoogleMap1_1Dlg::OnRadio1() 
{
	// TODO: Add your control notification handler code here
	GetDlgItemText(IDC_RADIO1,strSend);
	GetDlgItem(IDC_STATIC_STRSEND)->SetWindowText(strSend);
}

void CGoogleMap1_1Dlg::OnRadio2() 
{
	// TODO: Add your control notification handler code here
	GetDlgItemText(IDC_RADIO2,strSend);
	GetDlgItem(IDC_STATIC_STRSEND)->SetWindowText(strSend);
}

void CGoogleMap1_1Dlg::OnRadio3() 
{
	// TODO: Add your control notification handler code here
	GetDlgItemText(IDC_RADIO3,strSend);
	GetDlgItem(IDC_STATIC_STRSEND)->SetWindowText(strSend);
}

void CGoogleMap1_1Dlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	m_geApplication.SetCameraParams(38.870444,121.530197,1050.0,1,513,1.0,0.0,2.0);
}



void CGoogleMap1_1Dlg::OnButtonPosture() 
{
	// TODO: Add your control notification handler code here
	CDialog_Posture cPosture;
	cPosture.DoModal();
}

void CGoogleMap1_1Dlg::setExcel(CString slon, CString slat, CString sCompass, CString sGyro)
{
	//CSpreadSheet excel("E:\\C++\\test\\Excel\\gps.xls","gpsSheet");
	CSpreadSheet excel("E:\\gps.xls","gpsSheet");
	CStringArray excelArray;
	excel.BeginTransaction();
	excelArray.RemoveAll();
	excelArray.Add(slon);
	excelArray.Add(slat);
	excelArray.Add(sCompass);
	excelArray.Add(sGyro);
	excel.AddRow(excelArray);
	excel.Commit();	
}
