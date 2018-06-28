#include "stdafx.h"

C_MYWIN* C_MYWIN::m_pMyWin = nullptr;

C_MYWIN::C_MYWIN() :
	m_hInstance(NULL),
	m_hWndLogin(NULL),
	m_hWndJoin(NULL),
	m_hWndComm(NULL),
	m_nWinHeight(0),
	m_nWinPosX(0),
	m_nWinPosY(0),
	m_nWinWidth(0),
	m_cLoginInfo(),
	m_cJoinInfo(),
	m_cCommInfo()
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

LRESULT C_MYWIN::wndLoginProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return m_pMyWin->myLoginProc(hWnd, message, wParam, lParam);
}

LRESULT C_MYWIN::myLoginProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		m_cLoginInfo.initLoginControl(hWnd, m_hInstance);
		break;
	case WM_SIZING:
	{
		RECT * pRectSize = (RECT*)lParam;
		MoveWindow(m_hWndJoin, pRectSize->left, pRectSize->top, pRectSize->right - pRectSize->left, pRectSize->bottom - pRectSize->top, false);
		MoveWindow(m_hWndComm, pRectSize->left, pRectSize->top, pRectSize->right - pRectSize->left, pRectSize->bottom - pRectSize->top, false);
	}
		break;
	case WM_MOVING:
	{
		RECT * pRectMove = (RECT*)lParam;
		MoveWindow(m_hWndJoin, pRectMove->left, pRectMove->top, pRectMove->right - pRectMove->left, pRectMove->bottom - pRectMove->top, false);
		MoveWindow(m_hWndComm, pRectMove->left, pRectMove->top, pRectMove->right - pRectMove->left, pRectMove->bottom - pRectMove->top, false);
	}
		break;
	case WM_GETMINMAXINFO:
		//윈도우 크기 고정
		((MINMAXINFO*)lParam)->ptMaxTrackSize.x = m_nWinWidth;
		((MINMAXINFO*)lParam)->ptMaxTrackSize.y = m_nWinHeight;
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = m_nWinWidth;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = m_nWinHeight;
		break;
	case WM_COMMAND:
		switch (GET_X_LPARAM(wParam))
		{
		case IDC_LOGIN_BTN_OK:
		{
			m_cLoginInfo.getEditText();

			

			//로그인 network 추가

			m_cLoginInfo.initEditText();

			ShowWindow(m_hWndComm, SW_SHOWDEFAULT);
			ShowWindow(hWnd, SW_HIDE);
		}
			break;
		case IDC_LOGIN_BTN_CANCEL:
			DestroyWindow(hWnd);
			break;
		case IDC_LOGIN_BTN_JOIN:
			m_cLoginInfo.initEditText();

			ShowWindow(m_hWndJoin, SW_SHOWDEFAULT);
			ShowWindow(hWnd, SW_HIDE);
			break;
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

LRESULT C_MYWIN::wndJoinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return m_pMyWin->myJoinProc(hWnd, message, wParam, lParam);
}

LRESULT C_MYWIN::myJoinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		m_cJoinInfo.initJoinControl(hWnd, m_hInstance);

		break;
	case WM_SIZING:
	{
		RECT * pRectSize = (RECT*)lParam;
		MoveWindow(m_hWndLogin, pRectSize->left, pRectSize->top, pRectSize->right - pRectSize->left, pRectSize->bottom - pRectSize->top, false);
		MoveWindow(m_hWndComm, pRectSize->left, pRectSize->top, pRectSize->right - pRectSize->left, pRectSize->bottom - pRectSize->top, false);
	}
	break;
	case WM_MOVING:
	{
		RECT * pRectMove = (RECT*)lParam;
		MoveWindow(m_hWndLogin, pRectMove->left, pRectMove->top, pRectMove->right - pRectMove->left, pRectMove->bottom - pRectMove->top, false);
		MoveWindow(m_hWndComm, pRectMove->left, pRectMove->top, pRectMove->right - pRectMove->left, pRectMove->bottom - pRectMove->top, false);
	}
	break;
	case WM_GETMINMAXINFO:
		//윈도우 크기 고정
		((MINMAXINFO*)lParam)->ptMaxTrackSize.x = m_nWinWidth;
		((MINMAXINFO*)lParam)->ptMaxTrackSize.y = m_nWinHeight;
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = m_nWinWidth;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = m_nWinHeight;
		break;
	case WM_COMMAND:
		switch (GET_X_LPARAM(wParam))
		{
		case IDC_JOIN_BTN_OK:
		{
			if (!m_cJoinInfo.idOverlapCheck())
			{
				MessageBox(hWnd, L"ID 중복 확인 하세요", L"회원가입 실패", MB_OK);
				break;
			}
			else
			{
				if (!m_cJoinInfo.NickOverlapCheck())
				{
					MessageBox(hWnd, L"Nick Name 중복 확인 하세요", L"회원가입 실패", MB_OK);
					break;
				}
			}

			m_cJoinInfo.getEditText();

			if (!m_cJoinInfo.pwEqualCheck())
			{
				MessageBox(hWnd, L"Password 확인 틀림", L"회원가입 실패", MB_OK);
				break;
			}

			//회원가입 network


			m_cJoinInfo.initEditText();

			MessageBox(hWnd, L"회원가입 완료", L"회원가입", MB_OK);
			
			ShowWindow(hWnd, SW_HIDE);
			ShowWindow(m_hWndLogin, SW_SHOWDEFAULT);
		}
			break;
		case IDC_JOIN_BTN_CANCEL:
			m_cJoinInfo.initEditText();

			ShowWindow(hWnd, SW_HIDE);
			ShowWindow(m_hWndLogin, SW_SHOWDEFAULT);
			break;
		case IDC_JOIN_BTN_ID_OVERLAP:
			//ID 중복 확인 network

			break;
		case IDC_JOIN_BTN_NICK_OVERLAP:
			//NICKNAME 중복 확인 network

			break;
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

LRESULT C_MYWIN::wndCommProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return m_pMyWin->myCommProc(hWnd, message, wParam, lParam);

}

