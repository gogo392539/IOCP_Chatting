// dbForm.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"


int main()
{
	




	C_DB_SERVER cDbServer;
	cDbServer.init();
//	cDbServer.insertUserInfo(6, L"go3925", L"정오정크", L"qwer");
//	cDbServer.updateConnCheck(5, true);
//	int nSerialNum = -1;
//	char strNick[13] = {};
//	cDbServer.loginCheck(&nSerialNum, strNick, "gogo392539", "qwer");
//	printf("%s \n", strNick);
//	cDbServer.selectUserAll();
	//cDbServer.selectUserId("gogo392539");
	cDbServer.release();
    return 0;
}

