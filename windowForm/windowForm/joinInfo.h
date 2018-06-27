#pragma once

class C_JOININFO
{
private:
	HWND m_hEditId;
	HWND m_hEditNick;
	HWND m_hEditPw1;
	HWND m_hEditPw2;

	HWND m_hBtnIdOverlap;
	HWND m_hBtnNickOverlap;
	HWND m_hBtnOk;
	HWND m_hBtnCancel;

	bool m_bIdCheck;
	bool m_bNickCheck;

	WCHAR m_strId[13];
	WCHAR m_strNick[13];
	WCHAR m_strPw1[13];
	WCHAR m_strPw2[13];

	int m_nIdLen;
	int m_nNickLen;
	int m_nPwLen;


public:
	C_JOININFO();
	void initJoinControl(HWND hWndMain, HINSTANCE hInstance);

	void getEditText();
	void initEditText();

	bool idOverlapCheck();
	bool NickOverlapCheck();

	bool pwEqualCheck();
};
