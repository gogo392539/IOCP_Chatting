#pragma once

#include "netDef.h"
#include "dbServer.h"

class C_MAINSERVER
{
private:
	SOCKET		m_sockListen;
	HANDLE		m_hIOCP;
	int			m_nCountOfThread;
	HWND		m_hWnd;

	std::mutex						m_mtxData;
	std::vector<std::thread*>		m_vecWorkerThreads;
	std::map<int, S_HANDLE_DATE*>	m_mapClients;
	std::thread*					m_pThreadAccept;
	std::map<int, std::wstring>		m_mapAccessClientInfo;
	bool							m_bAcceptThreadSet;

private:
	C_DB_SERVER		m_cDbServer;


private:
	void acceptClient();
	void makeWorkerThread();
	void threadJoin();
	void workerThread();

public:
	C_MAINSERVER();
	void init(HWND hWnd);
	void release();
	void serverEnd();
	void closeClient();

};
