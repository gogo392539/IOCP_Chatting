#include "stdafx.h"

LRESULT C_MYWIN::wndLoginProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return m_pMyWin->myLoginProc(hWnd, message, wParam, lParam);
}

LRESULT C_MYWIN::myLoginProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		CreateWindow(TEXT("button"), L"확인", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 360, 400, 100, 20, hWnd,
			(HMENU)IDC_LOGIN_BTN_OK, m_hInstance, NULL);
		CreateWindow(TEXT("button"), L"종료", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 230, 400, 100, 20, hWnd,
			(HMENU)IDC_LOGIN_BTN_CANCEL, m_hInstance, NULL);
		CreateWindow(TEXT("button"), L"회원가입", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 180, 100, 20, hWnd,
			(HMENU)IDC_LOGIN_BTN_JOIN, m_hInstance, NULL);
		CreateWindow(TEXT("static"), L"ID", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 100, 40, 25, hWnd,
			(HMENU)IDC_STATIC, m_hInstance, NULL);
		CreateWindow(TEXT("static"), L"PW", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 140, 40, 25, hWnd,
			(HMENU)IDC_STATIC, m_hInstance, NULL);
		m_hEditLoginId = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 50, 100, 200, 25, hWnd,
			(HMENU)IDC_LOGIN_EDIT_ID, m_hInstance, NULL);
		m_hEditLoginPw = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 50, 140, 200, 25, hWnd,
			(HMENU)IDC_LOGIN_EDIT_PW, m_hInstance, NULL);
	}
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
		case IDC_LOGIN_BTN_OK:
		{
			m_cNetChat.init(m_hEditCommChat);
			WCHAR strId[13];
			WCHAR strPw[13];
			int nIdLen;
			int nPwLen;

			GetWindowText(m_hEditLoginId, strId, 13);
			GetWindowText(m_hEditLoginPw, strPw, 13);
			nIdLen = lstrlenW(strId);
			nPwLen = lstrlenW(strPw);

			//로그인 network 
			m_cNetChat.sendLoginMessage(strId, nIdLen, strPw, nPwLen);
			while (1)
			{
				if (m_cNetChat.getLoginSuccessCheck())
				{
					ShowWindow(m_hWndComm, SW_SHOWDEFAULT);
					ShowWindow(hWnd, SW_HIDE);
					break;
				}
				else if (m_cNetChat.getLoginFailCheck())
				{
					MessageBox(hWnd, L"회원 정보 틀림", L"로그인 실패", MB_OK);
					m_cNetChat.release();
					break;
				}
			}

			

			SetWindowText(m_hEditLoginId, L"");
			SetWindowText(m_hEditLoginPw, L"");
		}
		break;
		case IDC_LOGIN_BTN_CANCEL:
			DestroyWindow(hWnd);
			break;
		case IDC_LOGIN_BTN_JOIN:
			SetWindowText(m_hEditLoginId, L"");
			SetWindowText(m_hEditLoginPw, L"");

			m_cNetJoin.init(m_hWndJoin);

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