#pragma once

#include "netDef.h"

class C_CHAT_CLIENT
{
private:
	static const char* SERVER_IP;

private:
	SOCKET m_sockClient;
	std::thread* m_threadRecv;
	std::thread* m_threadSend;

	int m_nMyId;

public:
	C_CHAT_CLIENT();
	void init();
	void release();

private:
	void sendLoginMessage();
	void makeThread();
	void workerRecvThread();
	void workerSendThread();
	void errorMessage(const char *msg, int err_no, int line);
	

};