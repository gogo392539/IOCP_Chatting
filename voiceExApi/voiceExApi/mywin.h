#pragma once

#include <Windows.h>
#include "voice.h"

class C_MYWIN
{
private:
	HINSTANCE	m_hInstance;
	HWND		m_hWnd;

	C_VOICE		m_cVoice;

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
	void update();
	void release();
};
