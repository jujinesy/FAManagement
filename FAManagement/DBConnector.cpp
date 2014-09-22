//#pragma comment( lib, "../../FAManagement/lib/mysqlcppconn.lib")
#include "stdafx.h"
#include "DBConnector.h"


using namespace std;

CDBConnector::CDBConnector(void)
{
	driver = NULL;
	con = NULL;
	stmt = NULL;
	res = NULL;
	pstmt = NULL;
	errNo = 0;
}


CDBConnector::~CDBConnector(void)
{
}

BOOL CDBConnector::dbConnector()
{
	try
	{
		//driver = sql::mysql::MySQL_Driver::Instance();
		driver = get_driver_instance();

		if(driver)
		{
			con = driver->connect("tcp://172.16.100.61", "root", "aaaaaa");
			con->setSchema("FAdb");

			if(con)
			{

				stmt = con->createStatement();


				//  stmt = con->createStatement();
				//  stmt->execute("DROP TABLE IF EXISTS test");
				//  stmt->execute("CREATE TABLE test(id INT)");
				//  delete stmt;
				//
				//  /* '?' is the supported placeholder syntax */
				//pstmt = con->prepareStatement("INSERT INTO test(id) VALUES (?)");
				char str[100] ="";
				char str2[100]="";
				char str3[100]="";
				pstmt = con->prepareStatement("INSERT INTO LOGFILE (FILE_NAME, EVENT_TYPE, EVENT_TIME, MD5, USER_NAME, USER_IP) VALUES ((?), (?), (?), \"MD501\", (?), (?))");
				for (int i = 1; i <= 9999; i++) {
					strcpy_s(str, "FILE");
					_itoa_s(rand()%100+1,str2,10);
					strcat_s(str, str2);
					pstmt->setString(1, str);


					pstmt->setString(2, rand()%2?"C":rand()%2?"D":"U");

					strcpy_s(str, "201310");
					_itoa_s(rand()%19+10,str2,10);
					strcat_s(str, str2);

					_itoa_s(rand()%14+10,str2,10);
					strcat_s(str, str2);

					_itoa_s(rand()%50+10,str2,10);
					strcat_s(str, str2);

					_itoa_s(rand()%60+10,str2,10);
					strcat_s(str, str2);
					pstmt->setString(3, str);

					strcpy_s(str, "a");
					_itoa_s(rand()%200+1,str2,10);
					strcat_s(str, str2);
					pstmt->setString(4, str);

					pstmt->setInt(5, rand());

					pstmt->executeUpdate();
				}


				//  delete pstmt;
				//stmt->execute("INSERT INTO LOGFILE (FILE_NAME, EVENT_TYPE, EVENT_TIME, MD5, USER_NAME, USER_IP) VALUES ("FILE01", "A", default, "MD501", "a001", 98988)");
				//INSERT INTO LOGFILE (FILE_NAME, EVENT_TYPE, EVENT_TIME, MD5, USER_NAME, USER_IP) VALUES ("FILE01", "A", "2013-10-19 16:00:00", "MD501", "a001", 98988)


				res = stmt->executeQuery("SELECT 'Hello World!' AS _message");



				while (res->next()) {
					MultiByteToWideChar(0, 0, res->getString("_message").c_str(), 'ab', Wbuf, 'ab');
					OutputDebugStringW(Wbuf);
					OutputDebugStringW(_T("\n"));

					MultiByteToWideChar(0, 0, res->getString(1).c_str(), 'ab', Wbuf, 'ab');
					OutputDebugStringW(Wbuf);
					OutputDebugStringW(_T("\n"));
				}
				delete res;

				delete pstmt;
				delete stmt;
			}
		}

		delete con;
	}
	catch(sql::SQLException &e)
	{
		errNo = e.getErrorCode();
		AfxMessageBox(_T("Test Error! Code: %d"), errNo);
	}
	return true;
}





