
// FAManagementDlg.cpp : 구현 파일
//
#pragma once
#include "stdafx.h"
#include "FAManagement.h"
#include "FAManagementDlg.h"
#include "UserNoticeDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG      
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
	HICON m_hIcon;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{	
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CFAManagementDlg 대화 상자

CFAManagementDlg::CFAManagementDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFAManagementDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CFAManagementDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGIN, m_LOGIN);
}

BEGIN_MESSAGE_MAP(CFAManagementDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()




	ON_MESSAGE(WM_TRAYICON,OnTrayIcon)
	ON_COMMAND(WM_DIALOG_SHOW,OnDialogShow)	
	ON_COMMAND(WM_ABOUTBOX, OnCreatorInfo)
	ON_COMMAND(WM_NOTICE, OnCreatorNotice)
	ON_COMMAND(WM_NONE, OnNONE)
	ON_COMMAND(WM_LOGOUT, OnLOGOUT)
	ON_COMMAND(WM_APP_EXIT,OnAppExit)
	ON_WM_DESTROY()




	ON_BN_CLICKED(IDOK, &CFAManagementDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CFAManagementDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CFAManagementDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CFAManagementDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON5, &CFAManagementDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON4, &CFAManagementDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON6, &CFAManagementDlg::OnBnClickedButton6)
END_MESSAGE_MAP()


// CFAManagementDlg 메시지 처리기

BOOL CFAManagementDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_Tray.m_bHide=TRUE;
	m_Tray.AddTrayIcon(GetSafeHwnd());
	m_Loginbit=0;
	SetWindowText(_T("FA Management"));


	//rtDialog.right 338;
	//rtDialog.bottom 257;

	HBITMAP hbit = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP1));  
	m_LOGIN.SetBitmap(hbit);  
	CRect rt;  
	GetClientRect(&rt);  
	m_LOGIN.SetWindowPos(NULL,0,0,rt.Width(),rt.Height(),SWP_SHOWWINDOW);



	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CFAManagementDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CFAManagementDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CFAManagementDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




LRESULT CFAManagementDlg::OnTrayIcon(WPARAM wParam , LPARAM lParam)
{
	m_Tray.ProcTrayMsg(GetSafeHwnd(),wParam,lParam);
	return 0; //리턴값이 없으면 에러
}

// 트레이 종료
void CFAManagementDlg::OnAppExit()
{
	OnDestroy();
}

// 다이얼로그 감추기, 보이기
void CFAManagementDlg::OnDialogShow()
{
	if(m_Tray.m_bHide)
	{
		ShowWindow(false);
		m_Tray.m_bHide=false;
	}
	else
	{
		ShowWindow(true);
		m_Tray.m_bHide=true;
	}
}

// 다이얼로그 종료시 최종호출 함수
void CFAManagementDlg::OnDestroy(void)
{
	m_Tray.DelTrayIcon(GetSafeHwnd());
	OnLOGOUT();
	exit(1);
}

void CFAManagementDlg::OnCreatorInfo(void)
{
	OnSysCommand(IDM_ABOUTBOX, NULL);		
}

void CFAManagementDlg::OnCreatorNotice(void)
{
	UserNoticeDlg dlg;
	
	dlg.DoModal();
}

void CFAManagementDlg::OnNONE(void)
{
}

void CFAManagementDlg::OnLOGOUT(void)
{
	m_Loginbit=FALSE;
	PostMessage (WM_SHOWWINDOW, TRUE, SW_OTHERUNZOOM); 

	m_PF.StopThread();
	m_FSW.StopBit=0;
	m_Un.StopBit=0;
}

void CFAManagementDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnOK();
	CString Pw;
	GetDlgItemText(IDC_EDIT1, uid);
	GetDlgItemText(IDC_EDIT2, Pw);
	//if(m_Db.login(uid, Pw)) {
	if(1){
		//AfxMessageBox(_T("로그인 되었습니다."));
		m_Loginbit=TRUE;
		PostMessage (WM_SHOWWINDOW, FALSE, SW_OTHERUNZOOM); 

		//드라이버 로드
		//m_PF.StartThread();

		//파일 생성 로그
		m_FSW.StartThread((char*)CT2CA(uid));

		//공지 사항
		m_Un.SetInit(GetSafeHwnd(), uid);
		m_Un.StartThread();

		SetDlgItemText(IDC_EDIT1, _T(""));
		SetDlgItemText(IDC_EDIT2, _T(""));


	}else {		
		AfxMessageBox(_T("아이디 또는 비밀번호가 틀렸습니다.\n확인후 다시 로그인해 주세요."));
	}
}

void CFAManagementDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//system("start /b FA.exe");	
	m_PF.StartThread();

}

void CFAManagementDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_PF.StopThread();
}


void CFAManagementDlg::OnBnClickedButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//hThread = (HANDLE)_beginthreadex(NULL, 0, Start, (LPVOID)(new FileSystemParam(uid, (CString)"", (CString)"")), 0, NULL);
	m_FSW.StartThread((char*)CT2CA(uid));
}

void CFAManagementDlg::OnBnClickedButton4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//TerminateThread(hThread, 0);
	//CloseHandle(hThread);
	m_FSW.StopBit=0;
}

void CFAManagementDlg::OnBnClickedButton5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	//m_Tray.TrayIconTooltip(GetSafeHwnd());
	m_Un.SetInit(GetSafeHwnd(), uid);
	m_Un.StartThread();	
}

void CFAManagementDlg::OnBnClickedButton6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Un.StopBit=0;
}
