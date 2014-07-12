#pragma once
#include "TaryIconConMgr.h"
#include "DBConnector.h"

class CUserNotice
{
public:
	CUserNotice();
	~CUserNotice(void);

	void SetInit(HWND hWnd, CString UID);
	void StartThread();  
	void StopThread();  
    //virtual void Run()=0;     
	virtual void Run();
    static UINT WINAPI Update(LPVOID parameter);  
  

public:  
    HANDLE m_ThreadHandle;          //스레드 핸들  
    //DWORD m_ThreadID;               //스레드 아이디 
	unsigned int m_ThreadID;               //스레드 아이디 
	HWND m_TrayhWnd;
	CString m_UID;
	CTaryIconConMgr m_Tray;
	CDBConnector m_Db;
	int StopBit;
    enum 
    {  
        LIFE_DEF,  
        LIFE_LIVE,  
        LIFE_TERMINATE,  
    };  
    char m_cLife;                   //스레드의 상태  
};

