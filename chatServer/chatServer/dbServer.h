#pragma once

class C_DB_SERVER
{
private:
	static const char* m_strDB_HOST;
	static const char* m_strDB_USER;
	static const char* m_strDB_PASS;
	static const char* m_strDB_SCHEMA;
	static const char* m_strDB_TABLE;

private:
	MYSQL *		m_pMySqlConn = nullptr;
	MYSQL		m_mySqlConn = {};
	MYSQL_RES*	m_pMySqlResult = {};
	MYSQL_ROW	m_mySqlRow = {};

	WCHAR		m_wstrDB_TABLE[10];

private:
	void wcharToChar(char * strDst, LPCWSTR wstrSrc);
	void charToWchar(LPWSTR wstrDst, const char * strSrc);


public:
	C_DB_SERVER();
	bool init();
	void release();
	bool mysqlQuery(const char* strQuery);
	bool insertUserInfo(int nSerialNum, const std::wstring& wstrUserId, const std::wstring& wstrUserNickname, const std::wstring& wstrUserPw);
	bool selectUserId(const std::wstring& wstrUserId);
	bool selectUserNickName(const std::wstring& wstrUserNickname);
	bool selectUserAll();
	bool updateVoiceCheck(int nSerialNum, bool bVoiceCheck);
	bool updateConnCheck(int nSerialNum, bool bConnCheck);

	bool loginCheck(int* nSerialNum, LPWSTR wstrNick, const std::wstring& wstrUserId, const std::wstring& wstrUserPw);
};