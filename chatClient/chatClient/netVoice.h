#pragma once

#include <Windows.h>
#include "netDef.h"

class C_NET_VOICE
{
private:
	static const char* SERVER_IP;

private:
	SOCKET			m_sockUdp;
	SOCKADDR_IN		m_sockAddrServerInfo;
	HWND			m_hWnd;
	std::thread*	m_threadVoiceComm;
	int				m_nMySerialId;

private:
	void voiceWorkerThread();

public:
	void sendClientSockInfo();
	void sendClientVoiceData();

public:
	C_NET_VOICE();
	void init(HWND hWnd, int nMySerialId);
	void release();
	void serverEnd();

};