BOOL CDBConnector::login(CString Id, CString Pw)
{
	try
	{
		driver = get_driver_instance();
		if(driver)
		{
			con = driver->connect("192.168.0.254", "root", "aaaaaa");
			con->setSchema("SGdb");
			if(con)
			{
				stmt = con->createStatement();
				pstmt = con->prepareStatement("SELECT 'true' as sd FROM USER where USER_ID=(?) and USER_PASSWORD=(?)");

				//strcpy(str1, Id.GetString);
				//1) ch = (LPSTR)(LPCSTR)str; 
				//str1 = (LPSTR)(LPCSTR)Pwd; 
				//2) ch = str.GetBuffer(str.GetLength());
				//str1=Pwd.GetBuffer(Pwd.GetLength());
				//3) wsprintf( ch, "%s", str);
				//wsprintf((LPWSTR)str1, "%s", Pwd);
				//Pwd.GetBuffer(Pwd.GetLength()
				//memcpy(str1, (char*)(LPCTSTR)Pwd, Pwd.GetLength());
				//memcpy(str1, (unsigned char*)(LPCTSTR)Pwd, 2);
				pstmt->setString(1, (char*)CT2A(Id));
				pstmt->setString(2, (char*)CT2A(Pw));
				res = pstmt->executeQuery();
				while (res->next()) {
					return true;
				}
				return false;
				delete res;
				delete pstmt;
				delete stmt;
			}
		}
		delete con;
	}
	catch(sql::SQLException &e)
	{
		errNo = e.getErrorCode();
		AfxMessageBox(_T("login Error! Code: %d"), errNo);
		return false;
	}	
	return false;
}

