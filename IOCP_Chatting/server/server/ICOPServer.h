#pragma once

#include "netDef.h"

class C_IOCPSERVER
{
private:
	SOCKET		m_sockListen;
	HANDLE		m_hIOCP;
	int			m_nCountOfThread;

	std::mutex						m_mtxData;
	std::vector<std::thread*>		m_vecWorkerThreads;
	std::map<int, S_HANDLE_DATE*>	m_mapClients;

private:
	void acceptClient();
	void makeWorkerThread();
	void sendMessage(S_PACKET * pPacket);
	void workerThreadJoin();
	void workerThread();

	void errorMessage(const char *msg, int err_no, int line);

public:
	C_IOCPSERVER();
	void init();
	void release();
	
};
