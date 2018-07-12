#include "stdafx.h"

const char* C_DB_SERVER::m_strDB_HOST	= "127.0.0.1";
const char* C_DB_SERVER::m_strDB_USER	= "DBserver";
const char* C_DB_SERVER::m_strDB_PASS	= "wjd0268!@#$";
const char* C_DB_SERVER::m_strDB_SCHEMA = "chat_db";

const char* C_DB_SERVER::m_strDB_TABLE	= "user_info";

void C_DB_SERVER::wcharToChar(char * strDst, LPCWSTR wstrSrc)
{
	int len = WideCharToMultiByte(CP_ACP, 0, wstrSrc, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, wstrSrc, -1, strDst, len, NULL, NULL);
}

void C_DB_SERVER::charToWchar(LPWSTR wstrDst, const char * strSrc)
{
	int nLen = MultiByteToWideChar(CP_ACP, 0, strSrc, strlen(strSrc), NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, strSrc, strlen(strSrc), wstrDst, nLen);
}

C_DB_SERVER::C_DB_SERVER() :
	m_mySqlConn{},
	m_mySqlRow{},
	m_pMySqlConn(nullptr),
	m_pMySqlResult(nullptr),
	m_wstrDB_TABLE{}
{
}

bool C_DB_SERVER::init()
{
	int	nQuery_stat = 0;
	mysql_init(&m_mySqlConn);
	
	m_pMySqlConn = mysql_real_connect(&m_mySqlConn, m_strDB_HOST, m_strDB_USER, m_strDB_PASS, m_strDB_SCHEMA, 3306, (char*)NULL, 0);
	printf("%s \n", mysql_get_server_info(m_pMySqlConn));
	if (m_pMySqlConn == NULL)
	{
		fprintf(stderr, "Mysql connection error : %s", mysql_error(&m_mySqlConn));
		return false;
	}

	nQuery_stat = mysql_query(&m_mySqlConn, "set names euckr");
	if (nQuery_stat != 0)
	{
		fprintf(stderr, "Mysql query error : %s", mysql_error(&m_mySqlConn));
		return false;
	}

	charToWchar(m_wstrDB_TABLE, m_strDB_TABLE);

	return true;
}

void C_DB_SERVER::release()
{
	mysql_close(m_pMySqlConn);
}

bool C_DB_SERVER::mysqlQuery(const char * strQuery)
{
	int	nQuery_stat = 0;
	nQuery_stat = mysql_query(m_pMySqlConn, strQuery);
	if (nQuery_stat != 0)
	{
		fprintf(stderr, "Mysql query error : %s \n", mysql_error(&m_mySqlConn));
		return false;
	}
	return true;
}

bool C_DB_SERVER::insertUserInfo(int nSerialNum, const std::wstring & wstrUserId, const std::wstring & wstrUserNickname, const std::wstring & wstrUserPw)
{
	int	nQuery_stat = 0;
	WCHAR wstrQuery[200] = {};
	char strQuery[400] = {};

	swprintf_s(wstrQuery, 200, L"insert into %ws(serial_id, user_id, user_nickname, user_pw) values ('%d', '%ws', '%ws', '%ws')", 
		m_wstrDB_TABLE, nSerialNum, wstrUserId.c_str(), wstrUserNickname.c_str(), wstrUserPw.c_str());

	wcharToChar(strQuery, wstrQuery);

	if (!mysqlQuery(strQuery))
	{
		return false;
	}

	return true;
}

bool C_DB_SERVER::selectUserId(const std::wstring & wstrUserId)
{
	int	nQuery_stat = 0;
	WCHAR wstrQuery[100] = {};
	char strQuery[200] = {};

	swprintf_s(wstrQuery, 100, L"select user_nickname from %ws where user_id = '%ws'", m_wstrDB_TABLE, wstrUserId.c_str());
	
	wcharToChar(strQuery, wstrQuery);

	if (!mysqlQuery(strQuery))
	{
		return false;
	}

	m_pMySqlResult = mysql_store_result(m_pMySqlConn);

	m_mySqlRow = mysql_fetch_row(m_pMySqlResult);
	if (m_mySqlRow == NULL)
	{
		return false;
	}

	printf("%s \n", m_mySqlRow[0]);

	mysql_free_result(m_pMySqlResult);

	return true;
}

bool C_DB_SERVER::selectUserNickName(const std::wstring & wstrUserNickname)
{

	return true;
}

bool C_DB_SERVER::selectUserAll()
{
	int	nQuery_stat = 0;
	char strQuery[100] = {};

	sprintf_s(strQuery, 100, "select * from %s", m_strDB_TABLE);
	if (!mysqlQuery(strQuery))
	{
		return false;
	}

	m_pMySqlResult = mysql_store_result(m_pMySqlConn);
	while ((m_mySqlRow = mysql_fetch_row(m_pMySqlResult)) != NULL)
	{
		printf("%s, %s, %s, %s, %s, %s \n", m_mySqlRow[0], m_mySqlRow[1], m_mySqlRow[2], m_mySqlRow[3], m_mySqlRow[4], m_mySqlRow[5]);
	}

	mysql_free_result(m_pMySqlResult);

	return true;
}

bool C_DB_SERVER::updateVoiceCheck(int nSerialNum, bool bVoiceCheck)
{
	int	nQuery_stat = 0;
	char strQuery[100] = {};

	sprintf_s(strQuery, 100, "update %s set voice_check = %d where serial_id = %d", m_strDB_TABLE, bVoiceCheck, nSerialNum);
	if (!mysqlQuery(strQuery))
	{
		return false;
	}

	return true;
}

bool C_DB_SERVER::updateConnCheck(int nSerialNum, bool bConnCheck)
{
	int	nQuery_stat = 0;
	char strQuery[100] = {};

	sprintf_s(strQuery, 100, "update %s set conn_check = %d where serial_id = %d", m_strDB_TABLE, bConnCheck, nSerialNum);
	if (!mysqlQuery(strQuery))
	{
		return false;
	}

	return true;
}

bool C_DB_SERVER::loginCheck(int* nSerialNum, char* strNick, const std::wstring & wstrUserId, const std::wstring & wstrUserPw)
{
	int	nQuery_stat = 0;
	WCHAR wstrQuery[200] = {};
	char strQuery[400] = {};

	swprintf_s(wstrQuery, 200, L"select serial_id, user_id, user_pw, user_nickname from %ws where user_id = '%ws' AND user_pw = '%ws'",
		m_wstrDB_TABLE, wstrUserId.c_str(), wstrUserPw.c_str());

	wcharToChar(strQuery, wstrQuery);

	if (!mysqlQuery(strQuery))
	{
		return false;
	}

	m_pMySqlResult = mysql_store_result(m_pMySqlConn);

	m_mySqlRow = mysql_fetch_row(m_pMySqlResult);
	if (m_mySqlRow == NULL)
	{
		*nSerialNum = -1;
		return false;
	}
	printf("%s, %s \n", m_mySqlRow[1], m_mySqlRow[2]);
	*nSerialNum = atoi(m_mySqlRow[0]);
	printf("%d \n", *nSerialNum);

	unsigned long* pColumLen = nullptr;
	pColumLen = mysql_fetch_lengths(m_pMySqlResult);
	for (int i = 0; i < (int)pColumLen[3]; i++)
	{
		strNick[i] = m_mySqlRow[3][i];
	}

	mysql_free_result(m_pMySqlResult);

	return true;
}
