#pragma once

class C_COMMINFO
{
private:
	HWND m_hBtnOk;
	HWND m_hBtnCancel;
	HWND m_hBtnVoiceOk;
	HWND m_hBtnVoiceCancel;
	HWND m_hEditInputText;
	HWND m_hEditChat;

	WCHAR m_strInputText[128];
	int m_nInputTextLen;

	WCHAR m_strChatText[2048];

public:
	C_COMMINFO();
	void initCommControl(HWND hWndMain, HINSTANCE hInstance);

	void transInputText();
	void initEditText();
};
