#pragma once

#include <Windows.h>
#include "chatClient.h"
#include "netJoin.h"

class C_MYWIN
{
private://basic handle
	HINSTANCE	m_hInstance;
	HWND		m_hWndLogin;
	HWND		m_hWndJoin;
	HWND		m_hWndComm;

	int			m_nWinPosX;
	int			m_nWinPosY;
	int			m_nWinHeight;
	int			m_nWinWidth;

private:
	C_NET_CHAT  m_cNetChat;
	C_NET_JOIN	m_cNetJoin;

private://login Handle
	/*HWND m_hBtnOk;
	HWND m_hBtnCancel;
	HWND m_hBtnJoin;*/
	HWND		m_hEditLoginId;
	HWND		m_hEditLoginPw;

private://join Handle
	HWND		m_hEditJoinId;
	HWND		m_hEditJoinNick;
	HWND		m_hEditJoinPw1;
	HWND		m_hEditJoinPw2;
	bool		m_bIdOverlapCheck;
	bool		m_bNickOverlapCheck;
	/*HWND m_hBtnIdOverlap;
	HWND m_hBtnNickOverlap;
	HWND m_hBtnOk;
	HWND m_hBtnCancel;*/

private://comm Handle
	/*HWND m_hBtnOk;
	HWND m_hBtnCancel;
	HWND m_hBtnVoiceOk;
	HWND m_hBtnVoiceCancel;*/
	HWND		m_hEditCommInputText;
	HWND		m_hEditCommChat;
	WCHAR		m_strInputText[128];
	WCHAR		m_strChatText[2048];
	int			m_nInputTextLen;
	bool		m_bVoiceStart;
	bool		m_bVoiceEnd;

private:
	static C_MYWIN* m_pMyWin;

public:
	static void createWin();
	static C_MYWIN* getWin();
	static void releaseWin();

private:
	C_MYWIN();
	static LRESULT CALLBACK wndLoginProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK myLoginProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK wndJoinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK myJoinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK wndCommProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK myCommProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	bool init(HINSTANCE hInstance);
	void updateMsg();
	void release();
};
