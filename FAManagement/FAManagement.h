
// FAManagement.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CFAManagementApp:
// �� Ŭ������ ������ ���ؼ��� FAManagement.cpp�� �����Ͻʽÿ�.
//

class CFAManagementApp : public CWinApp
{
public:
	CFAManagementApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CFAManagementApp theApp;