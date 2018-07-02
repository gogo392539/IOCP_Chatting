#include "stdafx.h"

LRESULT C_MYWIN::wndJoinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return m_pMyWin->myJoinProc(hWnd, message, wParam, lParam);
}

LRESULT C_MYWIN::myJoinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		CreateWindow(TEXT("static"), L"ID", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 40, 100, 25, hWnd,
			(HMENU)IDC_STATIC, m_hInstance, NULL);
		CreateWindow(TEXT("static"), L"NICKNAME", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 80, 100, 25, hWnd,
			(HMENU)IDC_STATIC, m_hInstance, NULL);
		CreateWindow(TEXT("static"), L"PW", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 120, 100, 25, hWnd,
			(HMENU)IDC_STATIC, m_hInstance, NULL);
		CreateWindow(TEXT("static"), L"PW Ȯ��", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 160, 100, 25, hWnd,
			(HMENU)IDC_STATIC, m_hInstance, NULL);

		m_hEditJoinId = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 120, 40, 200, 25, hWnd,
			(HMENU)IDC_JOIN_EDIT_ID, m_hInstance, NULL);
		m_hEditJoinNick = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 120, 80, 200, 25, hWnd,
			(HMENU)IDC_JOIN_EDIT_NICK, m_hInstance, NULL);
		m_hEditJoinPw1 = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 120, 120, 200, 25, hWnd,
			(HMENU)IDC_JOIN_EDIT_PW1, m_hInstance, NULL);
		m_hEditJoinPw2 = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 120, 160, 200, 25, hWnd,
			(HMENU)IDC_JOIN_EDIT_PW2, m_hInstance, NULL);

		CreateWindow(TEXT("button"), L"ID �ߺ� Ȯ��", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 330, 40, 120, 20, hWnd,
			(HMENU)IDC_JOIN_BTN_ID_OVERLAP, m_hInstance, NULL);
		CreateWindow(TEXT("button"), L"Nick �ߺ� Ȯ��", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 330, 80, 120, 20, hWnd,
			(HMENU)IDC_JOIN_BTN_NICK_OVERLAP, m_hInstance, NULL);

		CreateWindow(TEXT("button"), L"ȸ�� ����", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 360, 400, 100, 20, hWnd,
			(HMENU)IDC_JOIN_BTN_OK, m_hInstance, NULL);
		CreateWindow(TEXT("button"), L"���", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 230, 400, 100, 20, hWnd,
			(HMENU)IDC_JOIN_BTN_CANCEL, m_hInstance, NULL);
	}
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
	{
		//������ ũ�� ����
		((MINMAXINFO*)lParam)->ptMaxTrackSize.x = m_nWinWidth;
		((MINMAXINFO*)lParam)->ptMaxTrackSize.y = m_nWinHeight;
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = m_nWinWidth;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = m_nWinHeight;
	}
		break;
	case WM_COMMAND:
		switch (GET_X_LPARAM(wParam))
		{
		case IDC_JOIN_BTN_OK:
		{
			if (!m_bIdOverlapCheck)
			{
				MessageBox(hWnd, L"ID �ߺ� Ȯ�� �ϼ���", L"ȸ������ ����", MB_OK);
				break;
			}
			else
			{
				if (!m_bNickOverlapCheck)
				{
					MessageBox(hWnd, L"Nick Name �ߺ� Ȯ�� �ϼ���", L"ȸ������ ����", MB_OK);
					break;
				}
			}

			WCHAR strPw1[13];
			WCHAR strPw2[13];
			GetWindowText(m_hEditJoinPw1, strPw1, 13);
			GetWindowText(m_hEditJoinPw2, strPw2, 13);
			if (lstrcmpW(strPw1, strPw2) != 0)
			{
				MessageBox(hWnd, L"Password Ȯ�� Ʋ��", L"ȸ������ ����", MB_OK);
				break;
			}


			//ȸ������ network


			SetWindowText(m_hEditJoinId, L"");
			SetWindowText(m_hEditJoinNick, L"");
			SetWindowText(m_hEditJoinPw1, L"");
			SetWindowText(m_hEditJoinPw2, L"");
			m_bIdOverlapCheck = false;
			m_bNickOverlapCheck = false;

			MessageBox(hWnd, L"ȸ������ �Ϸ�", L"ȸ������", MB_OK);

			ShowWindow(hWnd, SW_HIDE);
			ShowWindow(m_hWndLogin, SW_SHOWDEFAULT);
		}
		break;
		case IDC_JOIN_BTN_CANCEL:
			SetWindowText(m_hEditJoinId, L"");
			SetWindowText(m_hEditJoinNick, L"");
			SetWindowText(m_hEditJoinPw1, L"");
			SetWindowText(m_hEditJoinPw2, L"");
			m_bIdOverlapCheck = false;
			m_bNickOverlapCheck = false;

			ShowWindow(hWnd, SW_HIDE);
			ShowWindow(m_hWndLogin, SW_SHOWDEFAULT);
			break;
		case IDC_JOIN_BTN_ID_OVERLAP:
			//ID �ߺ� Ȯ�� network
			m_bIdOverlapCheck = true;

			break;
		case IDC_JOIN_BTN_NICK_OVERLAP:
			//NICKNAME �ߺ� Ȯ�� network
			m_bNickOverlapCheck = true;

			break;
		}
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
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