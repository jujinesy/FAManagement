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
    HANDLE m_ThreadHandle;          //������ �ڵ�  
    //DWORD m_ThreadID;               //������ ���̵� 
	unsigned int m_ThreadID;               //������ ���̵� 
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
    char m_cLife;                   //�������� ����  
};

