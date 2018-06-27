#include "stdafx.h"

C_JOININFO::C_JOININFO() :
	m_hBtnCancel(NULL),
	m_hBtnIdOverlap(NULL),
	m_hBtnNickOverlap(NULL),
	m_hBtnOk(NULL),
	m_hEditId(NULL),
	m_hEditNick(NULL),
	m_hEditPw1(NULL),
	m_hEditPw2(NULL),
	m_bIdCheck(),
	m_bNickCheck(),
	m_strId{},
	m_strNick{},
	m_strPw1{},
	m_strPw2{},
	m_nIdLen(0),
	m_nNickLen(0),
	m_nPwLen(0)
{
}

void C_JOININFO::initJoinControl(HWND hWndMain, HINSTANCE hInstance)
{
	CreateWindow(TEXT("static"), L"ID", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 40, 100, 25, hWndMain,
		(HMENU)IDC_STATIC, hInstance, NULL);
	CreateWindow(TEXT("static"), L"NICKNAME", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 80, 100, 25, hWndMain,
		(HMENU)IDC_STATIC, hInstance, NULL);
	CreateWindow(TEXT("static"), L"PW", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 120, 100, 25, hWndMain,
		(HMENU)IDC_STATIC, hInstance, NULL);
	CreateWindow(TEXT("static"), L"PW 확인", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 160, 100, 25, hWndMain,
		(HMENU)IDC_STATIC, hInstance, NULL);

	m_hEditId = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 120, 40, 200, 25, hWndMain,
		(HMENU)IDC_JOIN_EDIT_ID, hInstance, NULL);
	m_hEditNick = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 120, 80, 200, 25, hWndMain,
		(HMENU)IDC_JOIN_EDIT_NICK, hInstance, NULL);
	m_hEditPw1 = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 120, 120, 200, 25, hWndMain,
		(HMENU)IDC_JOIN_EDIT_PW1, hInstance, NULL);
	m_hEditPw2 = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 120, 160, 200, 25, hWndMain,
		(HMENU)IDC_JOIN_EDIT_PW2, hInstance, NULL);

	m_hBtnIdOverlap = CreateWindow(TEXT("button"), L"ID 중복 확인", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 330, 40, 120, 20, hWndMain,
		(HMENU)IDC_JOIN_BTS_ID_OVERLAP, hInstance, NULL);
	m_hBtnNickOverlap = CreateWindow(TEXT("button"), L"Nick 중복 확인", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 330, 80, 120, 20, hWndMain,
		(HMENU)IDC_JOIN_BTS_NICK_OVERLAP, hInstance, NULL);

	m_hBtnOk = CreateWindow(TEXT("button"), L"회원 가입", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 360, 400, 100, 20, hWndMain,
		(HMENU)IDC_JOIN_BTS_OK, hInstance, NULL);
	m_hBtnCancel = CreateWindow(TEXT("button"), L"취소", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 230, 400, 100, 20, hWndMain,
		(HMENU)IDC_JOIN_BTS_CANCEL, hInstance, NULL);
}

void C_JOININFO::getEditText()
{
	GetWindowText(m_hEditId, m_strId, 13);
	GetWindowText(m_hEditNick, m_strNick, 13);
	GetWindowText(m_hEditPw1, m_strPw1, 13);
	GetWindowText(m_hEditPw2, m_strPw2, 13);

	m_nIdLen = lstrlenW(m_strId);
	m_nNickLen = lstrlenW(m_strNick);
	m_nPwLen = lstrlenW(m_strPw1);
}

void C_JOININFO::initEditText()
{
	SetWindowText(m_hEditId, L"");
	SetWindowText(m_hEditNick, L"");
	SetWindowText(m_hEditPw1, L"");
	SetWindowText(m_hEditPw2, L"");
}



bool C_JOININFO::idOverlapCheck()
{
	if(!m_bIdCheck)
		return false;
	return true;
}

bool C_JOININFO::NickOverlapCheck()
{
	if(!m_bNickCheck)
		return false;
	return true;
}

bool C_JOININFO::pwEqualCheck()
{
	if (lstrcmpW(m_strPw1, m_strPw2) != 0)
		return false;
	return true;
}
