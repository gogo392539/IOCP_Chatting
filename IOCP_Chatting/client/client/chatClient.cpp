#include "stdafx.h"

const char* C_CHAT_CLIENT::SERVER_IP = "127.0.0.1";

C_CHAT_CLIENT::C_CHAT_CLIENT() :
	m_sockClient(),
	threadRecv()
{
}

void C_CHAT_CLIENT::init()
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

	makeRecvThread();

	while (1) 
	{
		packet sPacket = {};
		scanf_s("%s", sPacket.dataBuf, E_BUF_MAX);
		send(m_sockClient, (const char*)&sPacket, E_PACKET_MAX, 0);
	}
}

void C_CHAT_CLIENT::release()
{
	threadRecv->join();
	delete threadRecv;

	closesocket(m_sockClient);
	WSACleanup();
}

void C_CHAT_CLIENT::makeRecvThread()
{
	threadRecv = new std::thread(&C_CHAT_CLIENT::workerThread, this);
}

void C_CHAT_CLIENT::workerThread()
{
	while (1)
	{
		packet sPacket = {};
		recv(m_sockClient, (char*)&sPacket, E_PACKET_MAX, 0);
		printf("%s \n", sPacket.dataBuf);
	}
}

void C_CHAT_CLIENT::errorMessage(const char * msg, int err_no, int line)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[ %s - %d ]", msg, line);
	wprintf(L"¿¡·¯ %s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
	exit(-1);
}
