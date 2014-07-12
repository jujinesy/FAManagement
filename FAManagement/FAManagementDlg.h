
// FAManagementDlg.h : ��� ����
//

#pragma once
#include "TaryIconConMgr.h" // Ʈ���̾������� ����ϴ� ����� ���� ���
#include "DBconnector.h" // Ʈ���̾������� ����ϴ� ����� ���� ���
#include "FileSystemWatcher.h"
#include "PacketFilter.h"
#include "UserNotice.h"
#include <process.h>
#include "afxwin.h"

struct FileSystemParam
{
	FileSystemParam(LPCTSTR uid, LPCTSTR one, LPCTSTR two)
	{
		userid = uid;
		etc1 = one;
		etc2 = two;
	};
	CString userid;
	CString etc1;
	CString etc2;
};

// CFAManagementDlg ��ȭ ����
class CFAManagementDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CFAManagementDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_FAMANAGEMENT_DIALOG };



	bool m_Loginbit;
	CString uid;
	// Ʈ���̾����� ����
	LRESULT OnTrayIcon(WPARAM wParam , LPARAM lParam);
	// Ʈ���� ����
	void OnAppExit();
	// ���̾�α� ���̱�, ���߱�
	void OnDialogShow();
	// Ʈ���� ������ �Ŵ���	
	CTaryIconConMgr m_Tray;
	CDBConnector m_Db;
	PacketFilter m_PF;
	CUserNotice m_Un;
	CFileSystemWatcher m_FSW;

	// ���̾�α� ����� ����ȣ�� �Լ�
	afx_msg void OnDestroy();
	void OnCreatorInfo();
	void OnCreatorNotice();
	void OnNONE();
	void OnLOGOUT();
	HANDLE hThread;





	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton6();
	CStatic m_LOGIN;
};
UINT WINAPI Start(LPVOID lpParameter);
void OnFileCreate(const CFileSystemEvent& evt);
void OnFileChange(const CFileSystemEvent& evt);
void OnFileDelete(const CFileSystemEvent& evt);
void OnFileRename(const CFileSystemEvent& evt);
struct AttributesParam* ShowAttributes(DWORD attrib);