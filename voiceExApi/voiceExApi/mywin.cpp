#include "stdafx.h"

C_MYWIN* C_MYWIN::m_pMyWin = nullptr;

C_MYWIN::C_MYWIN() : 
	m_hInstance(nullptr),
	m_hWnd(nullptr),
	m_cVoice()
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
	case WM_LBUTTONDOWN:
		if (!m_cVoice.isWaveInStart())
		{
			m_cVoice.waveInOpenDevice(hWnd);
			m_cVoice.waveOutOpenDevice(hWnd);
		}
		break;
	case WM_RBUTTONDOWN:
		if(m_cVoice.isWaveOutStart())
			m_cVoice.waveInEnd();
		break;

	case MM_WIM_OPEN:
		m_cVoice.waveInVoice();
		break;
	case MM_WIM_DATA:
	{
		WAVEHDR * pWaveHdr = (WAVEHDR*)lParam;
		m_cVoice.waveOutVoice(pWaveHdr->lpData, pWaveHdr->dwBufferLength);
		m_cVoice.waveInReuseQueue(pWaveHdr);
	}
		break;
	case MM_WIM_CLOSE:
		m_cVoice.waveOutEnd();
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
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VOICEEXAPI));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = L"voice";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassExW(&wcex);


	m_hWnd = CreateWindowW(L"voice", nullptr, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	if (!m_hWnd)
		return false;

	m_hInstance = hInstance;
	m_cVoice.init();

	ShowWindow(m_hWnd, SW_SHOWDEFAULT);
	UpdateWindow(m_hWnd);

	return true;
}

void C_MYWIN::update()
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
	m_cVoice.release();
}
