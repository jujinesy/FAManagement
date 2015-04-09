#pragma once
#include <atlstr.h>
#include <process.h>
#include <Windows.h>
#include "FileSystemEvent.h"

struct AttributesParam
{
	AttributesParam(){};
	CString READONLY;
	CString HIDDEN;
	CString SYSTEM;
	CString DIRECTORY;
	CString ARCHIVE;
	CString DEVICE;
	CString NORMAL;
	CString TEMPORARY;
	CString SPARSE_FILE;
	CString REPARSE_POINT;
	CString COMPRESSED;
	CString OFFLINE;
	CString NOT_CONTENT_INDEXED;
	CString ENCRYPTED;
	CString INTEGRITY_STREAM;
	CString VIRTUAL;
	CString NO_SCRUB_DATA;
};

class CFileSystemWatcher
{
public:

	typedef void(*PFN_EVENT_HANDLER)(const CFileSystemEvent& args);

private:
	struct MONITOR;
	std::string       m_Path;
	HANDLE            m_hDirectory;
	MONITOR*          m_Monitor;
	PFN_EVENT_HANDLER m_EventHandler[CFileSystemEvent::MAX];


public:
	std::string       m_WatcherUid;
	CFileSystemWatcher();
	CFileSystemWatcher(const std::string& directory);
	virtual ~CFileSystemWatcher();

public:
	bool Open(const std::string& directory);
	bool SetUid(const std::string& uid);
	void Close();
	bool IsGood() const;
	void Heartbeat(DWORD period = 1);
	void SetEventHandler(
		CFileSystemEvent::ChangeType type, PFN_EVENT_HANDLER handler);
	const std::string& GetPath() const { return m_Path; }


private:
	bool PostChangeNotificationRequest(int operation);
	void OnCreate(const std::string& fileName, const std::string& Uid);
	void OnChange(const std::string& fileName, const std::string& Uid);
	void OnDelete(const std::string& fileName, const std::string& Uid);
	void OnRename(const std::string& oldFileName, const std::string& newFileName, const std::string& Uid);
	static void CALLBACK FileSystemWatcherCallback(DWORD, DWORD, LPOVERLAPPED);

public:
	void StartThread(std::string Uid);
	void StopThread();
	//virtual void Run()=0;     
	virtual void Run();
	//자식 클래스들이 자신들의 용도에 맞게 구현하도록 한다  

	static UINT WINAPI Update(LPVOID parameter);


public:
	HANDLE m_ThreadHandle;          //스레드 핸들  
	//DWORD m_ThreadID;               //스레드 아이디 
	unsigned int m_ThreadID;               //스레드 아이디 
	int StopBit;
	enum
	{
		LIFE_DEF,
		LIFE_LIVE,
		LIFE_TERMINATE,
	};
	char m_cLife;                   //스레드의 상태  
};

void OnFileCreate(const CFileSystemEvent& evt);
void OnFileChange(const CFileSystemEvent& evt);
void OnFileDelete(const CFileSystemEvent& evt);
void OnFileRename(const CFileSystemEvent& evt);
struct AttributesParam* ShowAttributes(DWORD attrib);