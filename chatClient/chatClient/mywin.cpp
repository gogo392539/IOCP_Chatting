#include "stdafx.h"

C_MYWIN* C_MYWIN::m_pMyWin = nullptr;

C_MYWIN::C_MYWIN() :
	m_hInstance(NULL),
	m_hWndLogin(NULL),
	m_hWndJoin(NULL),
	m_hWndComm(NULL),
	m_hEditLoginId(NULL),
	m_hEditLoginPw(NULL),
	m_hEditJoinId(NULL),
	m_hEditJoinNick(NULL),
	m_hEditJoinPw1(NULL),
	m_hEditJoinPw2(NULL),
	m_hEditCommChat(NULL),
	m_hEditCommInputText(NULL),
	m_nWinHeight(0),
	m_nWinPosX(0),
	m_nWinPosY(0),
	m_nWinWidth(0),
	m_bIdOverlapCheck(),
	m_bNickOverlapCheck(),
	m_strInputText{},
	m_strChatText{},
	m_nInputTextLen(),
	m_bVoiceStart(),
	m_bVoiceEnd(),
	m_cNetChat()
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
