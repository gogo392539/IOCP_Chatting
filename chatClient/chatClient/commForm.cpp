#include "stdafx.h"

LRESULT C_MYWIN::wndCommProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return m_pMyWin->myCommProc(hWnd, message, wParam, lParam);

}

LRESULT C_MYWIN::myCommProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		CreateWindow(TEXT("button"), L"확인", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 230, 400, 100, 20, hWnd,
			(HMENU)IDC_COMM_BTN_OK, m_hInstance, NULL);
		CreateWindow(TEXT("button"), L"종료", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 360, 400, 100, 20, hWnd,
			(HMENU)IDC_COMM_BTN_CANCEL, m_hInstance, NULL);
		CreateWindow(TEXT("button"), L"음성 통신", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 360, 40, 100, 20, hWnd,
			(HMENU)IDC_COMM_BTN_VOICE_OK, m_hInstance, NULL);
		CreateWindow(TEXT("button"), L"음성 종료", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 360, 80, 100, 20, hWnd,
			(HMENU)IDC_COMM_BTN_VOICE_CANCEL, m_hInstance, NULL);

		m_hEditCommInputText = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 10, 400, 220, 20, hWnd,
			(HMENU)IDC_COMM_EDIT_TEXT, m_hInstance, NULL);

		m_hEditCommChat = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN, 10, 40, 320, 360, hWnd,
			(HMENU)IDC_COMM_EDIT_CHAT, m_hInstance, NULL);
	}
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
	{
		//윈도우 크기 고정
		((MINMAXINFO*)lParam)->ptMaxTrackSize.x = m_nWinWidth;
		((MINMAXINFO*)lParam)->ptMaxTrackSize.y = m_nWinHeight;
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = m_nWinWidth;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = m_nWinHeight;
	}
		break;
	case WM_COMMAND:
		switch (GET_X_LPARAM(wParam))
		{
		case IDC_COMM_BTN_OK:
		{
			GetWindowText(m_hEditCommInputText, m_strInputText, 128);
			m_nInputTextLen = lstrlenW(m_strInputText);

			lstrcatW(m_strInputText, L"\r\n");
			lstrcatW(m_strChatText, m_strInputText);

			SetWindowText(m_hEditCommChat, m_strChatText);
			SetWindowText(m_hEditCommInputText, L"");
		}
			break;
		case IDC_COMM_BTN_CANCEL:
		{
			//m_cNetChat.initLoginCheck();
			m_cNetChat.sendLogoutMessage();
			m_cNetChat.release();

			SetWindowText(m_hEditCommInputText, L"");
			SetWindowText(m_hEditCommChat, L"");
			//voice network

			ShowWindow(hWnd, SW_HIDE);
			ShowWindow(m_hWndLogin, SW_SHOWDEFAULT);
		}
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