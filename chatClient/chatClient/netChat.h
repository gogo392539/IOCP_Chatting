#pragma once

#include "netDef.h"

class C_NET_CHAT
{
private:
	static const char* SERVER_IP;

private:
	SOCKET			m_sockClient;
	std::thread*	m_threadRecv;

	int				m_nMyId;
	bool			m_bLoginSuccess;
	bool			m_bLoginFail;
	bool			m_bWorkThread;

	HWND			m_hEditComm;

public:
	C_NET_CHAT();
	void init(HWND hEditComm);
	void release();

public:
	void sendLoginMessage(LPCWSTR wstrId, int nIdLen, LPCWSTR wstrPw, int nPwLen);
	void sendLogoutMessage();
	void sendMsgMessage(int nMsgLen, LPCWSTR wstrMsg);

	bool getLoginSuccessCheck();
	bool getLoginFailCheck();

private:
	void makeThread();
	void workerRecvThread();

	
};
