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
	int       nQuery_stat = 0;


	mysql_init(&mySqlConn);

	pMySqlConnection = mysql_real_connect(&mySqlConn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);
	if (pMySqlConnection == NULL)
	{
		fprintf(stderr, "Mysql connection error : %s", mysql_error(&mySqlConn));
		return 1;
	}

	nQuery_stat = mysql_query(pMySqlConnection, "select * from user");
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

	mysql_free_result(pMySqlResult);

	char strQuery[255];
	int nId = 111;
	char nName[12] = "sss";
	char nPwd[12] = "qwert";
	sprintf_s(strQuery, 255, "insert into user values " "('%d', '%s', '%s')", nId, nName, nPwd);

	nQuery_stat = mysql_query(pMySqlConnection, strQuery);
	if (nQuery_stat != 0)
	{
		fprintf(stderr, "Mysql query error : %s", mysql_error(&mySqlConn));
		return 1;
	}

	mysql_close(pMySqlConnection);

    return 0;
}