BOOL CDBConnector::FileLog(
	CString Uid, 
	CString Event_type, 
	CString Old_file_path, 
	CString New_file_path, 
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
	CString MD5_hash)
{
	CString Old_file_name;
	CString New_file_name;
	int column=1;
	try
	{
		driver = get_driver_instance();
		if(driver)
		{
			
			con = driver->connect("192.168.0.254", "root", "aaaaaa");
			con->setSchema("FAdb");
			if(con)
			{
				char str1[1000]="";
				stmt = con->createStatement();
				//stmt->execute("set names utf8");
				//stmt->execute("set names utf8");
				//stmt->execute("SET CHARACTER SET utf8");
				//stmt->execute("set session character_set_connection=utf8;");
				//stmt->execute("set session character_set_results=euckr;");
				//stmt->execute("set session character_set_client=euckr;");



				pstmt = con->prepareStatement("INSERT INTO FILE_LOG (USER_ID, EVENT_TYPE, OLD_FILE_PATH, OLD_FILE_NAME, NEW_FILE_PATH, NEW_FILE_NAME, READONLY, HIDDEN, SYSTEM, DIRECTORY, ARCHIVE, DEVICE, NORMAL, TEMPORARY, SPARSE_FILE, REPARSE_POINT, COMPRESSED, OFFLINE, NOT_CONTENT_INDEXED, ENCRYPTED, INTEGRITY_STREAM, VIRTUAL, NO_SCRUB_DATA, MD5_HASH) \
											  VALUES ((?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?))");

				//pstmt = con->prepareStatement("INSERT INTO FILE_LOG (USER_ID, EVENT_TYPE, OLD_FILE_NAME, NEW_FILE_NAME, READONLY, HIDDEN, SYSTEM, DIRECTORY, ARCHIVE, DEVICE, NORMAL, TEMPORARY, SPARSE_FILE, REPARSE_POINT, COMPRESSED, OFFLINE, NOT_CONTENT_INDEXED, ENCRYPTED, INTEGRITY_STREAM, VIRTUAL, NO_SCRUB_DATA, MD5_HASH) \
				//							  VALUES ((?), (?), (?), \"한글\", (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?), (?))");
				
				//CREATE TABLE `FAdb`.`FILE_LOG` (
				//`USER_ID` VARCHAR(50) NULL,
				//`EVENT_TYPE` VARCHAR(7) NULL,
				//`EVENT_TIME` TIMESTAMP NULL DEFAULT CURRENT_TIMESTAMP,
				//`OLD_FILE_PATH` VARCHAR(500) NULL,
				//`OLD_FILE_NAME` VARCHAR(250) NULL,
				//`NEW_FILE_PATH` VARCHAR(500) NULL,
				//`NEW_FILE_NAME` VARCHAR(250) NULL,
				//`READONLY` VARCHAR(3) NULL,
				//`HIDDEN` VARCHAR(3) NULL,
				//`SYSTEM` VARCHAR(3) NULL,
				//`DIRECTORY` VARCHAR(3) NULL,
				//`ARCHIVE` VARCHAR(3) NULL,
				//`DEVICE` VARCHAR(3) NULL,
				//`NORMAL` VARCHAR(3) NULL,
				//`TEMPORARY` VARCHAR(3) NULL,
				//`SPARSE_FILE` VARCHAR(3) NULL,
				//`REPARSE_POINT` VARCHAR(3) NULL,
				//`COMPRESSED` VARCHAR(3) NULL,
				//`OFFLINE` VARCHAR(3) NULL,
				//`NOT_CONTENT_INDEXED` VARCHAR(3) NULL,
				//`ENCRYPTED` VARCHAR(3) NULL,
				//`INTEGRITY_STREAM` VARCHAR(3) NULL,
				//`VIRTUAL` VARCHAR(3) NULL,
				//`NO_SCRUB_DATA` VARCHAR(3) NULL,
				//`MD5_HASH` VARCHAR(200) NULL) ENGINE=MyISAM DEFAULT CHARSET=utf8;
				
				//주석 추가
				New_file_name = Old_file_path.Mid(Old_file_path.ReverseFind('\\')+1, Old_file_path.GetLength());
				New_file_name = New_file_path.Mid(New_file_path.ReverseFind('\\')+1, New_file_path.GetLength());

				pstmt->setString(column++, (char*)CT2CA(Uid));
				pstmt->setString(column++, (char*)CW2A(Event_type));
				pstmt->setString(column++, (char*)CW2A(Old_file_path, CP_UTF8));
				pstmt->setString(column++, (char*)CW2A(Old_file_name, CP_UTF8));
				pstmt->setString(column++, (char*)CW2A(New_file_path, CP_UTF8));
				pstmt->setString(column++, (char*)CW2A(New_file_name, CP_UTF8));
				//pstmt->setString(column++, "한글");
				pstmt->setString(column++, (char*)CT2A(READONLY));
				pstmt->setString(column++, (char*)CT2A(HIDDEN));
				pstmt->setString(column++, (char*)CT2A(SYSTEM));
				pstmt->setString(column++, (char*)CT2A(DIRECTORY));
				pstmt->setString(column++, (char*)CT2A(ARCHIVE));
				pstmt->setString(column++, (char*)CT2A(DEVICE));
				pstmt->setString(column++, (char*)CT2A(NORMAL));
				pstmt->setString(column++, (char*)CT2A(TEMPORARY));
				pstmt->setString(column++, (char*)CT2A(SPARSE_FILE));
				pstmt->setString(column++, (char*)CT2A(REPARSE_POINT));
				pstmt->setString(column++, (char*)CT2A(COMPRESSED));
				pstmt->setString(column++, (char*)CT2A(OFFLINE));
				pstmt->setString(column++, (char*)CT2A(NOT_CONTENT_INDEXED));
				pstmt->setString(column++, (char*)CT2A(ENCRYPTED));
				pstmt->setString(column++, (char*)CT2A(INTEGRITY_STREAM));
				pstmt->setString(column++, (char*)CT2A(VIRTUAL));
				pstmt->setString(column++, (char*)CT2A(NO_SCRUB_DATA));
				pstmt->setString(column++, (char*)CT2A(MD5_hash));
				
				pstmt->executeUpdate();
				
				delete pstmt;
				delete stmt;
			}
		}
		delete con;
	}
	catch(sql::SQLException &e)
	{
		errNo = e.getErrorCode();
		AfxMessageBox(_T("FileLog Error! Code: %d"), errNo);
		return false;
	}	
	return false;
}

