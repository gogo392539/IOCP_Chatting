#include "stdafx.h"

//const char* C_NET_VOICE::SERVER_IP = "192.168.1.145";
const char* C_NET_VOICE::SERVER_IP = "192.168.0.222";

C_NET_VOICE::C_NET_VOICE() : 
	m_hWnd(NULL),
	m_sockAddrServerInfo{},
	m_sockUdp(0),
	m_threadVoiceComm(nullptr),
	m_nMySerialId(0),
	m_cVoice(),
	m_bWorkerThreadSet()
{
}

void C_NET_VOICE::init(HWND hWnd, int nMySerialId)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("[ WSAStartup Error - %d ]", __LINE__);
		exit(1);
	}

	m_hWnd = hWnd;
	m_nMySerialId = nMySerialId;
	m_bWorkerThreadSet = true;
	m_cVoice.init();

	m_sockUdp = socket(PF_INET, SOCK_DGRAM, 0);
	if (m_sockUdp == INVALID_SOCKET)
	{
		printf("[ socket Error - %d ]", __LINE__);
		exit(1);
	}

	memset(&m_sockAddrServerInfo, 0, sizeof(SOCKADDR_IN));
	m_sockAddrServerInfo.sin_family = AF_INET;
	m_sockAddrServerInfo.sin_port = htons(E_UDP_SERVER_PORT);
	InetPtonA(m_sockAddrServerInfo.sin_family, SERVER_IP, &m_sockAddrServerInfo.sin_addr.S_un.S_addr);

	connect(m_sockUdp, (SOCKADDR*)&m_sockAddrServerInfo, sizeof(SOCKADDR_IN));

	m_threadVoiceComm = new std::thread(&C_NET_VOICE::voiceWorkerThread, this);
}

void C_NET_VOICE::voiceWorkerThread()
{
	int nResult = 0;
	char strVoiceData[E_VOICE_DATA_LENGHT] = {};

	ULONG isNonBlocking = 1;
	ioctlsocket(m_sockUdp,        //Non-Blocking으로 변경할 소켓
		FIONBIO,       //변경할 소켓의 입출력 모드
		&isNonBlocking //넘기는 인자, 여기서는 nonblocking설정 값
	);


	while (m_bWorkerThreadSet)
	{
		ZeroMemory(strVoiceData, E_VOICE_DATA_LENGHT);
		nResult = recv(m_sockUdp, (char*)&strVoiceData, E_VOICE_DATA_LENGHT, 0);
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

		//voice 출력
		m_cVoice.waveOutVoice(strVoiceData, nResult);
	}
}

void C_NET_VOICE::sendClientVoiceStart()
{
	S_UDP_VOICE_PACKET sUdpVoicePacket = {};
	sUdpVoicePacket.eType = E_VOICE_PACKET_TYPE::E_ID_CHECK;
	sUdpVoicePacket.nSerialId = m_nMySerialId;
	int nResult = send(m_sockUdp, (char*)&sUdpVoicePacket, E_VOICE_PACKET_TYPE_SIZE + E_VOICE_ID_SIZE, 0);
	if (nResult == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		WCHAR strErr[128] = L"";
		int len = swprintf_s(strErr, 128, L"send Error [%d - %d]", nErrNo, __LINE__);
		MessageBox(m_hWnd, strErr, L"오류", MB_OK);
	}
}

void C_NET_VOICE::sendClientVoiceEnd()
{
	S_UDP_VOICE_PACKET sUdpVoicePacket = {};
	sUdpVoicePacket.eType = E_VOICE_PACKET_TYPE::E_SERVER_END;
	sUdpVoicePacket.nSerialId = m_nMySerialId;
	int nResult = send(m_sockUdp, (char*)&sUdpVoicePacket, E_VOICE_PACKET_TYPE_SIZE + E_VOICE_ID_SIZE, 0);
	if (nResult == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		WCHAR strErr[128] = L"";
		int len = swprintf_s(strErr, 128, L"send Error [%d - %d]", nErrNo, __LINE__);
		MessageBox(m_hWnd, strErr, L"오류", MB_OK);
	}
}

void C_NET_VOICE::sendClientVoiceData(const char* strVoiceData, int nDataLen)
{
	S_UDP_VOICE_PACKET sUdpVoicePacket = {};
	sUdpVoicePacket.eType = E_VOICE_PACKET_TYPE::E_DATA;
	sUdpVoicePacket.nSerialId = m_nMySerialId;
	memcpy_s(sUdpVoicePacket.strVoiceMsg, E_VOICE_DATA_LENGHT, strVoiceData, nDataLen);
	int nResult = send(m_sockUdp, (char*)&sUdpVoicePacket, E_VOICE_PACKET_TYPE_SIZE + E_VOICE_ID_SIZE + nDataLen, 0);
	if (nResult == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		WCHAR strErr[128] = L"";
		int len = swprintf_s(strErr, 128, L"send Error [%d - %d]", nErrNo, __LINE__);
		MessageBox(m_hWnd, strErr, L"오류", MB_OK);
	}
}

void C_NET_VOICE::voiceCommStart()
{
	m_cVoice.waveInOpenDevice(m_hWnd);
	m_cVoice.waveOutOpenDevice(m_hWnd);
}

void C_NET_VOICE::voiceCommEnd()
{
	m_cVoice.waveInEnd();
}

void C_NET_VOICE::voiceInStart()
{
}

void C_NET_VOICE::voiceInReuseQueue()
{
}

C_VOICE * C_NET_VOICE::getVoiceClass()
{
	return &m_cVoice;
}

void C_NET_VOICE::release()
{
	m_threadVoiceComm->join();
	delete m_threadVoiceComm;
	m_threadVoiceComm = nullptr;

	m_cVoice.release();

	closesocket(m_sockUdp);
	WSACleanup();
}

void C_NET_VOICE::serverEnd()
{
	m_bWorkerThreadSet = false;
}
