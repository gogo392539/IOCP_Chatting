#include "stdafx.h"

//const char* C_NET_JOIN::SERVER_IP = "192.168.1.145";
const char* C_NET_JOIN::SERVER_IP = "192.168.0.222";

C_NET_JOIN::C_NET_JOIN() :
	m_sockClient(),
	m_threadRecv(),
	m_hWndJoin(NULL),
	m_bWorkThread(),
	m_bIdCheck(),
	m_bNickCheck()
{
}

void C_NET_JOIN::init(HWND hWnd)
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("[ WSAStartup Error - %d ]", __LINE__);
	}

	m_bIdCheck = false;
	m_bNickCheck = false;
	m_bWorkThread = true;
	m_hWndJoin = hWnd;

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
	sockAddrClient.sin_port = htons(E_JOIN_SERVER_PORT);
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

void C_NET_JOIN::release()
{
	m_threadRecv->join();
	delete m_threadRecv;
	m_threadRecv = nullptr;

	closesocket(m_sockClient);
	WSACleanup();
}

bool C_NET_JOIN::getIdCheck()
{
	return m_bIdCheck;
}

bool C_NET_JOIN::getNickCheck()
{
	return m_bNickCheck;
}

void C_NET_JOIN::setIdCheck(bool bCheck)
{
	m_bIdCheck = bCheck;
}

void C_NET_JOIN::setNickCheck(bool bCheck)
{
	m_bNickCheck = bCheck;
}

void C_NET_JOIN::sendIdCheckMessage(LPCWSTR wstrId, int nIdLen)
{
	S_CTS_ID_CHECK_PACKET sCTSIdCheckPacket = {};
	sCTSIdCheckPacket.eType = E_JOIN_PACKET_TYPE::E_ID_CHECK;
	sCTSIdCheckPacket.nIdLen = nIdLen * 2;
	lstrcatW(sCTSIdCheckPacket.wstrId, wstrId);
	int nRetval = send(m_sockClient, (const char*)&sCTSIdCheckPacket, 
		sCTSIdCheckPacket.nIdLen + E_PACKET_TYPE_LENGTH_SIZE + E_DATA_LENGTH_SIZE, 0);
	if (nRetval == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		//errorMessage("send error", nErrNo, __LINE__);
		exit(1);
	}
}

void C_NET_JOIN::sendNickCheckMessage(LPCWSTR wstrNick, int nNickLen)
{
	S_CTS_NICK_CHECK_PACKET sCTSNickCheckPacket = {};
	sCTSNickCheckPacket.eType = E_JOIN_PACKET_TYPE::E_NICK_CHECK;
	sCTSNickCheckPacket.nNickLen = nNickLen * 2;
	lstrcatW(sCTSNickCheckPacket.wstrNick, wstrNick);
	
	int nRetval = send(m_sockClient, (const char*)&sCTSNickCheckPacket,
		sCTSNickCheckPacket.nNickLen + E_PACKET_TYPE_LENGTH_SIZE + E_DATA_LENGTH_SIZE, 0);
	if (nRetval == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		//errorMessage("send error", nErrNo, __LINE__);
		exit(1);
	}
}

void C_NET_JOIN::sendJoinMessage(LPCWSTR wstrId, int nIdLen, LPCWSTR wstrNick, int nNickLen, LPCWSTR wstrPw, int nPwLen)
{
	S_CTS_JOIN_PACKET sCtsJoinPacket = {};
	sCtsJoinPacket.eType = E_JOIN_PACKET_TYPE::E_JOIN;
	sCtsJoinPacket.nIdLen = nIdLen;
	sCtsJoinPacket.nNickLen = nNickLen;
	sCtsJoinPacket.nPwLen = nPwLen;
	//sCtsJoinPacket.nDataSize = (sCtsJoinPacket.nIdLen + sCtsJoinPacket.nNickLen + sCtsJoinPacket.nPwLen) * 2 + 12;
	lstrcatW(sCtsJoinPacket.wstrData, wstrId);
	lstrcatW(sCtsJoinPacket.wstrData, wstrNick);
	lstrcatW(sCtsJoinPacket.wstrData, wstrPw);
	sCtsJoinPacket.nDataSize = (lstrlenW(sCtsJoinPacket.wstrData)) * 2 + 12;

	int nRetval = send(m_sockClient, (const char*)&sCtsJoinPacket,
		sCtsJoinPacket.nDataSize + E_PACKET_TYPE_LENGTH_SIZE + E_DATA_LENGTH_SIZE, 0);
	if (nRetval == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		//errorMessage("send error", nErrNo, __LINE__);
		exit(1);
	}
}

void C_NET_JOIN::sendExitMessage()
{
	S_EXIT_MESSATE_PACKET sExitMsgPacket = {};
	sExitMsgPacket.eType = E_JOIN_PACKET_TYPE::E_EXIT;
	int nRetval = send(m_sockClient, (const char*)&sExitMsgPacket, E_PACKET_TYPE_LENGTH_SIZE, 0);
	if (nRetval == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		//errorMessage("send error", nErrNo, __LINE__);
		exit(1);
	}
}

void C_NET_JOIN::makeThread()
{
	m_threadRecv = new std::thread(&C_NET_JOIN::workerRecvThread, this);
}

void C_NET_JOIN::workerRecvThread()
{
	int nRetval = 0;
	while (m_bWorkThread)
	{
		E_JOIN_PACKET_TYPE eType = E_JOIN_PACKET_TYPE::E_NONE;
		nRetval = recv(m_sockClient, (char*)&eType, E_PACKET_TYPE_LENGTH_SIZE, 0);
		if (nRetval == SOCKET_ERROR)
		{
			int nErrNo = WSAGetLastError();
			exit(1);
		}

		switch (eType)
		{
		case E_JOIN_PACKET_TYPE::E_ID_CHECK_SUCCESS:
		{
			m_bIdCheck = true;
			MessageBox(m_hWndJoin, L"ID 중복확인 성공", L"ID 중복확인", MB_OK);
		}
			break;
		case E_JOIN_PACKET_TYPE::E_ID_CHECK_FAIL:
		{
			m_bIdCheck = false;
			MessageBox(m_hWndJoin, L"ID 중복확인 실패", L"ID 중복확인", MB_OK);
		}
			break;
		case E_JOIN_PACKET_TYPE::E_NICK_CHECK_SUCCESS:
		{
			m_bNickCheck = true;
			MessageBox(m_hWndJoin, L"NICKNAME 중복확인 성공", L"NICKNAME 중복확인", MB_OK);
		}
			break;
		case E_JOIN_PACKET_TYPE::E_NICK_CHECK_FAIL:
		{
			m_bNickCheck = false;
			MessageBox(m_hWndJoin, L"NICKNAME 중복확인 실패", L"NICKNAME 중복확인", MB_OK);
		}
			break;
		case E_JOIN_PACKET_TYPE::E_JOIN:
		{
			m_bWorkThread = false;
			//MessageBox(m_hWndJoin, L"회원가입 성공", L"회원가입", MB_OK);
		}
			break;
		case E_JOIN_PACKET_TYPE::E_EXIT:
		{
			m_bWorkThread = false;
		}
			break;
		}

	}

}