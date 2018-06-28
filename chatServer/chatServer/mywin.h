#pragma once

#include <Windows.h>

class C_MYWIN
{
private:
	HINSTANCE	m_hInstance;
	HWND		m_hWnd;

	HWND		m_hEditUserJoinList;
	HWND		m_hEditUserLoginList;
	
	HWND		m_hBtnServerEnd;
	HWND		m_hBtnServerStart;
	HWND		m_hBtnShowList;
	HWND		m_hBtnEraseList;

private:
	static C_MYWIN* m_pMyWin;

public:
	static void createWin();
	static C_MYWIN* getWin();
	static void releaseWin();

private:
	C_MYWIN();
	static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK myProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	bool init(HINSTANCE hInstance);
	void updateMsg();
	void release();

};
