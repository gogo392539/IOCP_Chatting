#include "stdafx.h"

C_LOGININFO::C_LOGININFO() : 
	m_hBtnCancel(NULL),
	m_hBtnJoin(NULL),
	m_hBtnOk(NULL),
	m_hEditId(NULL),
	m_hEditPw(NULL),
	m_strId{},
	m_strPw{},
	m_nIdLen(0),
	m_nPwLen(0)
{
}

void C_LOGININFO::initLoginControl(HWND hWndMain, HINSTANCE hInstance)
{
	m_hBtnOk = CreateWindow(TEXT("button"), L"확인", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 360, 400, 100, 20, hWndMain,
		(HMENU)IDC_LOGIN_BTS_OK, hInstance, NULL);
	m_hBtnCancel = CreateWindow(TEXT("button"), L"종료", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 230, 400, 100, 20, hWndMain,
		(HMENU)IDC_LOGIN_BTS_CANCEL, hInstance, NULL);
	m_hBtnJoin = CreateWindow(TEXT("button"), L"회원가입", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 180, 100, 20, hWndMain,
		(HMENU)IDC_LOGIN_BTS_JOIN, hInstance, NULL);
	CreateWindow(TEXT("static"), L"ID", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 100, 40, 25, hWndMain,
		(HMENU)IDC_STATIC, hInstance, NULL);
	CreateWindow(TEXT("static"), L"PW", WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 140, 40, 25, hWndMain,
		(HMENU)IDC_STATIC, hInstance, NULL);
	m_hEditId = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 50, 100, 200, 25, hWndMain,
		(HMENU)IDC_LOGIN_EDIT_ID, hInstance, NULL);
	m_hEditPw = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 50, 140, 200, 25, hWndMain,
		(HMENU)IDC_LOGIN_EDIT_PW, hInstance, NULL);
}

void C_LOGININFO::getEditText()
{
	GetWindowText(m_hEditId, m_strId, 13);
	GetWindowText(m_hEditPw, m_strPw, 13);
	m_nIdLen = lstrlenW(m_strId);
	m_nPwLen = lstrlenW(m_strPw);
}

void C_LOGININFO::initEditText()
{
	SetWindowText(m_hEditId, L"");
	SetWindowText(m_hEditPw, L"");
}

