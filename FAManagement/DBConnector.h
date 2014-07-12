#pragma once
#include <string>
#include "stdafx.h"
#include "mysql_driver.h"
#include "mysql_connection.h"
#include "statement.h"
#include "resultset.h"
#include <atlconv.h>
#include <stdio.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

class CDBConnector
{
public:
	CDBConnector(void);
	~CDBConnector(void);

	BOOL dbConnector();
	BOOL login(CString Id, CString Pw);
	BOOL CDBConnector::FileLog(
		CString Id, 
		CString Event_type, 
		CString Old_file_name, 
		CString New_file_name, 
		CString READONLY,
		CString HIDDEN,
		CString SYSTEM, 
		CString DIRECTORY,
		CString ARCHIVE,
		CString DEVICE,
		CString NORMAL, 
		CString TEMPORARY, 
		CString SPARSE_FILE, 
		CString REPARSE_POINT, 
		CString COMPRESSED,
		CString OFFLINE, 
		CString NOT_CONTENT_INDEXED, 
		CString ENCRYPTED, 
		CString INTEGRITY_STREAM, 
		CString VIRTUAL, 
		CString NO_SCRUB_DATA,	
		CString MD5_hash);
	BOOL CDBConnector::GetNotice(CListCtrl* pList, CString Uid, BOOL ALL);
	BOOL CDBConnector::ReadNotice(CString IDX, CString UID);
	BOOL CDBConnector::GetFlag(CString Id);

public:
	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	sql::ResultSet *res;
	sql::PreparedStatement *pstmt;
	WCHAR Wbuf['ab'];

	std::string query;
	int errNo;
};

