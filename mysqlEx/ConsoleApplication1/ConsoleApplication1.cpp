// ConsoleApplication1.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#define DB_HOST "127.0.0.1"
#define DB_USER "testuser"
#define DB_PASS "wjd0268!@#$"
#define DB_NAME "test_seo"

int main()
{
	MYSQL*		pMySqlConnection = nullptr;
	MYSQL		mySqlConn = {};
	MYSQL_RES*	pMySqlResult = {};
	MYSQL_ROW	mySqlRow = {};
	int			nQuery_stat = 0;


	mysql_init(&mySqlConn);

	pMySqlConnection = mysql_real_connect(&mySqlConn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);
	if (pMySqlConnection == NULL)
	{
		fprintf(stderr, "Mysql connection error : %s", mysql_error(&mySqlConn));
		return 1;
	}

	mysql_query(&mySqlConn, "set names euckr");


	/*nQuery_stat = mysql_query(pMySqlConnection, "select * from user");
	if (nQuery_stat != 0)
	{
		fprintf(stderr, "Mysql query error : %s", mysql_error(&mySqlConn));
		return 1;
	}

	pMySqlResult = mysql_store_result(pMySqlConnection);

	while ((mySqlRow = mysql_fetch_row(pMySqlResult)) != NULL)
	{
		printf("%s, %s, %s \n", mySqlRow[0], mySqlRow[1], mySqlRow[2]);
	}

	mysql_free_result(pMySqlResult);*/

	WCHAR wstrQuery[255] = {};
	int nId = 222;
	/*char strName[12] = "ssss";
	char strPw[12] = "qwert";*/
	
	std::wstring wstrName = L"정크ㅋ";
	std::wstring wstrPw = L"sssssss";

	swprintf_s(wstrQuery, 255, L"insert into user values " L"('%d', '%ws', '%ws')", nId, wstrName.c_str(), wstrPw.c_str());

	char strQuery[255] = {};
	int len = WideCharToMultiByte(CP_ACP, 0, wstrQuery, -1, NULL, 0, NULL, NULL);	
	WideCharToMultiByte(CP_ACP, 0, wstrQuery, -1, strQuery, len, NULL, NULL);

	nQuery_stat = mysql_query(pMySqlConnection, strQuery);
	if (nQuery_stat != 0)
	{
		fprintf(stderr, "Mysql query error : %s", mysql_error(&mySqlConn));
		return 1;
	}

	mysql_close(pMySqlConnection);

    return 0;
}

