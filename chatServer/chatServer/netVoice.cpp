#include "stdafx.h"

void C_NET_VOICE_SERVER::voiceWorkerThread()
{
}

C_NET_VOICE_SERVER::C_NET_VOICE_SERVER() :
	m_sockUdp(0),
	m_hWnd(NULL),
	m_threadVoiceComm(nullptr)
{
}

void C_NET_VOICE_SERVER::init(HWND hWnd)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("[ WSAStartup Error - %d ]", __LINE__);
	}

	m_hWnd = hWnd;

	m_sockUdp = socket(PF_INET, SOCK_DGRAM, 0);
	if (m_sockUdp == INVALID_SOCKET)
	{
		int nErrNo = WSAGetLastError();
		//errorMessage("bind Error", nErrNo, __LINE__);
		WCHAR strErr[128] = L"";
		int len = swprintf_s(strErr, 128, L"bind Error [%d - %d]", nErrNo, __LINE__);
		MessageBox(m_hWnd, strErr, L"오류", MB_OK);
	}

	SOCKADDR_IN sockAddrUdp = {};
	ZeroMemory(&sockAddrUdp, sizeof(SOCKADDR_IN));
	sockAddrUdp.sin_family = AF_INET;
	sockAddrUdp.sin_addr.s_addr = htonl(INADDR_ANY);
	sockAddrUdp.sin_port = htons(E_UDP_SERVER_PORT);

	if (::bind(m_sockUdp, (sockaddr*)&sockAddrUdp, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		{
			int nErrNo = WSAGetLastError();
			//errorMessage("bind Error", nErrNo, __LINE__);
			WCHAR strErr[128] = L"";
			int len = swprintf_s(strErr, 128, L"bind Error [%d - %d]", nErrNo, __LINE__);
			MessageBox(m_hWnd, strErr, L"오류", MB_OK);
		}
	}

	m_threadVoiceComm = new std::thread(&C_NET_VOICE_SERVER::voiceWorkerThread, this);
}

void C_NET_VOICE_SERVER::release()
{
	m_threadVoiceComm->join();
	delete m_threadVoiceComm;
	m_threadVoiceComm = nullptr;

	closesocket(m_sockUdp);
	WSACleanup();
}

void C_NET_VOICE_SERVER::serverEnd()
{
}
