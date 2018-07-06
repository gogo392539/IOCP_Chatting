#pragma once

#include "netDef.h"

class C_NET_JOIN
{
private:
	static const char* SERVER_IP;

private:
	SOCKET			m_sockClient;
	std::thread*	m_threadRecv;

	HWND			m_hEditComm;
	bool			m_bWorkThread;
	bool			m_bIdCheck;
	bool			m_bNickCheck;

public:
	C_NET_JOIN();
	void init(HWND hWnd);
	void release();

	bool getIdCheck();
	bool getNickCheck();

	void setIdCheck(bool bCheck = false);
	void setNickCheck(bool bCheck = false);
	
	void sendIdCheckMessage(LPCWSTR wstrId, int nIdLen);
	void sendNickCheckMessage(LPCWSTR wstrNick, int nNickLen);
	void sendJoinMessage(LPCWSTR wstrId, int nIdLen, LPCWSTR wstrNick, int nNickLen, LPCWSTR wstrPw, int nPwLen);
	void sendExitMessage();


private:
	void makeThread();
	void workerRecvThread();


};
