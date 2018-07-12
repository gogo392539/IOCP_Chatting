#include "stdafx.h"

const char* C_NET_CHAT::SERVER_IP = "192.168.1.145";

C_NET_CHAT::C_NET_CHAT() :
	m_sockClient(),
	m_threadRecv(),
	m_nMyId(0),
	m_bLoginSuccess(),
	m_bLoginFail(),
	m_bWorkThread(true),
	m_hEditComm(NULL)

{
}

void C_NET_CHAT::init(HWND hEditComm)
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("[ WSAStartup Error - %d ]", __LINE__);
	}

	m_nMyId = -1;
	m_bLoginSuccess = false;
	m_bLoginFail = false;
	m_bWorkThread = true;
	m_hEditComm = hEditComm;

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

void C_NET_CHAT::sendLoginMessage(LPCWSTR wstrId, int nIdLen, LPCWSTR wstrPw, int nPwLen)
{
	S_CTS_LOGIN_PACKET sCTSLoginPacket = {};
	sCTSLoginPacket.eType = E_PACKET_TYPE::E_LOGIN_CALL;
	sCTSLoginPacket.nDataSize = nIdLen * 2 + nPwLen * 2 + 12;
	sCTSLoginPacket.nIdLen = nIdLen;
	sCTSLoginPacket.nPwLen = nPwLen;
	lstrcatW(sCTSLoginPacket.wstrData, wstrId);
	lstrcatW(sCTSLoginPacket.wstrData, wstrPw);
	
	int nRetval = send(m_sockClient, (const char*)&sCTSLoginPacket, sCTSLoginPacket.nDataSize + E_PACKET_TYPE_LENGTH_SIZE, 0);
	if (nRetval == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		//errorMessage("send error", nErrNo, __LINE__);
		exit(1);
	}
}

void C_NET_CHAT::sendLogoutMessage()
{
	S_CTS_LOGOUT_PACKET sCTSLogoutPacket = {};
	sCTSLogoutPacket.eType = E_PACKET_TYPE::E_LOGOUT;
	sCTSLogoutPacket.nSerialId = m_nMyId;

	int nRetval = send(m_sockClient, (const char*)&sCTSLogoutPacket, E_LOGOUT_PACKET_SIZE, 0);
	if (nRetval == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		//errorMessage("send error", nErrNo, __LINE__);
		exit(1);
	}
}

void C_NET_CHAT::sendMsgMessage(int nMsgLen, LPCWSTR wstrMsg)
{
	S_CTS_MSG_PACKET sCTSMsgPacket = {};
	sCTSMsgPacket.eType = E_PACKET_TYPE::E_MESSAGE;
	sCTSMsgPacket.nDataSize = (nMsgLen * 2) + 8;
	sCTSMsgPacket.nMgsLen = nMsgLen;
	lstrcatW(sCTSMsgPacket.wstrMsg, wstrMsg);
	
	int nRetval = send(m_sockClient, (const char*)&sCTSMsgPacket, sCTSMsgPacket.nDataSize + E_PACKET_TYPE_LENGTH_SIZE, 0);
	if (nRetval == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		//errorMessage("send error", nErrNo, __LINE__);
		exit(1);
	}
}

void C_NET_CHAT::sendVoiceCheckMessage(E_PACKET_TYPE eType)
{
	S_VOICE_PACKET sVoicePacket = {};
	sVoicePacket.eType = eType;
	sVoicePacket.nSerialId = m_nMyId;
	int nRetval = send(m_sockClient, (const char*)&sVoicePacket, E_VOICE_PACKET_SIZE, 0);
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

int C_NET_CHAT::getMySerialId()
{
	return m_nMyId;
}

void C_NET_CHAT::makeThread()
{
	m_threadRecv = new std::thread(&C_NET_CHAT::workerRecvThread, this);
}

void C_NET_CHAT::workerRecvThread()
{
	int nRetval = 0;
	while (m_bWorkThread)
	{
		E_PACKET_TYPE eType = E_PACKET_TYPE::E_NONE;
		nRetval = recv(m_sockClient, (char*)&eType, E_PACKET_TYPE_LENGTH_SIZE, 0);
		if (nRetval == SOCKET_ERROR)
		{
			int nErrNo = WSAGetLastError();
			exit(1);
		}
		
		switch (eType)
		{
		case E_PACKET_TYPE::E_LOGIN_SUCCESS:
		{
			nRetval = recv(m_sockClient, (char*)&m_nMyId, 4, 0);
			if (nRetval == SOCKET_ERROR)
			{
				int nErrNo = WSAGetLastError();
				exit(1);
			}
			m_bLoginSuccess = true;
		}
			break;
		case E_PACKET_TYPE::E_LOGIN_FAIL:
		{
			nRetval = recv(m_sockClient, (char*)&m_nMyId, 4, 0);
			if (nRetval == SOCKET_ERROR)
			{
				int nErrNo = WSAGetLastError();
				exit(1);
			}
			m_bLoginFail = true;
			m_bWorkThread = false;
		}
			break;
		case E_PACKET_TYPE::E_MESSAGE:
		{
			int nDataSize = 0;
			S_STC_MSG_PACKET sSTCMsgPacket = {};
			const WCHAR* wstrDivision = L" : ";
			WCHAR wstrNick[13] = {};
			WCHAR wstrMsg[128] = {};
			WCHAR wstrData[256] = {};
			nRetval = recv(m_sockClient, (char*)&sSTCMsgPacket + E_PACKET_TYPE_LENGTH_SIZE, 4, 0);
			if (nRetval == SOCKET_ERROR)
			{
				int nErrNo = WSAGetLastError();
				exit(1);
			}

			nRetval = recv(m_sockClient, (char*)&sSTCMsgPacket + E_PACKET_TYPE_LENGTH_SIZE + E_DATA_LENGTH_SIZE, sSTCMsgPacket.nDataSize, 0);
			if (nRetval == SOCKET_ERROR)
			{
				int nErrNo = WSAGetLastError();
				exit(1);
			}

			lstrcpynW(wstrNick, sSTCMsgPacket.wstrData, sSTCMsgPacket.nNickLen + 1);
			lstrcpynW(wstrMsg, sSTCMsgPacket.wstrData + sSTCMsgPacket.nNickLen, sSTCMsgPacket.nMgsLen + 1);

			lstrcatW(wstrData, wstrNick);
			lstrcatW(wstrData, wstrDivision);
			lstrcatW(wstrData, wstrMsg);
			lstrcatW(wstrData, L"\r\n");

			int nWstrDatalen = GetWindowTextLength(m_hEditComm);
			SendMessage(m_hEditComm, EM_SETSEL, nWstrDatalen, nWstrDatalen);
			SendMessage(m_hEditComm, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)wstrData);
		}
			break;
		case E_PACKET_TYPE::E_LOGOUT:
		{
			m_bWorkThread = false;
		}
			break;
		case E_PACKET_TYPE::E_VOICE_ACT:
		{

		}
			break;
		case E_PACKET_TYPE::E_VOICE_DEACT:
		{

		}
			break;
		}

	}

}