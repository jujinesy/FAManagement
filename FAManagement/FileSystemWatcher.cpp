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

	/// �۾��� ����
	enum Operation
	{
		CONTINUE_MONITORING,  ///< ����͸��� ��� �Ѵ�.
		STOP_MONITORING,      ///< ����͸��� �ߴ��Ѵ�.
	};

	/// ����͸��� �̺�Ʈ�� ����
	const DWORD FILTER = 
	FILE_NOTIFY_CHANGE_CREATION | 
	FILE_NOTIFY_CHANGE_LAST_WRITE |
	FILE_NOTIFY_CHANGE_SIZE |
	FILE_NOTIFY_CHANGE_DIR_NAME |
	FILE_NOTIFY_CHANGE_FILE_NAME |
	FILE_NOTIFY_CHANGE_ATTRIBUTES;

	/// MONITOR ������ ũ��
	const size_t BUFFER_SIZE = sizeof(FILE_NOTIFY_INFORMATION) * 1024;

	/// FILE_NOTIFY_INFORMATION ����ü �ȿ� ���̵� ���ڿ��� std::string ���·� 
	/// ��ȯ�ؼ� ��ȯ�Ѵ�.
	std::string ExtractFileName(const FILE_NOTIFY_INFORMATION* info)
	{
		char fileName[1024] = {0, };
		int count = WideCharToMultiByte(
			CP_ACP, 0, info->FileName, info->FileNameLength / sizeof(WCHAR), 
			fileName, _ARRAYSIZE(fileName) - 1, NULL, NULL);
		fileName[count] = '\0';
		return std::string(fileName);
	}

	/// ��� ������ �齽������ �߰��� ��θ� ��ȯ�Ѵ�.
	std::string AddBackslash(const std::string& path)
	{
		if (path.empty()) return std::string("");
		return path[path.size()-1] != '\\' ? path + std::string("\\") : path;
	}

	/// ���丮 + ���� �̸��� ���ļ� ��ü ��θ� �����Ѵ�.
	std::string Combine(const std::string& dir, const std::string& file)
	{
		return AddBackslash(dir) + file;
	}

	/// ���丮�� �����ϴ����� ���θ� ��ȯ�Ѵ�.
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
	BYTE                Buffer[BUFFER_SIZE]; ///< ����
	int                 Operation;           ///< �۾��� ����
	CFileSystemWatcher* Watcher;             ///< ��ó ��ü
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

	/*FILE_ATTRIBUTE_ARCHIVE 32 (0x20��) 
	��ī�̺� ���� �Ǵ� ���丮 ���� �Ǵ� ���丮. ���� ���α׷��� �Ϲ������� ����ϰų� ���� �� ������ ǥ���Ϸ����� �Ӽ��� ����մϴ�. 

	FILE_ATTRIBUTE_COMPRESSED 2048 (0x800�� �����ϴ�) 
	���� ���� �Ǵ� ���丮. ������ ��� ������ ��� �����Ͱ� ����˴ϴ�. ���丮, ������ ���� ���� �� ���� �� ���� ���丮�� �⺻���Դϴ�. 

	FILE_ATTRIBUTE_DEVICE 64 (0x40��) 
	�� ���� �ý��� ����� ���� ����Ǿ� �ֽ��ϴ�. 

	FILE_ATTRIBUTE_DIRECTORY 16 (0X10) 
	���丮�� �ĺ��ϴ� �ڵ��Դϴ�. 

	FILE_ATTRIBUTE_ENCRYPTED 16384 (0x4000����) 
	��ȣȭ �� ���� �Ǵ� ���丮. ������ ��� ������ ��� ������ ��Ʈ���� ��ȣȭ�˴ϴ�. ���丮�� ���, ��ȣȭ�� ���� ���� �� ���� �� ���� ���丮�� �⺻���Դϴ�. 

	FILE_ATTRIBUTE_HIDDEN 2 (0x2��) 
	���� �Ǵ� ���丮�� ������ �ֽ��ϴ�. �װ��� �Ϲ����� ���丮 ��Ͽ� ���ԵǾ� ���� �ʽ��ϴ�. 

	FILE_ATTRIBUTE_INTEGRITY_STREAM 32768 (0x8000����) 
	���丮 �Ǵ� ����� ������ ��Ʈ���� ���Ἲ (�� ���� �������� ����)�� �����Ǿ� �ֽ��ϴ�. �װ��� �Ϲ����� ���丮 ��Ͽ� ���ԵǾ� ���� �ʽ��ϴ�. �̸��� �ٲ� �Ŷ�� ���Ἲ ���� ������ �����մϴ�. ������ ���� �� ��� ���� �����̳� ��� ���丮 �ϳ��� ���Ἲ�� �����Ǿ��ִ� ��� ��� ���� ���Ἲ �������ؾ��մϴ�. 
	������ ���� 2008 R2, ������ 7, ������ ���� 2008, ������ ��Ÿ, Windows Server 2003 �� Windows XP :�� �÷��״� Windows Server 2012 ����� �������� �ʽ��ϴ�. 

	FILE_ATTRIBUTE_NORMAL 128 (0x80����) 
	�ٸ� �Ӽ��̾��� ������ �����մϴ�. �� �Ӽ��� �ܵ����� ����ϴ� ��쿡�� ��ȿ�մϴ�. 

	FILE_ATTRIBUTE_NOT_CONTENT_INDEXED 8192 (0x2000��) 
	�����̳� ���丮�� ������ �ε��� ���񽺿� ���� �ε��� �� �� �����ϴ�. 

	FILE_ATTRIBUTE_NO_SCRUB_DATA 131072 (0x20000����) 
	����� ������ ��Ʈ���� ��׶��� ������ ���Ἲ ��ĳ�� (AKA ��ũ����) ���� �� �����ϴ�. ���丮�� ������ ��쿡�� ����� �����մϴ�. �� �÷��״� ���� ������ ���� �������� �����˴ϴ�. �װ��� �Ϲ����� ���丮 ��Ͽ� ���ԵǾ� ���� �ʽ��ϴ�. 
	������ ���� 2008 R2, ������ 7, ������ ���� 2008, ������ ��Ÿ, Windows Server 2003 �� Windows XP :�� �÷��״� Windows 7 �� Windows Server 2012 ����� �������� �ʽ��ϴ�. 

	FILE_ATTRIBUTE_OFFLINE 4096 (0x1000 ��) 
	������ �����͸� ��� ����� �� �����ϴ�. �� �Ӽ��� ���� �����Ͱ� ���������� �������� ����ҷ� �̵��� ��Ÿ���ϴ�. �� �Ӽ��� ���� �� ���丮�� ���� ����Ʈ�����Դϴ� ���� ����ҿ� ���� ���˴ϴ�. ���ø����̼��� �������� �Ӽ��� �����ϸ� �ȵ˴ϴ�. 

	FILE_ATTRIBUTE_READONLY 1 (0x1��) 
	�б� ���� �����Դϴ�. ���� ���α׷��� ������ ���� ���� ������ ���ϰų� ������ �� �����ϴ�. �� �Ӽ��� ���丮�� ���õ˴ϴ�. �ڼ��� ������ Windows Vista���� �Ǵ� Windows 7���� Windows XP���� Windows Server 2003���� ������ �б� ���� �Ǵ� �ý��� Ư���� ���ų� ������ �� �����ϴ� . 

	FILE_ATTRIBUTE_REPARSE_POINT 1024 (0x400��) 
	���� ��м� ���� �Ǵ� �ɺ��� ��ũ ������ �����̳� ���丮. 

	FILE_ATTRIBUTE_SPARSE_FILE 512 (0x200) 
	�� ������ ���� �� �����Դϴ�. 

	FILE_ATTRIBUTE_SYSTEM 4 (0x4��) 
	���� �Ǵ� � ü���� �Ϻθ� ����ϰų� �ܵ����� ����ϴ� ���丮�Դϴ�. 

	FILE_ATTRIBUTE_TEMPORARY 256 (0X100) 
	�� ������ �ӽ� ������ ���� ���ǰ� �ֽ��ϴ�. ���� �ý����� ����� ĳ�� �޸𸮸� ����� ���ִ� ��� �Ϲ������� ���� ���α׷� ������ ó�� �� �� �ӽ� ������ ���� �ֱ� ������, ��뷮 ���� ��ġ�� �����͸� �ٽ� �ۼ�����. �� �ó����������� �ý����� ������ �����͸� ���� ���� �� �� �ֽ��ϴ�. �ڵ��� ���� �� �׷��� ������, �����Ͱ� ��ϵ˴ϴ�. 

	FILE_ATTRIBUTE_VIRTUAL 65536 (0x10000��) 
	�� ���� �ý��� ����� ���� ����Ǿ� �ֽ��ϴ�.*/ 
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