LRESULT C_MYWIN::myCommProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		m_cCommInfo.initCommControl(hWnd, m_hInstance);
		break;
	case WM_SIZING:
	{
		RECT * pRectSize = (RECT*)lParam;
		MoveWindow(m_hWndLogin, pRectSize->left, pRectSize->top, pRectSize->right - pRectSize->left, pRectSize->bottom - pRectSize->top, false);
		MoveWindow(m_hWndJoin, pRectSize->left, pRectSize->top, pRectSize->right - pRectSize->left, pRectSize->bottom - pRectSize->top, false);
	}
	break;
	case WM_MOVING:
	{
		RECT * pRectMove = (RECT*)lParam;
		MoveWindow(m_hWndLogin, pRectMove->left, pRectMove->top, pRectMove->right - pRectMove->left, pRectMove->bottom - pRectMove->top, false);
		MoveWindow(m_hWndJoin, pRectMove->left, pRectMove->top, pRectMove->right - pRectMove->left, pRectMove->bottom - pRectMove->top, false);
	}
	break;
	case WM_GETMINMAXINFO:
		//윈도우 크기 고정
		((MINMAXINFO*)lParam)->ptMaxTrackSize.x = m_nWinWidth;
		((MINMAXINFO*)lParam)->ptMaxTrackSize.y = m_nWinHeight;
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = m_nWinWidth;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = m_nWinHeight;
		break;
	case WM_COMMAND:
		switch (GET_X_LPARAM(wParam))
		{
		case IDC_COMM_BTN_OK:
			m_cCommInfo.transInputText();
			break;
		case IDC_COMM_BTN_CANCEL:
			m_cCommInfo.initEditText();
			//voice network

			ShowWindow(hWnd, SW_HIDE);
			ShowWindow(m_hWndLogin, SW_SHOWDEFAULT);
			break;
		case IDC_COMM_BTN_VOICE_OK:
			//voice network
			break;
		case IDC_COMM_BTN_VOICE_CANCEL:
			//voice network
			break;
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
	WNDCLASSEXW wcexLogin;
	wcexLogin.cbSize = sizeof(WNDCLASSEX);
	wcexLogin.style = CS_HREDRAW | CS_VREDRAW;
	wcexLogin.lpfnWndProc = wndLoginProc;
	wcexLogin.cbClsExtra = 0;
	wcexLogin.cbWndExtra = 0;
	wcexLogin.hInstance = hInstance;
	wcexLogin.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHATCLIENT));
	wcexLogin.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcexLogin.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcexLogin.lpszMenuName = 0;
	wcexLogin.lpszClassName = L"Login";
	wcexLogin.hIconSm = LoadIcon(wcexLogin.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassExW(&wcexLogin);

	WNDCLASSEXW wcexJoin;
	wcexJoin = wcexLogin;
	wcexJoin.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcexJoin.lpszClassName = L"Join";
	wcexJoin.lpfnWndProc = wndJoinProc;
	RegisterClassExW(&wcexJoin);

	WNDCLASSEXW wcexComm;
	wcexComm = wcexLogin;
	wcexComm.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcexComm.lpszClassName = L"Communication";
	wcexComm.lpfnWndProc = wndCommProc;
	RegisterClassExW(&wcexComm);


	m_nWinPosX = 200;
	m_nWinPosY = 100;
	m_nWinHeight = 500;
	m_nWinWidth = 500;

	m_hWndLogin = CreateWindowW(L"Login", L"Login", WS_OVERLAPPEDWINDOW,
		m_nWinPosX, m_nWinPosY, m_nWinWidth, m_nWinHeight, nullptr, nullptr, hInstance, nullptr);
	if (!m_hWndLogin)
		return false;

	m_hWndJoin = CreateWindowW(L"Join", L"Join", WS_OVERLAPPEDWINDOW,
		m_nWinPosX, m_nWinPosY, m_nWinWidth, m_nWinHeight, nullptr, nullptr, hInstance, nullptr);
	if (!m_hWndJoin)
		return false;

	m_hWndComm = CreateWindowW(L"Communication", L"Communication", WS_OVERLAPPEDWINDOW,
		m_nWinPosX, m_nWinPosY, m_nWinWidth, m_nWinHeight, nullptr, nullptr, hInstance, nullptr);
	if (!m_hWndComm)
		return false;

	m_hInstance = hInstance;

	ShowWindow(m_hWndLogin, SW_SHOWDEFAULT);
	UpdateWindow(m_hWndLogin);

	ShowWindow(m_hWndJoin, SW_HIDE);
	UpdateWindow(m_hWndJoin);

	ShowWindow(m_hWndComm, SW_HIDE);
	UpdateWindow(m_hWndComm);

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
}
