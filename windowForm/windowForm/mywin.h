#pragma once

#include <Windows.h>
#include "loginInfo.h"
#include "joinInfo.h"
#include "commInfo.h"

class C_MYWIN
{
private:
	HINSTANCE	m_hInstance;
	HWND		m_hWndLogin;
	HWND		m_hWndJoin;
	HWND		m_hWndComm;

	int			m_nWinPosX;
	int			m_nWinPosY;
	int			m_nWinHeight;
	int			m_nWinWidth;

private:
	C_LOGININFO m_cLoginInfo;
	C_JOININFO m_cJoinInfo;
	C_COMMINFO m_cCommInfo;

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
