#pragma once
#include <string>

class CFileSystemEvent
{
public:
	enum ChangeType 
	{
		CREATED,
		CHANGED,
		DELETED,
		RENAMED,
		MAX,
	};

private:
	ChangeType  m_ChangeType;
	std::string m_Directory;
	std::string m_FileName;
	std::string m_NewFileName;
	std::string m_EventUid;

public:
	CFileSystemEvent(ChangeType type, const std::string& directory, const std::string& fileName, const std::string& Uid);
	CFileSystemEvent(const std::string& directory, const std::string& oldFileName, const std::string& newFileName, const std::string& Uid);
	~CFileSystemEvent();

public:
	std::string GetUid() const;
	std::string GetFullPath() const;
	std::string GetOldFullPath() const;
	std::string GetNewFullPath() const;

public:
	ChangeType GetChangeType() const { return m_ChangeType; }
	const std::string& GetDirectory() const { return m_Directory; }
	const std::string& GetFileName() const { return m_FileName; }
	const std::string& GetOldFileName() const { return m_FileName; }
	const std::string& GetNewFileName() const { return m_NewFileName; }
};