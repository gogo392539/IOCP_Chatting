#include "stdafx.h"

C_MYWIN* C_MYWIN::m_pMyWin = nullptr;

C_MYWIN::C_MYWIN() :
	m_hInstance(NULL),
	m_hWnd(NULL),
	m_hWnd2(NULL),
	m_hBtn(NULL)
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
		CreateWindow(TEXT("button"), L"WIN2 HIDE", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 100, 100, 20, hWnd, (HMENU)IDC_BUTTON1, m_hInstance, NULL);
		CreateWindow(TEXT("button"), L"ALL SHOW", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 400, 100, 20, hWnd, (HMENU)IDC_BUTTON3, m_hInstance, NULL);
		
		break;
	case WM_COMMAND:
		switch (GET_X_LPARAM(wParam))
		{
		case IDC_BUTTON1:
			ShowWindow(m_hWnd2, SW_HIDE);
			
			break;
		case IDC_BUTTON3:
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			ShowWindow(m_hWnd2, SW_SHOWDEFAULT);
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

LRESULT C_MYWIN::wndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return m_pMyWin->myProc2(hWnd, message, wParam, lParam);
}

LRESULT C_MYWIN::myProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		CreateWindow(TEXT("button"), L"WIN1 HIDE", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 100, 100, 20, hWnd, (HMENU)IDC_BUTTON2, m_hInstance, NULL);
		CreateWindow(TEXT("button"), L"ALL SHOW", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 400, 100, 20, hWnd, (HMENU)IDC_BUTTON3, m_hInstance, NULL);

		break;
	case WM_COMMAND:
		switch (GET_X_LPARAM(wParam))
		{
		case IDC_BUTTON2:
			ShowWindow(m_hWnd, SW_HIDE);

			break;
		case IDC_BUTTON3:
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			ShowWindow(m_hWnd, SW_SHOWDEFAULT);
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
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWFORM));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = L"winForm";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassExW(&wcex);

	WNDCLASSEXW wcex2;
	wcex2 = wcex;
	wcex2.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex2.lpszClassName = L"winForm2";
	wcex2.lpfnWndProc = wndProc2;
	RegisterClassExW(&wcex2);

	m_hWnd = CreateWindowW(L"winForm", nullptr, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	if (!m_hWnd)
		return false;

	m_hWnd2 = CreateWindowW(L"winForm2", nullptr, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	if (!m_hWnd2)
		return false;

	m_hInstance = hInstance;

	ShowWindow(m_hWnd, SW_SHOWDEFAULT);
	UpdateWindow(m_hWnd);

	ShowWindow(m_hWnd2, SW_SHOWDEFAULT);
	UpdateWindow(m_hWnd2);

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
