#pragma once

#include "netDef.h"

class C_JOIN_SERVER
{
private:
	SOCKET		m_sockListen;
	HANDLE		m_hIOCP;
	int			m_nCountOfThread;
	HWND		m_hWnd;

	std::mutex							m_mtxData;
	std::vector<std::thread*>			m_vecWorkerThreads;
	std::map<int, S_JOIN_HANDLE_DATE*>	m_mapJoinClients;
	std::thread*						m_pThreadAccept;
	int									m_nMaxSerialId;
	int									m_nClientCount;

private:
	C_DB_SERVER		m_cDbServer;


private:
	void acceptJoinClient();
	void makeWorkerThread();
	void threadJoin();
	void workerThread();

public:
	C_JOIN_SERVER();
	void init(HWND hWnd);
	void release();
	void serverEnd();
	void closeClient();

};
