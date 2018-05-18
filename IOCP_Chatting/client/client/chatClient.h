#pragma once

class C_CHAT_CLIENT
{
private:
	static const char* SERVER_IP;
	enum
	{
		E_BUF_MAX = 256,
		E_PACKET_MAX = 264,
		E_SERVER_PORT = 20000
	};
	struct packet
	{
		int nId;
		int nBufLen;
		char dataBuf[E_BUF_MAX];
	};
private:
	SOCKET m_sockClient;
	std::thread* threadRecv;

public:
	C_CHAT_CLIENT();
	void init();
	void release();

private:
	void makeRecvThread();
	void workerThread();
	void errorMessage(const char *msg, int err_no, int line);
	

};