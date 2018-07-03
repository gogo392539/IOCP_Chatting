#include "stdafx.h"

const char* C_NET_CHAT::SERVER_IP = "192.168.1.145";

C_NET_CHAT::C_NET_CHAT() :
	m_sockClient(),
	m_threadRecv(),
	m_nMyId(0),
	m_bLoginSuccess(),
	m_bLoginFail(),
	m_bWorkThread(true)

{
}

void C_NET_CHAT::init()
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("[ WSAStartup Error - %d ]", __LINE__);
	}

	m_bLoginSuccess = false;
	m_bLoginFail = false;
	m_bWorkThread = true;

	m_sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sockClient == INVALID_SOCKET)
	{
		int nErrNo = WSAGetLastError();
		//errorMessage("socket error", nErrNo, __LINE__);
		exit(1);
	}

	SOCKADDR_IN sockAddrClient;
	memset(&sockAddrClient, 0, sizeof(SOCKADDR_IN));
	sockAddrClient.sin_family = PF_INET;
	sockAddrClient.sin_port = htons(E_SERVER_PORT);
	InetPtonA(sockAddrClient.sin_family, SERVER_IP, &sockAddrClient.sin_addr.S_un.S_addr);

	if (connect(m_sockClient, (SOCKADDR*)&sockAddrClient, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		closesocket(m_sockClient);
		WSACleanup();
		exit(1);
	}


	makeThread();
}

void C_NET_CHAT::release()
{
	m_threadRecv->join();
	delete m_threadRecv;
	m_threadRecv = nullptr;

	closesocket(m_sockClient);
	WSACleanup();
}

void C_NET_CHAT::sendLoginMessage(LPCWSTR strId, int nIdLen, LPCWSTR strPw, int nPwLen)
{
	S_CTS_LOGIN_PACKET sPacket = {};
	sPacket.eType = E_PACKET_TYPE::E_LOGIN_CALL;
	sPacket.nDataSize = nIdLen * 2 + nPwLen * 2 + 12;
	sPacket.nIdLen = nIdLen;
	sPacket.nPwLen = nPwLen;
	lstrcatW(sPacket.strData, strId);
	lstrcatW(sPacket.strData, strPw);
	
	int nRetval = send(m_sockClient, (const char*)&sPacket, sPacket.nDataSize + 4, 0);
	if (nRetval == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		//errorMessage("send error", nErrNo, __LINE__);
		exit(1);
	}
}

void C_NET_CHAT::sendLogoutMessage()
{
	S_CTS_LOGOUT_PACKET sPacket = {};
	sPacket.eType = E_PACKET_TYPE::E_LOGOUT;
	sPacket.nSerialId = m_nMyId;

	int nRetval = send(m_sockClient, (const char*)&sPacket, E_LOGOUT_PACKET_SIZE, 0);
	if (nRetval == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		//errorMessage("send error", nErrNo, __LINE__);
		exit(1);
	}
}

bool C_NET_CHAT::getLoginSuccessCheck()
{
	return m_bLoginSuccess;
}

bool C_NET_CHAT::getLoginFailCheck()
{
	return m_bLoginFail;
}

void C_NET_CHAT::makeThread()
{
	m_threadRecv = new std::thread(&C_NET_CHAT::workerRecvThread, this);
}

void C_NET_CHAT::workerRecvThread()
{
	while (m_bWorkThread)
	{
		E_PACKET_TYPE eType = E_PACKET_TYPE::E_NONE;
		int nRetval = recv(m_sockClient, (char*)&eType, E_PACKET_TYPE_LENGTH, 0);
		if (nRetval == SOCKET_ERROR)
		{
			int nErrNo = WSAGetLastError();
			exit(1);
		}
		
		switch (eType)
		{
		case E_PACKET_TYPE::E_LOGIN_SUCCESS:
		{
			int nRetval = recv(m_sockClient, (char*)&m_nMyId, 4, 0);
			if (nRetval == SOCKET_ERROR)
			{
				int nErrNo = WSAGetLastError();
				exit(1);
			}
			m_bLoginSuccess = true;
		}
			break;
		case E_PACKET_TYPE::E_LOGOUT:
		{
			m_bWorkThread = false;
		}
			break;
		}

	}

}
