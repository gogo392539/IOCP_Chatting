#pragma once

#include <Windows.h>
#include "voice.h"
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
	bool			m_bWorkerThreadSet;

private:
	C_VOICE			m_cVoice;

private:
	void voiceWorkerThread();

public:
	void sendClientVoiceStart();
	void sendClientVoiceEnd();
	void sendClientVoiceData(const char* strVoiceData, int nDataLen);

public:
	void voiceCommStart();
	void voiceCommEnd();
	void voiceInStart();
	void voiceInReuseQueue();

	C_VOICE* getVoiceClass();


public:
	C_NET_VOICE();
	void init(HWND hWnd, int nMySerialId);
	void release();
	void serverEnd();

};
