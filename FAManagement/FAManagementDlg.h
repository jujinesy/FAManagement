
// FAManagementDlg.h : 헤더 파일
//

#pragma once
#include "TaryIconConMgr.h" // 트레이아이콘을 담당하는 사용자 정의 헤더
#include "DBconnector.h" // 트레이아이콘을 담당하는 사용자 정의 헤더
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

// CFAManagementDlg 대화 상자
class CFAManagementDlg : public CDialogEx
{
// 생성입니다.
public:
	CFAManagementDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FAMANAGEMENT_DIALOG };



	bool m_Loginbit;
	CString uid;
	// 트래이아이콘 생성
	LRESULT OnTrayIcon(WPARAM wParam , LPARAM lParam);
	// 트레이 종료
	void OnAppExit();
	// 다이얼로그 보이기, 감추기
	void OnDialogShow();
	// 트래이 아이콘 매니저	
	CTaryIconConMgr m_Tray;
	CDBConnector m_Db;
	PacketFilter m_PF;
	CUserNotice m_Un;
	CFileSystemWatcher m_FSW;

	// 다이얼로그 종료시 최종호출 함수
	afx_msg void OnDestroy();
	void OnCreatorInfo();
	void OnCreatorNotice();
	void OnNONE();
	void OnLOGOUT();
	HANDLE hThread;





	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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