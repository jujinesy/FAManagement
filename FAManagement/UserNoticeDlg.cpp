// UserNoticeDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "FAManagement.h"
#include "UserNoticeDlg.h"
#include "afxdialogex.h"


// UserNoticeDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(UserNoticeDlg, CDialogEx)

	UserNoticeDlg::UserNoticeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(UserNoticeDlg::IDD, pParent)
{

}

UserNoticeDlg::~UserNoticeDlg()
{
}

void UserNoticeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	CFont fnt;
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(lf));
	lf.lfHeight = 48;
	lf.lfWeight = FW_BOLD;
	::lstrcpy(lf.lfFaceName, (LPCWSTR)"�ü�");
	fnt.CreateFontIndirect(&lf);
	GetDlgItem(IDC_STATIC)->SetFont(&fnt);
	fnt.Detach();
	DDX_Control(pDX, IDC_LIST1, m_NoticeList);
	DDX_Control(pDX, IDC_CHECK1, m_Allbutton);
	DDX_Control(pDX, IDC_NOTICE, m_NOTICE);
}


BEGIN_MESSAGE_MAP(UserNoticeDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &UserNoticeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &UserNoticeDlg::OnBnClickedCancel)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &UserNoticeDlg::OnNMClickList1)
	ON_BN_CLICKED(IDC_CHECK1, &UserNoticeDlg::OnBnClickedCheck1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &UserNoticeDlg::OnNMCustomdrawList1)
END_MESSAGE_MAP()


// UserNoticeDlg �޽��� ó�����Դϴ�.


void UserNoticeDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	ShowWindow(false);
	//visi
	CDialogEx::OnOK();
}


void UserNoticeDlg::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	ShowWindow(false);
	CDialogEx::OnCancel();
}


BOOL UserNoticeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	pWnd = (CFAManagementDlg*)GetParent();
	uid=pWnd->uid;
	RECT rtDialog;
	RECT DesktopRect;

	// Get the desktop window
	HWND hDesktop = ::GetDesktopWindow();

	// Get the Desktop rect
	::GetWindowRect(hDesktop, &DesktopRect);
	::GetClientRect(GetSafeHwnd(),&rtDialog);


	// Move the window to center
	// ::MoveWindow(GetSafeHwnd(), (DesktopRect.right)-rtDialog.right ,(DesktopRect.bottom)-rtDialog.bottom, rtDialog.right, rtDialog.bottom, TRUE);
	::MoveWindow(GetSafeHwnd(), (DesktopRect.right)-rtDialog.right ,0, rtDialog.right, rtDialog.bottom, TRUE);

	m_NoticeList.InsertColumn(0, _T("����"), LVCFMT_LEFT, 45);
	m_NoticeList.InsertColumn(1, _T("����"), LVCFMT_LEFT, 0);
	m_NoticeList.InsertColumn(2, _T("UID"), LVCFMT_LEFT, 0);
	m_NoticeList.InsertColumn(3, _T("����"), LVCFMT_LEFT, 226);
	m_NoticeList.InsertColumn(4, _T("����"), LVCFMT_LEFT, 0);
	m_NoticeList.InsertColumn(5, _T("���� ����"), LVCFMT_LEFT, 130);
	m_NoticeList.InsertColumn(6, _T("Ȯ�� ����"), LVCFMT_LEFT, 0);
	m_NoticeList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);



	m_NoticeList.DeleteAllItems();
	m_Db.GetNotice(&m_NoticeList, uid, FALSE);

	//rtDialog.right 426
	//rtDialog.bottom 517

	HBITMAP hbit = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP2));  
	m_NOTICE.SetBitmap(hbit);  
	CRect rt;  
	GetClientRect(&rt);  
	m_NOTICE.SetWindowPos(NULL,0,0,rt.Width(),rt.Height(),SWP_SHOWWINDOW);
		
	return TRUE;
}


void UserNoticeDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	int Mark=m_NoticeList.GetSelectionMark();
	SetDlgItemText(IDC_EDIT1, m_NoticeList.GetItemText(m_NoticeList.GetSelectionMark(), 3));
	SetDlgItemText(IDC_EDIT2, m_NoticeList.GetItemText(m_NoticeList.GetSelectionMark(), 4));
	//m_NoticeList.SetItemText(m_NoticeList.GetSelectionMark(), 3, _T("����"));
	m_Db.ReadNotice(m_NoticeList.GetItemText(m_NoticeList.GetSelectionMark(), 0), uid);
	OnBnClickedCheck1();
}


void UserNoticeDlg::OnBnClickedCheck1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_NoticeList.DeleteAllItems();
	if(!m_Allbutton.GetCheck())
	{
		m_Db.GetNotice(&m_NoticeList, uid, FALSE);
	} else 
	{
		m_Db.GetNotice(&m_NoticeList, uid, TRUE);
	}

}


void UserNoticeDlg::OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	//NMLVCUSTOMDRAW* pLVCD = reinterpret_cast(pNMHDR);  
	NMLVCUSTOMDRAW* pLVCD = (NMLVCUSTOMDRAW*)pNMHDR;



	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	BOOL bDoColoring = FALSE;  
  
    // List Control�� �ش� ������ ���ڿ��� �޾ƿɴϴ�.  
    // �ش� ���� 1(2��°)���� ��µ� ���ڿ��� ���� ���� ������ �ٲ̴ܰϴ�.  
	CString strWhatToDo = m_NoticeList.GetItemText(pLVCD->nmcd.dwItemSpec, 6);
      
    // ���� �˾Ƽ� ������ �־�߰���.  
	if(!strcmp(CW2A(strWhatToDo), ""))
    {  
        bDoColoring = TRUE;  
    }     

	*pResult = 0;



	if( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )   
    {  
        *pResult = CDRF_NOTIFYITEMDRAW;  
  
    }   
    else if( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )   
    {  
  
        if( bDoColoring == TRUE )   
        {  
            pLVCD->clrText = RGB(0, 0, 0);           // ���ڻ� ����  
            pLVCD->clrTextBk = RGB(139, 216, 252);   // ���� ����  
        }   
  
        *pResult = CDRF_DODEFAULT;  
    }  

}