BOOL CDBConnector::GetNotice(CListCtrl* pList, CString Uid, BOOL ALL)
{
	try
	{
		driver = get_driver_instance();
		if(driver)
		{
			con = driver->connect("192.168.0.254", "root", "aaaaaa");
			con->setSchema("FAdb");
			if(con)
			{
				int nIndex;
				LVITEM lvItem;
				char buf[1000];

				stmt = con->createStatement();
				stmt->execute("set names euckr");
				if(ALL)
				{
					pstmt = con->prepareStatement("SELECT distinct a.IDX, a.NOTICE, a.USER_ID, a.TITLE, a.CONTENT, a.SEND_TIME, b.CHECK_TIME from (select distinct IDX, NOTICE, (CASE WHEN NOTICE = 'YES' THEN (?) ELSE USER_ID END) AS 'USER_ID', TITLE, CONTENT, SEND_TIME from FAdb.NOTICE) as a left join (select distinct idx, USER_ID, MAX(CHECK_TIME) as CHECK_TIME from FAdb.NOTICE_CHECK where USER_ID =(?) GROUP BY idx) as b ON a.IDX = b.IDX where a.USER_ID=(?) ORDER BY a.IDX DESC");
				} else 
				{
					pstmt = con->prepareStatement("SELECT distinct a.IDX, a.NOTICE, a.USER_ID, a.TITLE, a.CONTENT, a.SEND_TIME, b.CHECK_TIME from (select distinct IDX, NOTICE, (CASE WHEN NOTICE = 'YES' THEN (?) ELSE USER_ID END) AS 'USER_ID', TITLE, CONTENT, SEND_TIME from FAdb.NOTICE) as a left join (select distinct idx, USER_ID, MAX(CHECK_TIME) as CHECK_TIME from FAdb.NOTICE_CHECK where USER_ID =(?) GROUP BY idx) as b ON a.IDX = b.IDX where a.USER_ID=(?) and b.CHECK_TIME IS NULL ORDER BY a.IDX DESC");
				}
				//SELECT distinct a.IDX, a.NOTICE, a.USER_ID, a.TITLE, a.CONTENT, a.SEND_TIME, b.CHECK_TIME
				//from (select distinct IDX, NOTICE, (CASE WHEN NOTICE = 'YES' THEN 'sss' ELSE USER_ID END) AS 'USER_ID', TITLE, CONTENT, SEND_TIME 
				//from FAdb.NOTICE) as a 
				//left join 
				//(select distinct idx, USER_ID, MAX(CHECK_TIME) as CHECK_TIME from FAdb.NOTICE_CHECK where USER_ID ='sss' GROUP BY idx) as b 
				//ON a.IDX = b.IDX
				//where a.USER_ID='sss'
				//ORDER BY a.IDX DESC;


				//strcpy(str1, Id.GetString);
				//1) ch = (LPSTR)(LPCSTR)str; 
				//str1 = (LPSTR)(LPCSTR)Pwd; 
				//2) ch = str.GetBuffer(str.GetLength());
				//str1=Pwd.GetBuffer(Pwd.GetLength());
				//3) wsprintf( ch, "%s", str);
				//wsprintf((LPWSTR)str1, "%s", Pwd);
				//Pwd.GetBuffer(Pwd.GetLength()
				//memcpy(str1, (char*)(LPCTSTR)Pwd, Pwd.GetLength());
				//memcpy(str1, (unsigned char*)(LPCTSTR)Pwd, 2);
				//pstmt->setString(1, (char*)CT2A(Uid));
				//pstmt->setString(2, (char*)CT2A(Uid));
				pstmt->setString(1, (char*)CT2A(Uid));
				pstmt->setString(2, (char*)CT2A(Uid));
				pstmt->setString(3, (char*)CT2A(Uid));
				res = pstmt->executeQuery();
				while (res->next()) {
					if(pList==0)
						return true;
					

					nIndex=pList->GetItemCount();
					ZeroMemory(&lvItem,sizeof(lvItem));
					lvItem.mask = LVIF_TEXT|LVIF_PARAM;
					lvItem.iItem=nIndex;
					lvItem.iSubItem=0;
					sprintf_s(buf,"%s", res->getString("IDX").c_str());
					MultiByteToWideChar(0, 0, buf, sizeof(buf), Wbuf, 'ab');
					lvItem.pszText=Wbuf;
					pList->InsertItem(&lvItem);
					pList->SetItemText(nIndex, 1, CA2T((res->getString("NOTICE").asStdString()).c_str()));
					pList->SetItemText(nIndex, 2, CA2T((res->getString("USER_ID").asStdString()).c_str()));
					pList->SetItemText(nIndex, 3, CA2T((res->getString("TITLE").asStdString()).c_str()));
					pList->SetItemText(nIndex, 4, CA2T((res->getString("CONTENT").asStdString()).c_str()));


					sprintf_s(buf,"%s", res->getString("SEND_TIME").c_str());
					MultiByteToWideChar(0, 0, buf, sizeof(buf), Wbuf, 'ab');
					pList->SetItemText(nIndex, 5, Wbuf);
					sprintf_s(buf,"%s", res->getString("CHECK_TIME").c_str());
					MultiByteToWideChar(0, 0, buf, sizeof(buf), Wbuf, 'ab');
					pList->SetItemText(nIndex, 6, Wbuf);
				}						
				delete res;
				delete pstmt;
				delete stmt;
			}
		}
		delete con;
	}
	catch(sql::SQLException &e)
	{
		errNo = e.getErrorCode();
		AfxMessageBox(_T("GetNotice Error! Code: %d"), errNo);
		return false;
	}	
	if(pList==0)
		return false;
	return true;
}

