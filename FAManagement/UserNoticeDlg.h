#pragma once
#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "DBConnector.h"
#include "FAManagementDlg.h"

// UserNoticeDlg 대화 상자입니다.

class UserNoticeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(UserNoticeDlg)

public:
	UserNoticeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~UserNoticeDlg();
	CDBConnector m_Db;
	CFAManagementDlg *pWnd;
	CString uid;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_Notice };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CListCtrl m_radio1;
	CListCtrl m_NoticeList;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheck1();
	CButton m_Allbutton;
	afx_msg void OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult);
	CStatic m_NOTICE;
};
