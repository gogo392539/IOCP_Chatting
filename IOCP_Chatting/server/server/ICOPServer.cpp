#include "stdafx.h"

C_IOCPSERVER::C_IOCPSERVER() :
	m_sockListen(0),
	m_hIOCP(nullptr),
	m_nCountOfThread(0),
	m_mtxData(),
	m_vecWorkerThreads(),
	m_mapClients()
{
	m_vecWorkerThreads.clear();
	m_mapClients.clear();
}

void C_IOCPSERVER::init()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("[ WSAStartup Error - %d ]", __LINE__); 
	}

	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (m_hIOCP == NULL)
	{
		int nErrNo = WSAGetLastError();
		errorMessage("CreateIoCompletionPort Error", nErrNo, __LINE__);
	}

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	m_nCountOfThread = (int)sysInfo.dwNumberOfProcessors * 2;

	makeWorkerThread();		// create workerThread

	m_sockListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN sockAddrListen;
	ZeroMemory(&sockAddrListen, sizeof(SOCKADDR_IN));
	sockAddrListen.sin_family = PF_INET;
	sockAddrListen.sin_addr.s_addr = htonl(INADDR_ANY);
	sockAddrListen.sin_port = htons(E_SERVER_PORT);

	int nRetval = 0;
	nRetval = bind(m_sockListen, (SOCKADDR*)&sockAddrListen, sizeof(SOCKADDR_IN));
	if(nRetval == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		errorMessage("bind Error", nErrNo, __LINE__);
	}

	nRetval = listen(m_sockListen, SOMAXCONN);
	if (nRetval == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		errorMessage("listen Error", nErrNo, __LINE__);
	}

	acceptClient();
}

void C_IOCPSERVER::acceptClient()
{
	DWORD dwFlag;
	int nAcceptCount = 0;

	while (1)
	{
		SOCKADDR_IN sockAddrClient;
		int addrLen = sizeof(SOCKADDR_IN);
		SOCKET sockClient = WSAAccept(m_sockListen, (SOCKADDR*)&sockAddrClient, &addrLen, NULL, NULL);
		if (sockClient == INVALID_SOCKET)
		{
			int nErrNo = WSAGetLastError();
			errorMessage("WSAAccept Error", nErrNo, __LINE__);
		}

		nAcceptCount++;
		printf("user %d accpet \n", nAcceptCount);

		S_HANDLE_DATE* pHandleData = new S_HANDLE_DATE();
		pHandleData->sockClient = sockClient;
		pHandleData->nId = nAcceptCount;
		pHandleData->iter = m_mapClients.find(pHandleData->nId);
		pHandleData->iter = m_mapClients.insert(pHandleData->iter, std::map<int, S_HANDLE_DATE*>::value_type(nAcceptCount, pHandleData));

		CreateIoCompletionPort((HANDLE)sockClient, m_hIOCP, (DWORD)pHandleData, 0);

		S_IO_DATA* pIoData = new S_IO_DATA();
		memset(&pIoData->overlapped, 0, sizeof(OVERLAPPED));
		pIoData->wsaBuf.len = E_PACKET_MAX;
		pIoData->wsaBuf.buf = (char*)&pIoData->packetData;

		dwFlag = 0;

		int nRetval = WSARecv(sockClient, &pIoData->wsaBuf, 1, NULL, &dwFlag, &pIoData->overlapped, NULL);
		if (nRetval == SOCKET_ERROR) {
			int nErrNo = WSAGetLastError();
			if (ERROR_IO_PENDING != nErrNo) {
				errorMessage("Accept::WSARecv", nErrNo, __LINE__);
			}
		}
	}
}

void C_IOCPSERVER::release()
{
	workerThreadJoin();

	closesocket(m_sockListen);
	WSACleanup();
}

void C_IOCPSERVER::makeWorkerThread()
{
	m_vecWorkerThreads.reserve(m_nCountOfThread);
	for (int i = 0; i < m_nCountOfThread; i++)
	{
		m_vecWorkerThreads.push_back(new std::thread(&C_IOCPSERVER::workerThread, this));
	}
}

void C_IOCPSERVER::sendMessage(S_PACKET * pPacket)
{
	S_PACKET sPacket = {};
	memcpy(&sPacket, pPacket, pPacket->nBufLen);
	WSABUF wsaBuf;
	wsaBuf.len = sPacket.nBufLen;
	wsaBuf.buf = (char*)&sPacket;
	DWORD dwFlag = 0;
	DWORD dwSendBytes = 0;
	
	std::map<int, S_HANDLE_DATE*>::iterator iter = m_mapClients.begin();
	while (iter != m_mapClients.end())
	{
		int nRetval = WSASend(iter->second->sockClient, &wsaBuf, 1, &dwSendBytes, dwFlag, NULL, NULL);
		if (nRetval == SOCKET_ERROR) {
			int err_no = WSAGetLastError();
			if (ERROR_IO_PENDING != err_no) {
				errorMessage("SendPacket::WSASend", err_no, __LINE__);
			}
		}
	}

	
}

void C_IOCPSERVER::workerThreadJoin()
{
	std::vector<std::thread*>::iterator iter = m_vecWorkerThreads.begin();
	while (iter != m_vecWorkerThreads.end())
	{
		(*iter)->join();
		delete (*iter);
	}
}