BOOL CDBConnector::ReadNotice(CString IDX, CString UID)
{
	try
	{
		driver = get_driver_instance();
		if(driver)
		{
			con = driver->connect("192.168.0.254", "root", "aaaaaa");
			con->setSchema("FAdb");
			if(con)
			{
				stmt = con->createStatement();
				stmt->execute("set names utf8");
				pstmt = con->prepareStatement("INSERT INTO FAdb.NOTICE_CHECK(IDX, USER_ID) VALUES ((?), (?))");
				//pstmt->setInt(1, atoi((char*)IDX.GetBuffer(IDX.GetLength())));
				pstmt->setString(1, (char*)CT2A(IDX));
				pstmt->setString(2, (char*)CT2A(UID));
				pstmt->executeUpdate();
				delete pstmt;
				delete stmt;
			}
		}
		delete con;
	}
	catch(sql::SQLException &e)
	{
		errNo = e.getErrorCode();
		AfxMessageBox(_T("ReadNotice Error! Code: %d"), errNo);
		return false;
	}	
	return true;
}

BOOL CDBConnector::GetFlag(CString Id)
{
	try
	{
		driver = get_driver_instance();
		if(driver)
		{
			con = driver->connect("192.168.0.254", "root", "aaaaaa");
			con->setSchema("SGdb");
			if(con)
			{
				stmt = con->createStatement();
				pstmt = con->prepareStatement("SELECT * FROM SGdb.USER where USER_ID=(?)");
				pstmt->setString(1, (char*)CT2A(Id));
				res = pstmt->executeQuery();
				while (res->next()) {
					if(!strcmp(res->getString("BLACK_FLAG").c_str(), "Y"))
					{
						return TRUE;
					} else 
					{
						return false;
					}
				}
				delete res;
				delete pstmt;
				delete stmt;
			}
		}
		delete con;
	}
	catch(sql::SQLException &e)
	{
		errNo = e.getErrorCode();
		AfxMessageBox(_T("Get Flag Error! Code: %d"), errNo);
		return false;
	}	
	return false;
}


//#include "mysql_connection.h"
//
//#include <cppconn/driver.h>
//#include <cppconn/exception.h>
//#include <cppconn/resultset.h>
//#include <cppconn/statement.h>
//
//using namespace std;
//
//int main(void)
//{
//cout << endl;
//cout << "Running 'SELECT 'Hello World!' »
//   AS _message'..." << endl;
//
//try {
//  sql::Driver *driver;
//  sql::Connection *con;
//  sql::Statement *stmt;
//  sql::ResultSet *res;
//
//  /* Create a connection */
//  driver = get_driver_instance();
//  con = driver->connect("tcp://127.0.0.1:3306", "root", "root");
//  /* Connect to the MySQL test database */
//  con->setSchema("test");
//
//  stmt = con->createStatement();
//  res = stmt->executeQuery("SELECT 'Hello World!' AS _message");
//  while (res->next()) {
//    cout << "\t... MySQL replies: ";
//    /* Access column data by alias or column name */
//    cout << res->getString("_message") << endl;
//    cout << "\t... MySQL says it again: ";
//    /* Access column fata by numeric offset, 1 is the first column */
//    cout << res->getString(1) << endl;
//  }
//  delete res;
//  delete stmt;
//  delete con;
//
//} catch (sql::SQLException &e) {
//  cout << "# ERR: SQLException in " << __FILE__;
//  cout << "(" << __FUNCTION__ << ") on line " »
//     << __LINE__ << endl;
//  cout << "# ERR: " << e.what();
//  cout << " (MySQL error code: " << e.getErrorCode();
//  cout << ", SQLState: " << e.getSQLState() << " )" << endl;
//}
//
//cout << endl;
//
//return EXIT_SUCCESS;
//}







