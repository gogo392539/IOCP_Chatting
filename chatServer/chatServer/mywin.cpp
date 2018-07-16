#include "stdafx.h"

C_MYWIN* C_MYWIN::m_pMyWin = nullptr;

C_MYWIN::C_MYWIN() : 
	m_hInstance(NULL),
	m_hWnd(NULL),
	m_hBtnEraseList(NULL),
	m_hBtnServerEnd(NULL),
	m_hBtnShowList(NULL),
	m_hEditUserJoinList(NULL),
	m_hEditUserLoginList(NULL),
	m_cMainServer(),
	m_cJoinServer(),
	m_cDBServer(),
	m_cNetVoiceServer()
{
}

void C_MYWIN::createWin()
{
	if (!m_pMyWin)
		m_pMyWin = new C_MYWIN();
}

C_MYWIN * C_MYWIN::getWin()
{
	return m_pMyWin;
}

void C_MYWIN::releaseWin()
{
	if (m_pMyWin)
	{
		delete m_pMyWin;
		m_pMyWin = nullptr;
	}
}

LRESULT C_MYWIN::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return m_pMyWin->myProc(hWnd, message, wParam, lParam);
}

LRESULT C_MYWIN::myProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		CreateWindow(TEXT("static"), L"유저 접속 목록", WS_CHILD | WS_VISIBLE | SS_CENTER, 20, 5, 250, 20, hWnd,
			(HMENU)IDC_STATIC, m_hInstance, NULL);
		CreateWindow(TEXT("static"), L"유저 가입 목록", WS_CHILD | WS_VISIBLE | SS_CENTER, 300, 5, 250, 20, hWnd,
			(HMENU)IDC_STATIC, m_hInstance, NULL);
		m_hEditUserLoginList = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN,
			20, 35, 250, 275, hWnd, (HMENU)IDC_EDIT_USERJOINLIST, m_hInstance, NULL);
		m_hEditUserJoinList = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN,
			300, 35, 250, 275, hWnd, (HMENU)IDC_EDIT_USERLOGINLIST, m_hInstance, NULL);

		m_hBtnServerEnd = CreateWindow(TEXT("button"), L"Server 종료", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 30, 320, 100, 20, hWnd,
			(HMENU)IDC_BTN_SERVEREND, m_hInstance, NULL);
		m_hBtnShowList = CreateWindow(TEXT("button"), L"List 보기", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 310, 320, 100, 20, hWnd,
			(HMENU)IDC_BTN_SHOWLIST, m_hInstance, NULL);
		m_hBtnEraseList = CreateWindow(TEXT("button"), L"List 지우기", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 435, 320, 100, 20, hWnd,
			(HMENU)IDC_BTN_ERASELIST, m_hInstance, NULL);
	}
		break;
	case WM_COMMAND:
	{
		switch (GET_X_LPARAM(wParam))
		{
		case IDC_BTN_SERVEREND:
		{
			m_cMainServer.serverEnd();
			m_cMainServer.closeClient();
			m_cJoinServer.serverEnd();
			m_cJoinServer.closeClient();
			m_cNetVoiceServer.serverEnd();
			DestroyWindow(m_hWnd);
		}
			break;
		case IDC_BTN_SERVERSTART:
		{

		}
			break;
		case IDC_BTN_SHOWLIST:
		{
			SetWindowText(m_hEditUserJoinList, L"");
			SetWindowText(m_hEditUserLoginList, L"");
			m_cDBServer.selectJoinUser(m_hEditUserJoinList);
			m_cDBServer.selectLoginUser(m_hEditUserLoginList);
		}
			break;
		case IDC_BTN_ERASELIST:
		{
			SetWindowText(m_hEditUserJoinList, L"");
			SetWindowText(m_hEditUserLoginList, L"");
		}
			break;
		}
	}
		break;
	case WM_GETMINMAXINFO:
	{
		//윈도우 크기 고정
		((MINMAXINFO*)lParam)->ptMaxTrackSize.x = 600;
		((MINMAXINFO*)lParam)->ptMaxTrackSize.y = 400;
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 600;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 400;
	}
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:

		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

bool C_MYWIN::init(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHATSERVER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = L"Server";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassExW(&wcex);


	m_hWnd = CreateWindowW(L"Server", nullptr, WS_OVERLAPPEDWINDOW,
		200, 100, 600, 400, nullptr, nullptr, hInstance, nullptr);

	if (!m_hWnd)
		return FALSE;

	m_hInstance = hInstance;
	m_cDBServer.init();
	m_cMainServer.init(m_hWnd);
	m_cJoinServer.init(m_hWnd);
	m_cNetVoiceServer.init(m_hWnd);

	ShowWindow(m_hWnd, SW_SHOWDEFAULT);
	UpdateWindow(m_hWnd);

	return true;
}

void C_MYWIN::updateMsg()
{
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void C_MYWIN::release()
{
	m_cMainServer.release();
	m_cJoinServer.release();
	m_cNetVoiceServer.release();
}
