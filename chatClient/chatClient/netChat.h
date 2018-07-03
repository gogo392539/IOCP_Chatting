#pragma once

#include "netDef.h"

class C_NET_CHAT
{
private:
	static const char* SERVER_IP;

private:
	SOCKET m_sockClient;
	std::thread* m_threadRecv;

	int m_nMyId;
	bool m_bLoginSuccess;
	bool m_bLoginFail;
	bool m_bWorkThread;

public:
	C_NET_CHAT();
	void init();
	void release();

public:
	void sendLoginMessage(LPCWSTR strId, int nIdLen, LPCWSTR strPw, int nPwLen);
	void sendLogoutMessage();

	bool getLoginSuccessCheck();
	bool getLoginFailCheck();

private:
	void makeThread();
	void workerRecvThread();


};
