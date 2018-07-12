#pragma once

#include <Windows.h>
#include "netDef.h"

class C_NET_VOICE_SERVER
{
private:
	SOCKET			m_sockUdp;
	HWND			m_hWnd;
	std::thread*	m_threadVoiceComm;
	std::mutex		m_mtxData;
	std::map<int, S_CLIENT_INFO*> m_mapClients;

private:
	void voiceWorkerThread();

public:
	C_NET_VOICE_SERVER();
	void init(HWND hWnd);
	void release();
	void serverEnd();
};
