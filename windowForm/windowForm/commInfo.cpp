#include "stdafx.h"

C_COMMINFO::C_COMMINFO() :
	m_hBtnCancel(NULL),
	m_hBtnOk(NULL),
	m_hBtnVoiceCancel(NULL),
	m_hBtnVoiceOk(NULL),
	m_hEditChat(NULL),
	m_hEditInputText(NULL),
	m_strInputText{},
	m_nInputTextLen(0),
	m_strChatText{}
{
}

void C_COMMINFO::initCommControl(HWND hWndMain, HINSTANCE hInstance)
{
	m_hBtnOk = CreateWindow(TEXT("button"), L"확인", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 230, 400, 100, 20, hWndMain,
		(HMENU)IDC_COMM_BTS_OK, hInstance, NULL);
	m_hBtnCancel = CreateWindow(TEXT("button"), L"종료", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 360, 400, 100, 20, hWndMain,
		(HMENU)IDC_COMM_BTS_CANCEL, hInstance, NULL);
	m_hBtnVoiceOk = CreateWindow(TEXT("button"), L"음성 통신", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 360, 40, 100, 20, hWndMain,
		(HMENU)IDC_COMM_BTS_VOICE_OK, hInstance, NULL);
	m_hBtnVoiceCancel = CreateWindow(TEXT("button"), L"음성 종료", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 360, 80, 100, 20, hWndMain,
		(HMENU)IDC_COMM_BTS_VOICE_CANCEL, hInstance, NULL);

	m_hEditInputText = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 10, 400, 220, 20, hWndMain,
		(HMENU)IDC_COMM_EDIT_TEXT, hInstance, NULL);

	m_hEditChat = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN, 10, 40, 320, 360, hWndMain,
		(HMENU)IDC_COMM_EDIT_CHAT, hInstance, NULL);
}

void C_COMMINFO::transInputText()
{
	GetWindowText(m_hEditInputText, m_strInputText, 128);
	m_nInputTextLen = lstrlenW(m_strInputText);

	lstrcatW(m_strInputText, L"\r\n");
	lstrcatW(m_strChatText, m_strInputText);

	SetWindowText(m_hEditChat, m_strChatText);
	SetWindowText(m_hEditInputText, L"");
}

void C_COMMINFO::initEditText()
{
	SetWindowText(m_hEditInputText, L"");
	SetWindowText(m_hEditChat, L"");
}
