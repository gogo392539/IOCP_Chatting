// windowForm.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

	C_MYWIN::createWin();
	C_MYWIN::getWin()->init(hInstance);
	C_MYWIN::getWin()->updateMsg();
	C_MYWIN::getWin()->release();
	C_MYWIN::releaseWin();


    

	return (int)0;
}
