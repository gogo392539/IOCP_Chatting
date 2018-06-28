#pragma once

class C_LOGININFO
{
private:
	HWND m_hBtnOk;
	HWND m_hBtnCancel;
	HWND m_hBtnJoin;
	HWND m_hEditId;
	HWND m_hEditPw;

	WCHAR m_strId[13];
	WCHAR m_strPw[13];

	int m_nIdLen;
	int m_nPwLen;


public:
	C_LOGININFO();
	void initLoginControl(HWND hWndMain, HINSTANCE hInstance);
	void getEditText();
	void initEditText();
};
