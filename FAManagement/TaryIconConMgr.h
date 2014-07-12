#pragma once



#include "resource.h"
#include "DBConnector.h"


class CTaryIconConMgr : public CObject
{
public:
	CTaryIconConMgr(void);
	virtual ~CTaryIconConMgr(void);
	// �˾��޴�����
	void MakePopupMenu(HWND hWnd, int x, int y);
	// Ʈ���̾����� ���� ����
	bool m_bExist;
	// Ʈ���̾����� ���迩��
	bool m_bHide;
	// �˾��޴��� �̺�Ʈ �߻��� ó�� ��
	void ProcTrayMsg(HWND hWnd , WPARAM wParam , LPARAM lParam);
	// Ʈ���̾����� �߰�
	bool AddTrayIcon(HWND hWnd);
	// Ʈ���̾����� ����
	bool TrayIconTooltip(HWND hWnd);
	// Ʈ���̾����� ����
	bool DelTrayIcon(HWND hWnd);
	CDBConnector m_Db;
};