//
//
///* Copyright 2008, 2010, Oracle and/or its affiliates. All rights reserved.
//
//This program is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; version 2 of the License.
//
//There are special exceptions to the terms and conditions of the GPL
//as it is applied to this software. View the full text of the
//exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this
//software distribution.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
//*/
//
///* Standard C++ includes */
//#include <stdlib.h>
//#include <iostream>
//
///*
//  Include directly the different
//  headers from cppconn/ and mysql_driver.h + mysql_util.h
//  (and mysql_connection.h). This will reduce your build time!
//*/
//#include "mysql_connection.h"
//
//#include <cppconn/driver.h>
//#include <cppconn/exception.h>
//#include <cppconn/resultset.h>
//#include <cppconn/statement.h>
//#include <cppconn/prepared_statement.h>
//
//using namespace std;
//
//int main(void)
//{
//cout << endl;
//cout << "Let's have MySQL count from 10 to 1..." << endl;
//
//try {
//  sql::Driver *driver;
//  sql::Connection *con;
//  sql::Statement *stmt;
//  sql::ResultSet *res;
//  sql::PreparedStatement *pstmt;
//
//  /* Create a connection */
//  driver = get_driver_instance();
//  con = driver->connect("tcp://127.0.0.1:3306", "root", "root");
//  /* Connect to the MySQL test database */
//  con->setSchema("test");
//
//  stmt = con->createStatement();
//  stmt->execute("DROP TABLE IF EXISTS test");
//  stmt->execute("CREATE TABLE test(id INT)");
//  delete stmt;
//
//  /* '?' is the supported placeholder syntax */
//  pstmt = con->prepareStatement("INSERT INTO test(id) VALUES (?)");
//  for (int i = 1; i <= 10; i++) {
//    pstmt->setInt(1, i);
//    pstmt->executeUpdate();
//  }
//  delete pstmt;
//
//  /* Select in ascending order */
//  pstmt = con->prepareStatement("SELECT id FROM test ORDER BY id ASC");
//  res = pstmt->executeQuery();
//
//  /* Fetch in reverse = descending order! */
//  res->afterLast();
//  while (res->previous())
//    cout << "\t... MySQL counts: " << res->getInt("id") << endl;
//  delete res;
//
//  delete pstmt;
//  delete con;
//
//} catch (sql::SQLException &e) {
//  cout << "# ERR: SQLException in " << __FILE__;
//  cout << "(" << __FUNCTION__ << ") on line " »
//     << __LINE__ << endl;
//  cout << "# ERR: " << e.what();
//  cout << " (MySQL error code: " << e.getErrorCode();
//  cout << ", SQLState: " << e.getSQLState() << »
//     " )" << endl;
//}
//
//cout << endl;
//
//return EXIT_SUCCESS;
//}


//atlconv.h 

//int _tmain(int argc, _TCHAR* argv[])
//{
//char *src = "정규"; // ANSI CP949
//CA2W unicode1(src); // ANSI -> UNICODE
//
//CW2A utf8 (unicode1, CP_UTF8); // UNICODE -> UTF8
//CA2W unicode2(utf8, CP_UTF8); // UTF8 -> UNICODE
//CW2A ansi (unicode2); // UNICODE -> ANSI
//CW2A ansi2 (CA2W(utf8, CP_UTF8)); 
//
//printf("%s\n", src);
//printf("%s\n", unicode1);
//printf("%s\n", utf8);
//printf("%s\n", unicode2);
//printf("%s\n", ansi);
//printf("%s\n", ansi2);
//
//return 0;
//}