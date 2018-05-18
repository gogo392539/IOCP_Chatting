#pragma once

class C_IOCPSERVER
{
private:
	enum
	{
		E_BUF_MAX = 256,
		E_PACKET_MAX = 264,

		E_SERVER_PORT = 20000,
	};
	struct packet
	{
		int nId;
		int nBufLen;
		char dataBuf[E_BUF_MAX];
	};
	struct S_HANDLE_DATE
	{
		SOCKET sockClient;
		int nId;
	};
	struct S_IO_DATA
	{
		OVERLAPPED overlapped;
		WSABUF wsaBuf;
		packet packetData;
	};

private:
	SOCKET	m_sockListen;
	HANDLE	m_hIOCP;
	int		m_nCountOfThread;
	std::mutex	m_mtxData;
	std::vector<std::thread*> m_vecWorkerThreads;

private:
	void acceptClient();
	void makeWorkerThread();
	void workerThreadJoin();
	void workerThread();

	void errorMessage(const char *msg, int err_no, int line);

public:
	C_IOCPSERVER();
	void init();
	void release();
	
};
