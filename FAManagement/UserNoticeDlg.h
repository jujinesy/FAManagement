#pragma once
#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "DBConnector.h"
#include "FAManagementDlg.h"

// UserNoticeDlg ��ȭ �����Դϴ�.

class UserNoticeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(UserNoticeDlg)

public:
	UserNoticeDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~UserNoticeDlg();
	CDBConnector m_Db;
	CFAManagementDlg *pWnd;
	CString uid;

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_Notice };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
