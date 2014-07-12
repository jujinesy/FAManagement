#include "stdafx.h"
#include "FileSystemWatcher.h"
#include "DBConnector.h"

namespace
{
	/*
	const DWORD FILTER = 
	FILE_NOTIFY_CHANGE_SECURITY |
	FILE_NOTIFY_CHANGE_CREATION |
	FILE_NOTIFY_CHANGE_LAST_ACCESS |
	FILE_NOTIFY_CHANGE_LAST_WRITE |
	FILE_NOTIFY_CHANGE_SIZE |
	FILE_NOTIFY_CHANGE_ATTRIBUTES |
	FILE_NOTIFY_CHANGE_DIR_NAME |
	FILE_NOTIFY_CHANGE_FILE_NAME;

	FILE_NOTIFY_CHANGE_CREATION | 
	FILE_NOTIFY_CHANGE_LAST_WRITE |
	FILE_NOTIFY_CHANGE_SIZE |
	FILE_NOTIFY_CHANGE_DIR_NAME |
	FILE_NOTIFY_CHANGE_FILE_NAME;
	*/

	/// 작업의 종류
	enum Operation
	{
		CONTINUE_MONITORING,  ///< 모니터링을 계속 한다.
		STOP_MONITORING,      ///< 모니터링을 중단한다.
	};

	/// 모니터링할 이벤트의 종류
	const DWORD FILTER = 
	FILE_NOTIFY_CHANGE_CREATION | 
	FILE_NOTIFY_CHANGE_LAST_WRITE |
	FILE_NOTIFY_CHANGE_SIZE |
	FILE_NOTIFY_CHANGE_DIR_NAME |
	FILE_NOTIFY_CHANGE_FILE_NAME |
	FILE_NOTIFY_CHANGE_ATTRIBUTES;

	/// MONITOR 버퍼의 크기
	const size_t BUFFER_SIZE = sizeof(FILE_NOTIFY_INFORMATION) * 1024;

	/// FILE_NOTIFY_INFORMATION 구조체 안에 와이드 문자열을 std::string 형태로 
	/// 변환해서 반환한다.
	std::string ExtractFileName(const FILE_NOTIFY_INFORMATION* info)
	{
		char fileName[1024] = {0, };
		int count = WideCharToMultiByte(
			CP_ACP, 0, info->FileName, info->FileNameLength / sizeof(WCHAR), 
			fileName, _ARRAYSIZE(fileName) - 1, NULL, NULL);
		fileName[count] = '\0';
		return std::string(fileName);
	}

	/// 경로 끝에다 백슬래쉬를 추가한 경로를 반환한다.
	std::string AddBackslash(const std::string& path)
	{
		if (path.empty()) return std::string("");
		return path[path.size()-1] != '\\' ? path + std::string("\\") : path;
	}

	/// 디렉토리 + 파일 이름을 합쳐서 전체 경로를 생성한다.
	std::string Combine(const std::string& dir, const std::string& file)
	{
		return AddBackslash(dir) + file;
	}

	/// 디렉토리가 존재하는지의 여부를 반환한다.
	bool IsDirectoryExist(const std::string& path)
	{
		DWORD attr = GetFileAttributes((CString)path.c_str());
		return (attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY);
	}
}

CFileSystemEvent::CFileSystemEvent(ChangeType type, 
								   const std::string& directory, 
								   const std::string& fileName,
								   const std::string& Uid)
								   : m_ChangeType(type), 
								   m_Directory(directory), 
								   m_FileName(fileName),
								   m_EventUid(Uid)
{
}

CFileSystemEvent::CFileSystemEvent(const std::string& directory, 
								   const std::string& oldFileName, 
								   const std::string& newFileName,
								   const std::string& Uid)
								   : m_ChangeType(RENAMED),
								   m_Directory(directory), 
								   m_FileName(oldFileName),
								   m_NewFileName(newFileName),
								   m_EventUid(Uid)
{
}

CFileSystemEvent::~CFileSystemEvent()
{
}

std::string CFileSystemEvent::GetUid() const
{
	return m_EventUid;
}