void C_IOCPSERVER::workerThread()
{
	HANDLE hCompletionPort = (HANDLE)m_hIOCP;
	S_HANDLE_DATE* pHandleData = nullptr;
	S_IO_DATA* pIoData = nullptr;
	DWORD dwBytesTransferred = 0;
	DWORD dwFlag = 0;
	DWORD dwSendBytes = 0;

	printf("worker Thread\n");

	while (1)
	{
		BOOL bResult =  GetQueuedCompletionStatus(hCompletionPort, &dwBytesTransferred, (PULONG_PTR)&pHandleData,
			(LPOVERLAPPED*)&pIoData, INFINITE);
		if (dwBytesTransferred == 0)
		{
			printf("종료 \n");

			dwFlag = 0;
			dwSendBytes = 0;

			pIoData->packetData.eType = E_PACKET_TYPE::E_LOGOUT;
			pIoData->packetData.nId = pHandleData->nId;
			pIoData->packetData.nBufLen = 12;
			pIoData->wsaBuf.len = pIoData->packetData.nBufLen;
			pIoData->wsaBuf.buf = (char*)&pIoData->packetData;

			std::map<int, S_HANDLE_DATE*>::iterator iter = m_mapClients.begin();
			while (iter != m_mapClients.end())
			{
				if (iter != pHandleData->iter)
				{
					int nRetval = WSASend(iter->second->sockClient, &pIoData->wsaBuf, 1, &dwSendBytes, dwFlag, NULL, NULL);
					if (nRetval == SOCKET_ERROR) {
						int err_no = WSAGetLastError();
						if (ERROR_IO_PENDING != err_no) {
							errorMessage("SendPacket::WSASend", err_no, __LINE__);
						}
					}
				}

				iter++;
			}

			m_mapClients.erase(pHandleData->iter);
			closesocket(pHandleData->sockClient);
			delete pHandleData;
			pHandleData = nullptr;
			delete pIoData;
			pIoData = nullptr;
			continue;
		}
		else
		{
			if (pIoData->packetData.eType == E_PACKET_TYPE::E_LOGIN)
			{
				dwFlag = 0;
				dwSendBytes = 0;
				//pIoData->packetData.nId = pHandleData->nId;
				int nRetval = WSASend(pHandleData->sockClient, &pIoData->wsaBuf, 1, &dwSendBytes, dwFlag, NULL, NULL);
				if (nRetval == SOCKET_ERROR) {
					int err_no = WSAGetLastError();
					if (ERROR_IO_PENDING != err_no) {
						errorMessage("SendPacket::WSASend", err_no, __LINE__);
					}
				}
			}
			else if(pIoData->packetData.eType == E_PACKET_TYPE::E_LOGOUT)
			{
				dwFlag = 0;
				dwSendBytes = 0;

				std::map<int, S_HANDLE_DATE*>::iterator iter = m_mapClients.begin();
				while (iter != m_mapClients.end())
				{
					if (iter != pHandleData->iter)
					{
						int nRetval = WSASend(iter->second->sockClient, &pIoData->wsaBuf, 1, &dwSendBytes, dwFlag, NULL, NULL);
						if (nRetval == SOCKET_ERROR) {
							int err_no = WSAGetLastError();
							if (ERROR_IO_PENDING != err_no) {
								errorMessage("SendPacket::WSASend", err_no, __LINE__);
							}
						}
					}

					iter++;
				}

				m_mapClients.erase(pHandleData->iter);
				closesocket(pHandleData->sockClient);
				delete pHandleData;
				pHandleData = nullptr;
				delete pIoData;
				pIoData = nullptr;

				continue;
			}
			else if (pIoData->packetData.eType == E_PACKET_TYPE::E_DATA)
			{
				dwFlag = 0;
				dwSendBytes = 0;

				std::map<int, S_HANDLE_DATE*>::iterator iter = m_mapClients.begin();
				while (iter != m_mapClients.end())
				{
					if (iter != pHandleData->iter)
					{
						int nRetval = WSASend(iter->second->sockClient, &pIoData->wsaBuf, 1, &dwSendBytes, dwFlag, NULL, NULL);
						if (nRetval == SOCKET_ERROR) {
							int err_no = WSAGetLastError();
							if (ERROR_IO_PENDING != err_no) {
								errorMessage("SendPacket::WSASend", err_no, __LINE__);
							}
						}
					}
					
					iter++;
				}
			}
		}

		memset(&pIoData->overlapped, 0, sizeof(OVERLAPPED));
		memset(&pIoData->packetData, 0, E_PACKET_MAX);
		pIoData->wsaBuf.len = E_PACKET_MAX;
		pIoData->wsaBuf.buf = (char*)&pIoData->packetData;

		dwFlag = 0;
		
		int nRetval = WSARecv(pHandleData->sockClient, &pIoData->wsaBuf, 1, NULL, &dwFlag, &pIoData->overlapped, NULL);
		if (nRetval == SOCKET_ERROR)
		{
			int nErrNo = WSAGetLastError();
			if (nErrNo != ERROR_IO_PENDING) {
				errorMessage("WorkerThreadStart::WSARecv", nErrNo, __LINE__);
			}
		}
	}
}

void C_IOCPSERVER::errorMessage(const char *msg, int err_no, int line)
{
	WCHAR *lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[ %s - %d ]", msg, line);
	wprintf(L"에러 %s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
	exit(-1);
}

