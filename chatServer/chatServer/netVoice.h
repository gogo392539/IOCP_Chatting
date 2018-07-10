#pragma once

#include <Windows.h>

class C_NET_VOICE_SERVER
{
private:
	enum
	{
		E_UDP_SERVER_PORT	= 20002,
		E_BUF_MAX			= 4800,
	};
private:
	SOCKET m_sockUdp;
	HWND m_hWnd;

	std::thread*	m_threadVoiceComm;

private:
	void voiceWorkerThread();

public:
	C_NET_VOICE_SERVER();
	void init(HWND hWnd);
	void release();
	void serverEnd();
};
