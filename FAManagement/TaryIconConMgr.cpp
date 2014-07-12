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

// �˾��޴�����

void CTaryIconConMgr::MakePopupMenu(HWND hWnd, int x, int y)
{
	//�˾� �޴��� �����ϰ� �޴� ����
	HMENU hMenu = CreatePopupMenu();
	CFAManagementDlg* CWnd;
	(CFAManagementDlg*)CWnd = (CFAManagementDlg*)CWnd::FromHandle(hWnd); 
	//(FAManagementDlg*)pcWnd = CWnd::FromHandle(hWnd);  
	if(CWnd->m_Loginbit)
	{
		AppendMenu(hMenu, MF_STRING, WM_NOTICE, _T("��������"));
		if(m_Db.GetFlag(CWnd->uid))		
		{
			AppendMenu(hMenu, MF_STRING, WM_NONE, _T("�����̿� ���� : �Ұ���"));
		} else 
		{
			AppendMenu(hMenu, MF_STRING, WM_NONE, _T("�����̿� ���� : ����"));
		}
	}
	if(!CWnd->m_Loginbit)
		if(m_bHide)		//���̾�αװ� ������ ���¶�� 
			AppendMenu(hMenu, MF_STRING, WM_DIALOG_SHOW, _T("�α��� â ���߱�"));
		else			//���̾�αװ� ������ ���¶��
			AppendMenu(hMenu, MF_STRING, WM_DIALOG_SHOW, _T("�α��� â ���̱�"));
	AppendMenu(hMenu, MF_STRING, WM_ABOUTBOX, _T("������"));
	if(CWnd->m_Loginbit)
		AppendMenu(hMenu, MF_STRING, WM_LOGOUT, _T("�α׾ƿ�"));
	AppendMenu(hMenu, MF_STRING, WM_APP_EXIT, _T("����"));

	SetForegroundWindow(hWnd);//������ �˾��޴� ���� Ŭ���� �� �˾� �ݱ�
	//�˾� �޴� ����
	TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, 0, hWnd, NULL);
}

// �˾��޴��� �̺�Ʈ �߻��� ó�� ��
void CTaryIconConMgr::ProcTrayMsg(HWND hWnd , WPARAM wParam , LPARAM lParam)
{
	HMENU hMenu = NULL;
	POINT pos;

	if(lParam == WM_RBUTTONDOWN)
	{
		GetCursorPos(&pos);
		MakePopupMenu(hWnd, pos.x, pos.y); //�˾� �޴� ���� �� ���
	}
}

// Ʈ���̾����� �߰�
bool CTaryIconConMgr::AddTrayIcon(HWND hWnd)
{
	if(m_bExist)		//�̹� Ʈ���� �������� �ִٸ� ����
		return FALSE;

	NOTIFYICONDATA nid;	//�������� �����Ͽ� ����
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
	if(Shell_NotifyIcon(NIM_ADD, &nid)==0) //Ʈ���� ������ ǥ��
		return FALSE;
	m_bExist = true;	
	return TRUE;
}

// Ʈ���̾����� ����
bool CTaryIconConMgr::TrayIconTooltip(HWND hWnd)
{
	NOTIFYICONDATA nid;	//�������� �����Ͽ� ����
	
	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	
	nid.uFlags = NIF_INFO;
	lstrcpy(nid.szInfo, _T("���ο� ���������� �����߽��ϴ�.\n���������� Ȯ���� �ּ���."));
	lstrcpy(nid.szInfoTitle, _T("�˸�"));
	nid.dwInfoFlags = NIIF_INFO;
	nid.uTimeout=10000;

	Shell_NotifyIcon(NIM_MODIFY, &nid);
	


	return TRUE;
}

// Ʈ���̾����� ����
bool CTaryIconConMgr::DelTrayIcon(HWND hWnd)
{
	NOTIFYICONDATA nid;

	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uFlags = NULL;
	if(Shell_NotifyIcon(NIM_DELETE, &nid)==0) //Ʈ���� ������ ����
		return FALSE;
	return TRUE;
}