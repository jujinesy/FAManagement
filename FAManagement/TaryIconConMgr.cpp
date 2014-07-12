#include "stdafx.h"
#include "TaryIconConMgr.h"
#include "FAManagementDlg.h"


CTaryIconConMgr::CTaryIconConMgr(void)
	: m_bExist(false)
	, m_bHide(TRUE)
{
}

CTaryIconConMgr::~CTaryIconConMgr(void)
{
}

// 팝업메뉴생성

void CTaryIconConMgr::MakePopupMenu(HWND hWnd, int x, int y)
{
	//팝업 메뉴를 생성하고 메뉴 구성
	HMENU hMenu = CreatePopupMenu();
	CFAManagementDlg* CWnd;
	(CFAManagementDlg*)CWnd = (CFAManagementDlg*)CWnd::FromHandle(hWnd); 
	//(FAManagementDlg*)pcWnd = CWnd::FromHandle(hWnd);  
	if(CWnd->m_Loginbit)
	{
		AppendMenu(hMenu, MF_STRING, WM_NOTICE, _T("공지사항"));
		if(m_Db.GetFlag(CWnd->uid))		
		{
			AppendMenu(hMenu, MF_STRING, WM_NONE, _T("정상이용 여부 : 불가능"));
		} else 
		{
			AppendMenu(hMenu, MF_STRING, WM_NONE, _T("정상이용 여부 : 가능"));
		}
	}
	if(!CWnd->m_Loginbit)
		if(m_bHide)		//다이얼로그가 감춰진 상태라면 
			AppendMenu(hMenu, MF_STRING, WM_DIALOG_SHOW, _T("로그인 창 감추기"));
		else			//다이얼로그가 보여진 상태라면
			AppendMenu(hMenu, MF_STRING, WM_DIALOG_SHOW, _T("로그인 창 보이기"));
	AppendMenu(hMenu, MF_STRING, WM_ABOUTBOX, _T("만든이"));
	if(CWnd->m_Loginbit)
		AppendMenu(hMenu, MF_STRING, WM_LOGOUT, _T("로그아웃"));
	AppendMenu(hMenu, MF_STRING, WM_APP_EXIT, _T("종료"));

	SetForegroundWindow(hWnd);//생성된 팝업메뉴 밖을 클릭할 때 팝업 닫기
	//팝업 메뉴 띄우기
	TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, 0, hWnd, NULL);
}

// 팝업메뉴의 이벤트 발생시 처리 함
void CTaryIconConMgr::ProcTrayMsg(HWND hWnd , WPARAM wParam , LPARAM lParam)
{
	HMENU hMenu = NULL;
	POINT pos;

	if(lParam == WM_RBUTTONDOWN)
	{
		GetCursorPos(&pos);
		MakePopupMenu(hWnd, pos.x, pos.y); //팝업 메뉴 생성 및 출력
	}
}

// 트래이아이콘 추가
bool CTaryIconConMgr::AddTrayIcon(HWND hWnd)
{
	if(m_bExist)		//이미 트레이 아이콘이 있다면 종료
		return FALSE;

	NOTIFYICONDATA nid;	//아이콘을 생성하여 설정
	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	//nid.szInfo = (LPCTSTR)"sd";
	//nid.szInfoTitle = (LPCTSTR)"sd";
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_TRAYICON;
	wsprintf(nid.szTip, _T("FA Management"));
	
	nid.uID = 0;
	nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	if(Shell_NotifyIcon(NIM_ADD, &nid)==0) //트레이 아이콘 표시
		return FALSE;
	m_bExist = true;	
	return TRUE;
}

// 트래이아이콘 툴팁
bool CTaryIconConMgr::TrayIconTooltip(HWND hWnd)
{
	NOTIFYICONDATA nid;	//아이콘을 생성하여 설정
	
	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	
	nid.uFlags = NIF_INFO;
	lstrcpy(nid.szInfo, _T("새로운 공지사항이 도착했습니다.\n공지사항을 확인해 주세요."));
	lstrcpy(nid.szInfoTitle, _T("알림"));
	nid.dwInfoFlags = NIIF_INFO;
	nid.uTimeout=10000;

	Shell_NotifyIcon(NIM_MODIFY, &nid);
	


	return TRUE;
}

// 트레이아이콘 제거
bool CTaryIconConMgr::DelTrayIcon(HWND hWnd)
{
	NOTIFYICONDATA nid;

	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uFlags = NULL;
	if(Shell_NotifyIcon(NIM_DELETE, &nid)==0) //트레이 아이콘 삭제
		return FALSE;
	return TRUE;
}