std::string CFileSystemEvent::GetFullPath() const
{
	return Combine(m_Directory, m_FileName);
}

std::string CFileSystemEvent::GetOldFullPath() const
{
	return Combine(m_Directory, m_FileName);
}

std::string CFileSystemEvent::GetNewFullPath() const
{
	return Combine(m_Directory, m_NewFileName);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

struct CFileSystemWatcher::MONITOR : public OVERLAPPED
{
	BYTE                Buffer[BUFFER_SIZE]; ///< 버퍼
	int                 Operation;           ///< 작업의 종류
	CFileSystemWatcher* Watcher;             ///< 왓처 객체
};

CFileSystemWatcher::CFileSystemWatcher()
	: m_hDirectory(INVALID_HANDLE_VALUE), m_Monitor(NULL)
{
	ZeroMemory(m_EventHandler, sizeof(PFN_EVENT_HANDLER) * CFileSystemEvent::MAX);
}

CFileSystemWatcher::CFileSystemWatcher(const std::string& directory)
	: m_hDirectory(INVALID_HANDLE_VALUE), m_Monitor(NULL)
{
	ZeroMemory(m_EventHandler, sizeof(PFN_EVENT_HANDLER) * CFileSystemEvent::MAX);

	if (!Open(directory))
		Close();
}

CFileSystemWatcher::~CFileSystemWatcher()
{
	Close();
}

bool CFileSystemWatcher::Open(const std::string& directory)
{
	//Close();

	bool success = false;

	do 
	{
		m_Path = AddBackslash(directory);
		if (!IsDirectoryExist(m_Path))
			break;

		m_hDirectory = CreateFile((CString)m_Path.c_str(),
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			NULL
			);

		if (m_hDirectory == INVALID_HANDLE_VALUE)
			break;

		m_Monitor = reinterpret_cast<MONITOR*>(HeapAlloc(GetProcessHeap(), 0, sizeof(MONITOR)));
		if (!m_Monitor)
			break;

		ZeroMemory(m_Monitor, sizeof(MONITOR));
		m_Monitor->Watcher = this;

		if (!PostChangeNotificationRequest(CONTINUE_MONITORING))
			break;

		success = true;
	} while(0);

	if (!success)
		Close();

	return success;
}

bool CFileSystemWatcher::SetUid(const std::string& uid)
{
	m_WatcherUid = uid;
	return TRUE;
}

void CFileSystemWatcher::Close()
{
	m_Path = "";

	if (m_hDirectory != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hDirectory);
		m_hDirectory = INVALID_HANDLE_VALUE;
	}

	if (m_Monitor)
	{
		HeapFree(GetProcessHeap(), 0, m_Monitor);
		m_Monitor = NULL;
	}
}

bool CFileSystemWatcher::IsGood() const
{
	return m_hDirectory != INVALID_HANDLE_VALUE && m_Monitor != NULL;
}

void CFileSystemWatcher::Heartbeat(DWORD period)
{
	if (IsGood())
	{
		SleepEx(period, TRUE); // make thread alertable
	}
}

void CFileSystemWatcher::SetEventHandler(
	CFileSystemEvent::ChangeType type, CFileSystemWatcher::PFN_EVENT_HANDLER handler)
{
	if (0 <= type && type < CFileSystemEvent::MAX)
		m_EventHandler[type] = handler;
}

