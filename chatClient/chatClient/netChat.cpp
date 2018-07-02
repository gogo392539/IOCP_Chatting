#include "stdafx.h"

const char* C_NET_CHAT::SERVER_IP = "127.0.0.1";

C_NET_CHAT::C_NET_CHAT() :
	m_sockClient(),
	m_threadRecv(),
	m_threadSend(),
	m_nMyId(0)

{
}

void C_NET_CHAT::init()
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("[ WSAStartup Error - %d ]", __LINE__);
	}

	m_sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sockClient == INVALID_SOCKET)
	{
		int nErrNo = WSAGetLastError();
		errorMessage("socket error", nErrNo, __LINE__);
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
		errorMessage("connect error", nErrNo, __LINE__);
	}

	sendLoginMessage();

	makeThread();
}

void C_NET_CHAT::release()
{
	m_threadRecv->join();
	delete m_threadRecv;
	m_threadRecv = nullptr;
	m_threadSend->join();
	delete m_threadSend;
	m_threadSend = nullptr;

	closesocket(m_sockClient);
	WSACleanup();
}

void C_NET_CHAT::sendLoginMessage()
{
	S_PACKET sPacket = {};
	sPacket.eType = E_PACKET_TYPE::E_LOGIN;
	sPacket.nBufLen = 12;
	scanf_s("%d", &sPacket.nId);
	m_nMyId = sPacket.nId;
	int nRetval = send(m_sockClient, (const char*)&sPacket, sPacket.nBufLen, 0);
	if (nRetval == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		errorMessage("send error", nErrNo, __LINE__);
	}
}

void C_NET_CHAT::makeThread()
{
	m_threadRecv = new std::thread(&C_NET_CHAT::workerRecvThread, this);
	m_threadSend = new std::thread(&C_NET_CHAT::workerSendThread, this);
}

void C_NET_CHAT::workerRecvThread()
{
	while (1)
	{
		S_PACKET sPacket = {};
		int nRetval = recv(m_sockClient, (char*)&sPacket, E_PACKET_MAX, 0);
		if (nRetval == SOCKET_ERROR)
		{
			int nErrNo = WSAGetLastError();
			errorMessage("recv error", nErrNo, __LINE__);
		}

		if (sPacket.eType == E_PACKET_TYPE::E_LOGIN)
		{
			printf("ID : %d Login \n", sPacket.nId);
			//m_nMyId = sPacket.nId;
		}
		else if (sPacket.eType == E_PACKET_TYPE::E_LOGOUT)
		{
			printf("ID : %d Logout \n", sPacket.nId);
		}
		else if (sPacket.eType == E_PACKET_TYPE::E_DATA)
		{
			printf_s("ID : %d -> %s \n", sPacket.nId, sPacket.dataBuf);
		}
	}
}

void C_NET_CHAT::workerSendThread()
{
	while (1)
	{
		S_PACKET sPacket = {};
		sPacket.eType = E_PACKET_TYPE::E_DATA;
		sPacket.nId = m_nMyId;
		scanf_s("%s", sPacket.dataBuf, E_BUF_CHAT_MAX);
		sPacket.nBufLen = strlen(sPacket.dataBuf) + 12;
		int nRetval = send(m_sockClient, (const char*)&sPacket, sPacket.nBufLen, 0);
		if (nRetval == SOCKET_ERROR)
		{
			int nErrNo = WSAGetLastError();
			errorMessage("send error", nErrNo, __LINE__);
		}
	}
}

void C_NET_CHAT::errorMessage(const char * msg, int err_no, int line)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[ %s - %d ]", msg, line);
	wprintf(L"¿¡·¯ %s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
	exit(-1);
}
