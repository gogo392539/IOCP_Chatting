#include "stdafx.h"

C_NET_VOICE_SERVER::C_NET_VOICE_SERVER() :
	m_sockUdp(0),
	m_hWnd(NULL),
	m_threadVoiceComm(nullptr),
	m_mapClients(),
	m_mtxData(),
	m_bWorkerThreadSet(),
	m_cDBServer()
{
	m_mapClients.clear();
}

void C_NET_VOICE_SERVER::init(HWND hWnd)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("[ WSAStartup Error - %d ]", __LINE__);
	}

	m_hWnd = hWnd;
	m_bWorkerThreadSet = true;
	m_cDBServer.init();

	m_sockUdp = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sockUdp == INVALID_SOCKET)
	{
		int nErrNo = WSAGetLastError();
		WCHAR strErr[128] = L"";
		int len = swprintf_s(strErr, 128, L"socket Error [%d - %d]", nErrNo, __LINE__);
		MessageBox(m_hWnd, strErr, L"오류", MB_OK);
	}

	SOCKADDR_IN sockAddrUdp = {};
	ZeroMemory(&sockAddrUdp, sizeof(SOCKADDR_IN));
	sockAddrUdp.sin_family = AF_INET;
	sockAddrUdp.sin_addr.s_addr = htonl(INADDR_ANY);
	sockAddrUdp.sin_port = htons(E_UDP_SERVER_PORT);

	if (bind(m_sockUdp, (sockaddr*)&sockAddrUdp, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		WCHAR strErr[128] = L"";
		int len = swprintf_s(strErr, 128, L"bind Error [%d - %d]", nErrNo, __LINE__);
		MessageBox(m_hWnd, strErr, L"오류", MB_OK);
	}

	m_threadVoiceComm = new std::thread(&C_NET_VOICE_SERVER::voiceWorkerThread, this);
}

void C_NET_VOICE_SERVER::voiceWorkerThread()
{
	SOCKADDR_IN sockAddrRecv = {};
	int nSockAddrLen = sizeof(SOCKADDR_IN);
	int nResult = 0;
	std::map<int, S_CLIENT_INFO*>::iterator iterClient;

	ULONG isNonBlocking = 1;
	ioctlsocket(m_sockUdp,        //Non-Blocking으로 변경할 소켓
		FIONBIO,       //변경할 소켓의 입출력 모드
		&isNonBlocking //넘기는 인자, 여기서는 nonblocking설정 값
	);

	while (m_bWorkerThreadSet)
	{
		S_UDP_VOICE_PACKET sUdpVoicePacket = {};
		nResult = recvfrom(m_sockUdp, (char*)&sUdpVoicePacket, E_VOICE_MAX_PACKET_LENGHT, 0, (SOCKADDR*)&sockAddrRecv, &nSockAddrLen);
		if (nResult == SOCKET_ERROR)
		{
			int nErrNo = WSAGetLastError();
			if (nErrNo != WSAEWOULDBLOCK) {
				WCHAR strErr[128] = L"";
				int len = swprintf_s(strErr, 128, L"recvfrom Error [%d - %d]", nErrNo, __LINE__);
				MessageBox(m_hWnd, strErr, L"오류", MB_OK);
			}
			else {
				continue;
			}
		}

		switch (sUdpVoicePacket.eType)
		{
		case E_VOICE_PACKET_TYPE::E_ID_CHECK:
		{
			S_CLIENT_INFO sClientInfo = {};
			sClientInfo.nSerialId = sUdpVoicePacket.nSerialId;
			memcpy(&sClientInfo.sockAddrClient, &sockAddrRecv, sizeof(SOCKADDR_IN));
			m_mtxData.lock();
			sClientInfo.iter = m_mapClients.find(sClientInfo.nSerialId);
			sClientInfo.iter = m_mapClients.insert(sClientInfo.iter, std::map<int, S_CLIENT_INFO*>::value_type(sClientInfo.nSerialId, &sClientInfo));
			m_mtxData.unlock();

			m_cDBServer.updateVoiceCheck(sUdpVoicePacket.nSerialId, true);

		}
			break;
		case E_VOICE_PACKET_TYPE::E_DATA:
		{
			iterClient = m_mapClients.begin();
			while (iterClient != m_mapClients.end())
			{
				if (iterClient->first != sUdpVoicePacket.nSerialId)
				{
					nResult = sendto(m_sockUdp, (char*)&sUdpVoicePacket.strVoiceMsg, E_VOICE_DATA_LENGHT, 0, (SOCKADDR*)&iterClient->second->sockAddrClient, sizeof(SOCKADDR_IN));
					if (nResult == SOCKET_ERROR)
					{
						int nErrNo = WSAGetLastError();
						WCHAR strErr[128] = L"";
						int len = swprintf_s(strErr, 128, L"sendto Error [%d - %d]", nErrNo, __LINE__);
						MessageBox(m_hWnd, strErr, L"오류", MB_OK);
					}
				}
				iterClient++;
			}
		}
			break;
		case E_VOICE_PACKET_TYPE::E_SERVER_END:
		{
			iterClient = m_mapClients.find(sUdpVoicePacket.nSerialId);
			m_mtxData.lock();
			m_mapClients.erase(iterClient);
			m_mtxData.unlock();

			m_cDBServer.updateVoiceCheck(sUdpVoicePacket.nSerialId, false);

		}
			break;
		}
	}
}

void C_NET_VOICE_SERVER::release()
{
	m_threadVoiceComm->join();
	delete m_threadVoiceComm;
	m_threadVoiceComm = nullptr;

	m_mapClients.clear();

	closesocket(m_sockUdp);
	WSACleanup();
}

void C_NET_VOICE_SERVER::serverEnd()
{
	m_bWorkerThreadSet = false;
}
