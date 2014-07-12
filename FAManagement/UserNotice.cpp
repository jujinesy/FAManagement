#include "stdafx.h"
#include "UserNotice.h"


CUserNotice::CUserNotice()
{
}


CUserNotice::~CUserNotice(void)
{
}

void CUserNotice::SetInit(HWND hWnd, CString UID)
{
	m_TrayhWnd = hWnd;
	m_UID=UID;
	StopBit=1;
}

void CUserNotice::StartThread()  
{     
    m_ThreadHandle = (HANDLE)_beginthreadex(   
        0, 
		0, 
		(unsigned int (__stdcall *)(void *))Update, 
		(LPVOID)this,  
        0, 
		//(unsigned int *)m_ThreadID );    
		&m_ThreadID ); 
}  

UINT WINAPI CALLBACK CUserNotice::Update(LPVOID parameter)  
{  
    CUserNotice* pThread = ( CUserNotice* )parameter;  
    pThread->m_cLife=LIFE_LIVE;  
    pThread->Run();    
    return 0;  
} 

void CUserNotice::Run() 
{
	while(true)
	{		
		if(!StopBit)
			break;
		Sleep(100);
		if(m_Db.GetNotice(0, m_UID, FALSE))
		{
			m_Tray.TrayIconTooltip(m_TrayhWnd);
			Sleep(10000);
		}
	}
}