bool CFileSystemWatcher::PostChangeNotificationRequest(int operation)
{
	if (!IsGood()) return false;

	m_Monitor->Operation = operation;

	DWORD bytesReturned = 0;
	DWORD bufferSize = static_cast<DWORD>(BUFFER_SIZE);

	if (ReadDirectoryChangesW(m_hDirectory, m_Monitor->Buffer, bufferSize, 
		TRUE, FILTER, &bytesReturned, m_Monitor, FileSystemWatcherCallback))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CFileSystemWatcher::OnCreate(const std::string& fileName, const std::string& Uid)
{
	if (m_EventHandler[CFileSystemEvent::CREATED])
	{
		CFileSystemEvent evt(CFileSystemEvent::CREATED, m_Path, fileName, Uid);
		m_EventHandler[CFileSystemEvent::CREATED](evt);
	}
}

void CFileSystemWatcher::OnChange(const std::string& fileName, const std::string& Uid)
{
	if (m_EventHandler[CFileSystemEvent::CHANGED])
	{
		CFileSystemEvent evt(CFileSystemEvent::CHANGED, m_Path, fileName, Uid);
		m_EventHandler[CFileSystemEvent::CHANGED](evt);
	}
}

void CFileSystemWatcher::OnDelete(const std::string& fileName, const std::string& Uid)
{
	if (m_EventHandler[CFileSystemEvent::DELETED])
	{
		CFileSystemEvent evt(CFileSystemEvent::DELETED, m_Path, fileName, Uid);
		m_EventHandler[CFileSystemEvent::DELETED](evt);
	}
}

void CFileSystemWatcher::OnRename(const std::string& oldFileName, const std::string& newFileName, const std::string& Uid)
{
	if (m_EventHandler[CFileSystemEvent::RENAMED])
	{
		CFileSystemEvent evt(m_Path, oldFileName, newFileName, Uid);
		m_EventHandler[CFileSystemEvent::RENAMED](evt);
	}
}

void CALLBACK CFileSystemWatcher::FileSystemWatcherCallback(
	DWORD errorCode, DWORD bytesTransferred, LPOVERLAPPED overlapped)
{
	CFileSystemWatcher::MONITOR* monitor = 
		reinterpret_cast<CFileSystemWatcher::MONITOR*>(overlapped);
	CFileSystemWatcher* watcher = monitor->Watcher;

	if (errorCode == ERROR_SUCCESS)
	{
		PFILE_NOTIFY_INFORMATION info = NULL;
		size_t offset = 0;
		std::string fileName, oldFileName, newFileName;

		do
		{
			info = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(&monitor->Buffer[offset]);
			offset += info->NextEntryOffset;

			if (info->Action == FILE_ACTION_ADDED)
			{
				fileName = ExtractFileName(info);
				watcher->OnCreate(fileName, watcher->m_WatcherUid);
			}
			else if (info->Action == FILE_ACTION_REMOVED)
			{
				fileName = ExtractFileName(info);
				watcher->OnDelete(fileName, watcher->m_WatcherUid);
			}
			else if (info->Action == FILE_ACTION_MODIFIED)
			{
				fileName = ExtractFileName(info);
				watcher->OnChange(fileName, watcher->m_WatcherUid);
			}
			else if (info->Action == FILE_ACTION_RENAMED_OLD_NAME)
			{
				oldFileName = ExtractFileName(info);
			}
			else if (info->Action == FILE_ACTION_RENAMED_NEW_NAME)
			{
				newFileName = ExtractFileName(info);
				watcher->OnRename(oldFileName, newFileName, watcher->m_WatcherUid);
			}
		} while (info->NextEntryOffset != 0);
	}

	if (monitor->Operation == CONTINUE_MONITORING)
		watcher->PostChangeNotificationRequest(CONTINUE_MONITORING);
}


void CFileSystemWatcher::StartThread(std::string Uid)  
{     
	m_WatcherUid=Uid;
	StopBit=1;
    m_ThreadHandle = (HANDLE)_beginthreadex(   
        0, 
		0, 
		(unsigned int (__stdcall *)(void *))Update, 
		(LPVOID)this,  
        0, 
		//(unsigned int *)m_ThreadID );    
		&m_ThreadID ); 
}  

UINT WINAPI CALLBACK CFileSystemWatcher::Update(LPVOID parameter)  
{  
    CFileSystemWatcher* pThread = ( CFileSystemWatcher* )parameter;  
    pThread->m_cLife=LIFE_LIVE;
    pThread->Run();    
    return 0;  
} 

void CFileSystemWatcher::Run() 
{
	CFileSystemWatcher FileSystem;
	CFileSystemWatcher FileSystem2;

	FileSystem.Open("D:\\");
	//FileSystem.SetUid(CT2CA(((FileSystemParam*)lpParameter)->userid).m_psz);
	FileSystem.SetUid(m_WatcherUid);
	FileSystem.SetEventHandler(CFileSystemEvent::CREATED, OnFileCreate);
	FileSystem.SetEventHandler(CFileSystemEvent::CHANGED, OnFileChange);
	FileSystem.SetEventHandler(CFileSystemEvent::DELETED, OnFileDelete);
	FileSystem.SetEventHandler(CFileSystemEvent::RENAMED, OnFileRename);

	FileSystem2.Open("C:\\");
	//FileSystem.SetUid(CT2CA(((FileSystemParam*)lpParameter)->userid).m_psz);
	FileSystem2.SetUid(m_WatcherUid);
	FileSystem2.SetEventHandler(CFileSystemEvent::CREATED, OnFileCreate);
	FileSystem2.SetEventHandler(CFileSystemEvent::CHANGED, OnFileChange);
	FileSystem2.SetEventHandler(CFileSystemEvent::DELETED, OnFileDelete);
	FileSystem2.SetEventHandler(CFileSystemEvent::RENAMED, OnFileRename);

	while (true)
	{
		if(!StopBit)
			break;
		FileSystem.Heartbeat();
		FileSystem2.Heartbeat();
	}
}

void OnFileCreate(const CFileSystemEvent& evt)
{
	CDBConnector Db;
	AttributesParam Ap;
	Ap = *(ShowAttributes(GetFileAttributes((CString)evt.GetFullPath().c_str())));

	Db.FileLog(
		(CString)(evt.GetUid().c_str())			//Uid
		, (CString)"CREATED"					//Event_type
		, (CString)""							//old_file_name
		, (CString)(evt.GetFullPath().c_str())	//new_file_name
		, Ap.READONLY
		, Ap.HIDDEN
		, Ap.SYSTEM
		, Ap.DIRECTORY
		, Ap.ARCHIVE
		, Ap.DEVICE
		, Ap.NORMAL
		, Ap.TEMPORARY
		, Ap.SPARSE_FILE
		, Ap.REPARSE_POINT
		, Ap.COMPRESSED
		, Ap.OFFLINE
		, Ap.NOT_CONTENT_INDEXED
		, Ap.ENCRYPTED
		, Ap.INTEGRITY_STREAM
		, Ap.VIRTUAL
		, Ap.NO_SCRUB_DATA
		, (CString)"md5");
}

void OnFileChange(const CFileSystemEvent& evt)
{
	CDBConnector Db;
	AttributesParam Ap;
	Ap = *(ShowAttributes(GetFileAttributes((CString)evt.GetFullPath().c_str())));

	Db.FileLog(
		(CString)(evt.GetUid().c_str())			//Uid
		, (CString)"CHANGED"					//Event_type
		, (CString)""							//old_file_name
		, (CString)(evt.GetFullPath().c_str())	//new_file_name
		, Ap.READONLY
		, Ap.HIDDEN
		, Ap.SYSTEM
		, Ap.DIRECTORY
		, Ap.ARCHIVE
		, Ap.DEVICE
		, Ap.NORMAL
		, Ap.TEMPORARY
		, Ap.SPARSE_FILE
		, Ap.REPARSE_POINT
		, Ap.COMPRESSED
		, Ap.OFFLINE
		, Ap.NOT_CONTENT_INDEXED
		, Ap.ENCRYPTED
		, Ap.INTEGRITY_STREAM
		, Ap.VIRTUAL
		, Ap.NO_SCRUB_DATA
		, (CString)"md5");
}

void OnFileDelete(const CFileSystemEvent& evt)
{
	CDBConnector Db;
	AttributesParam Ap;
	Ap = *(ShowAttributes(GetFileAttributes((CString)evt.GetFullPath().c_str())));

	Db.FileLog(
		(CString)(evt.GetUid().c_str())			//Uid
		, (CString)"DELETED"					//Event_type
		, (CString)""							//old_file_name
		, (CString)(evt.GetFullPath().c_str())	//new_file_name
		, (CString)""
		, (CString)""
		, (CString)""
		, (CString)""
		, (CString)""
		, (CString)""
		, (CString)""
		, (CString)""
		, (CString)""
		, (CString)""
		, (CString)""
		, (CString)""
		, (CString)""
		, (CString)""
		, (CString)""
		, (CString)""
		, (CString)""
		, (CString)"md5");
}

void OnFileRename(const CFileSystemEvent& evt)
{
	CDBConnector Db;
	AttributesParam Ap;
	Ap = *(ShowAttributes(GetFileAttributes((CString)evt.GetNewFullPath().c_str())));

	Db.FileLog(
		(CString)(evt.GetUid().c_str())			//Uid
		, (CString)"RENAME"					//Event_type
		, (CString)(evt.GetOldFullPath().c_str())							//old_file_name
		, (CString)(evt.GetNewFullPath().c_str())	//new_file_name
		, Ap.READONLY
		, Ap.HIDDEN
		, Ap.SYSTEM
		, Ap.DIRECTORY
		, Ap.ARCHIVE
		, Ap.DEVICE
		, Ap.NORMAL
		, Ap.TEMPORARY
		, Ap.SPARSE_FILE
		, Ap.REPARSE_POINT
		, Ap.COMPRESSED
		, Ap.OFFLINE
		, Ap.NOT_CONTENT_INDEXED
		, Ap.ENCRYPTED
		, Ap.INTEGRITY_STREAM
		, Ap.VIRTUAL
		, Ap.NO_SCRUB_DATA
		, (CString)"md5");
}

struct AttributesParam* ShowAttributes(DWORD attrib)
{
	struct AttributesParam *s = new AttributesParam();
	(attrib & FILE_ATTRIBUTE_READONLY)?s->READONLY="YES":s->READONLY="NO";
	(attrib & FILE_ATTRIBUTE_HIDDEN)?s->HIDDEN="YES":s->HIDDEN="NO";
	(attrib & FILE_ATTRIBUTE_SYSTEM)?s->SYSTEM="YES":s->SYSTEM="NO";
	(attrib & FILE_ATTRIBUTE_DIRECTORY)?s->DIRECTORY="YES":s->DIRECTORY="NO";
	(attrib & FILE_ATTRIBUTE_ARCHIVE)?s->ARCHIVE="YES":s->ARCHIVE="NO";
	(attrib & FILE_ATTRIBUTE_DEVICE)?s->DEVICE="YES":s->DEVICE="NO";
	(attrib & FILE_ATTRIBUTE_NORMAL)?s->NORMAL="YES":s->NORMAL="NO";
	(attrib & FILE_ATTRIBUTE_TEMPORARY)?s->TEMPORARY="YES":s->TEMPORARY="NO";
	(attrib & FILE_ATTRIBUTE_SPARSE_FILE)?s->SPARSE_FILE="YES":s->SPARSE_FILE="NO";
	(attrib & FILE_ATTRIBUTE_REPARSE_POINT)?s->REPARSE_POINT="YES":s->REPARSE_POINT="NO";
	(attrib & FILE_ATTRIBUTE_COMPRESSED)?s->COMPRESSED="YES":s->COMPRESSED="NO";
	(attrib & FILE_ATTRIBUTE_OFFLINE)?s->OFFLINE="YES":s->OFFLINE="NO";
	(attrib & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)?s->NOT_CONTENT_INDEXED="YES":s->NOT_CONTENT_INDEXED="NO";
	(attrib & FILE_ATTRIBUTE_ENCRYPTED)?s->ENCRYPTED="YES":s->ENCRYPTED="NO";
	(attrib & FILE_ATTRIBUTE_INTEGRITY_STREAM)?s->INTEGRITY_STREAM="YES":s->INTEGRITY_STREAM="NO";
	(attrib & FILE_ATTRIBUTE_VIRTUAL)?s->VIRTUAL="YES":s->VIRTUAL="NO";
	(attrib & FILE_ATTRIBUTE_NO_SCRUB_DATA)?s->NO_SCRUB_DATA="YES":s->NO_SCRUB_DATA="NO";

	//#define FILE_ATTRIBUTE_READONLY             0x00000001  
	//#define FILE_ATTRIBUTE_HIDDEN               0x00000002  
	//#define FILE_ATTRIBUTE_SYSTEM               0x00000004  
	//#define FILE_ATTRIBUTE_DIRECTORY            0x00000010  
	//#define FILE_ATTRIBUTE_ARCHIVE              0x00000020  
	//#define FILE_ATTRIBUTE_DEVICE               0x00000040  
	//#define FILE_ATTRIBUTE_NORMAL               0x00000080  
	//#define FILE_ATTRIBUTE_TEMPORARY            0x00000100  
	//#define FILE_ATTRIBUTE_SPARSE_FILE          0x00000200  
	//#define FILE_ATTRIBUTE_REPARSE_POINT        0x00000400  
	//#define FILE_ATTRIBUTE_COMPRESSED           0x00000800  
	//#define FILE_ATTRIBUTE_OFFLINE              0x00001000  
	//#define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  0x00002000  
	//#define FILE_ATTRIBUTE_ENCRYPTED            0x00004000  
	//#define FILE_ATTRIBUTE_INTEGRITY_STREAM     0x00008000  
	//#define FILE_ATTRIBUTE_VIRTUAL              0x00010000  
	//#define FILE_ATTRIBUTE_NO_SCRUB_DATA        0x00020000 

	/*FILE_ATTRIBUTE_ARCHIVE 32 (0x20을) 
	아카이브 파일 또는 디렉토리 파일 또는 디렉토리. 응용 프로그램은 일반적으로 백업하거나 제거 할 파일을 표시하려면이 속성을 사용합니다. 

	FILE_ATTRIBUTE_COMPRESSED 2048 (0x800과 같습니다) 
	압축 파일 또는 디렉토리. 파일의 경우 파일의 모든 데이터가 압축됩니다. 디렉토리, 압축은 새로 생성 된 파일 및 하위 디렉토리의 기본값입니다. 

	FILE_ATTRIBUTE_DEVICE 64 (0x40을) 
	이 값은 시스템 사용을 위해 예약되어 있습니다. 

	FILE_ATTRIBUTE_DIRECTORY 16 (0X10) 
	디렉토리를 식별하는 핸들입니다. 

	FILE_ATTRIBUTE_ENCRYPTED 16384 (0x4000으로) 
	암호화 된 파일 또는 디렉토리. 파일의 경우 파일의 모든 데이터 스트림이 암호화됩니다. 디렉토리의 경우, 암호화는 새로 생성 된 파일 및 하위 디렉토리의 기본값입니다. 

	FILE_ATTRIBUTE_HIDDEN 2 (0x2로) 
	파일 또는 디렉토리가 숨겨져 있습니다. 그것은 일반적인 디렉토리 목록에 포함되어 있지 않습니다. 

	FILE_ATTRIBUTE_INTEGRITY_STREAM 32768 (0x8000으로) 
	디렉토리 또는 사용자 데이터 스트림의 무결성 (만 심판 볼륨에서 지원)로 구성되어 있습니다. 그것은 일반적인 디렉토리 목록에 포함되어 있지 않습니다. 이름을 바꾼 거라면 무결성 설정 파일을 유지합니다. 파일을 복사 할 경우 원본 파일이나 대상 디렉토리 하나가 무결성이 설정되어있는 경우 대상 파일 무결성 설정을해야합니다. 
	윈도우 서버 2008 R2, 윈도우 7, 윈도우 서버 2008, 윈도우 비스타, Windows Server 2003 및 Windows XP :이 플래그는 Windows Server 2012 년까지 지원하지 않습니다. 

	FILE_ATTRIBUTE_NORMAL 128 (0x80에서) 
	다른 속성이없는 파일을 설정합니다. 이 속성은 단독으로 사용하는 경우에만 유효합니다. 

	FILE_ATTRIBUTE_NOT_CONTENT_INDEXED 8192 (0x2000과) 
	파일이나 디렉토리는 콘텐츠 인덱싱 서비스에 의해 인덱싱 할 수 없습니다. 

	FILE_ATTRIBUTE_NO_SCRUB_DATA 131072 (0x20000으로) 
	사용자 데이터 스트림은 백그라운드 데이터 무결성 스캐너 (AKA 스크러버) 읽을 수 없습니다. 디렉토리에 설정된 경우에만 상속을 제공합니다. 이 플래그는 저장 공간과 심판 볼륨에서 지원됩니다. 그것은 일반적인 디렉토리 목록에 포함되어 있지 않습니다. 
	윈도우 서버 2008 R2, 윈도우 7, 윈도우 서버 2008, 윈도우 비스타, Windows Server 2003 및 Windows XP :이 플래그는 Windows 7 및 Windows Server 2012 년까지 지원하지 않습니다. 

	FILE_ATTRIBUTE_OFFLINE 4096 (0x1000 인) 
	파일의 데이터를 즉시 사용할 수 없습니다. 이 속성은 파일 데이터가 물리적으로 오프라인 저장소로 이동을 나타냅니다. 이 속성은 계층 적 스토리지 관리 소프트웨어입니다 원격 저장소에 의해 사용됩니다. 애플리케이션이 임의의이 속성을 변경하면 안됩니다. 

	FILE_ATTRIBUTE_READONLY 1 (0x1로) 
	읽기 전용 파일입니다. 응용 프로그램이 파일을 읽을 수는 있지만 쓸하거나 삭제할 수 없습니다. 이 속성은 디렉토리에 무시됩니다. 자세한 내용은 Windows Vista에서 또는 Windows 7에서 Windows XP에서 Windows Server 2003에서 폴더의 읽기 전용 또는 시스템 특성을 보거나 변경할 수 없습니다 . 

	FILE_ATTRIBUTE_REPARSE_POINT 1024 (0x400이) 
	관련 재분석 지점 또는 심볼릭 링크 파일이 파일이나 디렉토리. 

	FILE_ATTRIBUTE_SPARSE_FILE 512 (0x200) 
	이 파일은 스파 스 파일입니다. 

	FILE_ATTRIBUTE_SYSTEM 4 (0x4로) 
	파일 또는 운영 체제의 일부를 사용하거나 단독으로 사용하는 디렉토리입니다. 

	FILE_ATTRIBUTE_TEMPORARY 256 (0X100) 
	이 파일은 임시 저장을 위해 사용되고 있습니다. 파일 시스템은 충분한 캐시 메모리를 사용할 수있는 경우 일반적으로 응용 프로그램 삭제를 처리 한 후 임시 파일이 닫혀 있기 때문에, 대용량 저장 장치에 데이터를 다시 작성하지. 이 시나리오에서는 시스템이 완전히 데이터를 쓰기 방지 할 수 있습니다. 핸들이 닫힌 후 그렇지 않으면, 데이터가 기록됩니다. 

	FILE_ATTRIBUTE_VIRTUAL 65536 (0x10000에) 
	이 값은 시스템 사용을 위해 예약되어 있습니다.*/ 
	return s;
}



//#include<stdio.h>
//#include<iostream>
//#include"FileSystemWatcher.h"
//
//
//void OnFileCreate(const cFileSystemEvent& evt)
//{
//	std::cout << evt.GetFullPath() << " created" << std::endl;
//}
//
//void OnFileChange(const cFileSystemEvent& evt)
//{
//	std::cout << evt.GetFullPath() << " changed" << std::endl;
//}
//
//void OnFileDelete(const cFileSystemEvent& evt)
//{
//	std::cout << evt.GetFullPath() << " deleted" << std::endl;
//}
//
//void OnFileRename(const cFileSystemEvent& evt)
//{
//	std::cout << evt.GetOldFullPath() << " rename to " << evt.GetNewFullPath() << std::endl;
//}
//
//void main()
//{ 
//	cFileSystemWatcher watcher("C:\\1");
//
//	watcher.SetEventHandler(cFileSystemEvent::CREATED, OnFileCreate);
//	watcher.SetEventHandler(cFileSystemEvent::CHANGED, OnFileChange);
//	watcher.SetEventHandler(cFileSystemEvent::DELETED, OnFileDelete);
//	watcher.SetEventHandler(cFileSystemEvent::RENAMED, OnFileRename);
//
//	while (true)
//	{
//		watcher.Heartbeat();
//	}
//}