
// FAManagementDlg.cpp : ���� ����
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

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

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

// CFAManagementDlg ��ȭ ����

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


// CFAManagementDlg �޽��� ó����

BOOL CFAManagementDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
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



	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CFAManagementDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CFAManagementDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




LRESULT CFAManagementDlg::OnTrayIcon(WPARAM wParam , LPARAM lParam)
{
	m_Tray.ProcTrayMsg(GetSafeHwnd(),wParam,lParam);
	return 0; //���ϰ��� ������ ����
}

// Ʈ���� ����
void CFAManagementDlg::OnAppExit()
{
	OnDestroy();
}

// ���̾�α� ���߱�, ���̱�
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

// ���̾�α� ����� ����ȣ�� �Լ�
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//CDialogEx::OnOK();
	CString Pw;
	GetDlgItemText(IDC_EDIT1, uid);
	GetDlgItemText(IDC_EDIT2, Pw);
	//if(m_Db.login(uid, Pw)) {
	if(1){
		//AfxMessageBox(_T("�α��� �Ǿ����ϴ�."));
		m_Loginbit=TRUE;
		PostMessage (WM_SHOWWINDOW, FALSE, SW_OTHERUNZOOM); 

		//����̹� �ε�
		//m_PF.StartThread();

		//���� ���� �α�
		m_FSW.StartThread((char*)CT2CA(uid));

		//���� ����
		m_Un.SetInit(GetSafeHwnd(), uid);
		m_Un.StartThread();

		SetDlgItemText(IDC_EDIT1, _T(""));
		SetDlgItemText(IDC_EDIT2, _T(""));


	}else {		
		AfxMessageBox(_T("���̵� �Ǵ� ��й�ȣ�� Ʋ�Ƚ��ϴ�.\nȮ���� �ٽ� �α����� �ּ���."));
	}
}

void CFAManagementDlg::OnBnClickedButton1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//system("start /b FA.exe");	
	m_PF.StartThread();

}

void CFAManagementDlg::OnBnClickedButton2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_PF.StopThread();
}


void CFAManagementDlg::OnBnClickedButton3()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//hThread = (HANDLE)_beginthreadex(NULL, 0, Start, (LPVOID)(new FileSystemParam(uid, (CString)"", (CString)"")), 0, NULL);
	m_FSW.StartThread((char*)CT2CA(uid));
}

void CFAManagementDlg::OnBnClickedButton4()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//TerminateThread(hThread, 0);
	//CloseHandle(hThread);
	m_FSW.StopBit=0;
}

void CFAManagementDlg::OnBnClickedButton5()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	
	//m_Tray.TrayIconTooltip(GetSafeHwnd());
	m_Un.SetInit(GetSafeHwnd(), uid);
	m_Un.StartThread();	
}

void CFAManagementDlg::OnBnClickedButton6()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_Un.StopBit=0;
}
