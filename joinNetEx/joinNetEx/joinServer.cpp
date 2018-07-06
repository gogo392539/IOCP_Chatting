#include "stdafx.h"

C_JOIN_SERVER::C_JOIN_SERVER() :
	m_sockListen(0),
	m_hIOCP(nullptr),
	m_nCountOfThread(0),
	m_mtxData(),
	m_vecWorkerThreads(),
	m_mapJoinClients(),
	m_hWnd(NULL),
	m_nSerialId(0),
	m_pThreadAccept(nullptr)
	//m_cDbServer()
{
	m_vecWorkerThreads.clear();
	m_mapJoinClients.clear();
}

void C_JOIN_SERVER::init(HWND hWnd)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("[ WSAStartup Error - %d ]", __LINE__);
	}

//	m_cDbServer.init();
	m_hWnd = hWnd;

	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (m_hIOCP == NULL)
	{
		int nErrNo = WSAGetLastError();
		//errorMessage("CreateIoCompletionPort Error", nErrNo, __LINE__);
		WCHAR strErr[128] = L"";
		int len = swprintf_s(strErr, 128, L"Nick Name 중복 확인 하세요 [%d - %d]", nErrNo, __LINE__);
		MessageBox(m_hWnd, strErr, L"오류", MB_OK);
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
	sockAddrListen.sin_port = htons(E_JOIN_SERVER_PORT);

	int nRetval = 0;
	nRetval = bind(m_sockListen, (SOCKADDR*)&sockAddrListen, sizeof(SOCKADDR_IN));
	if (nRetval == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		//errorMessage("bind Error", nErrNo, __LINE__);
		WCHAR strErr[128] = L"";
		int len = swprintf_s(strErr, 128, L"bind Error [%d - %d]", nErrNo, __LINE__);
		MessageBox(m_hWnd, strErr, L"오류", MB_OK);
	}

	nRetval = listen(m_sockListen, SOMAXCONN);
	if (nRetval == SOCKET_ERROR)
	{
		int nErrNo = WSAGetLastError();
		//	errorMessage("listen Error", nErrNo, __LINE__);
		WCHAR strErr[128] = L"";
		int len = swprintf_s(strErr, 128, L"listen Error [%d - %d]", nErrNo, __LINE__);
		MessageBox(m_hWnd, strErr, L"오류", MB_OK);
	}



	m_pThreadAccept = new std::thread(&C_JOIN_SERVER::acceptJoinClient, this);
}

void C_JOIN_SERVER::acceptJoinClient()
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
			//			errorMessage("WSAAccept Error", nErrNo, __LINE__);
			WCHAR strErr[128] = L"";
			int len = swprintf_s(strErr, 128, L"WSAAccept Error [%d - %d]", nErrNo, __LINE__);
			MessageBox(m_hWnd, strErr, L"오류", MB_OK);
		}

		S_JOIN_HANDLE_DATE* pJoinHandleData = new S_JOIN_HANDLE_DATE();
		pJoinHandleData->sockClient = sockClient;
		pJoinHandleData->nId = -1;

		CreateIoCompletionPort((HANDLE)sockClient, m_hIOCP, (DWORD)pJoinHandleData, 0);

		S_JOIN_IO_DATA* pJoinIoData = new S_JOIN_IO_DATA();
		memset(&pJoinIoData->overlapped, 0, sizeof(OVERLAPPED));
		pJoinIoData->wsaBuf.len = E_PACKET_TYPE_LENGTH_SIZE;
		pJoinIoData->wsaBuf.buf = (char*)&pJoinIoData->eType;

		dwFlag = 0;

		int nRetval = WSARecv(sockClient, &pJoinIoData->wsaBuf, 1, NULL, &dwFlag, &pJoinIoData->overlapped, NULL);
		if (nRetval == SOCKET_ERROR) {
			int nErrNo = WSAGetLastError();
			if (ERROR_IO_PENDING != nErrNo) {
				//				errorMessage("Accept::WSARecv", nErrNo, __LINE__);
				WCHAR strErr[128] = L"";
				int len = swprintf_s(strErr, 128, L"Accept::WSARecv Error [%d - %d]", nErrNo, __LINE__);
				MessageBox(m_hWnd, strErr, L"오류", MB_OK);
			}
		}
	}
}

void C_JOIN_SERVER::release()
{
	threadJoin();
//	m_cDbServer.release();
	closesocket(m_sockListen);
	WSACleanup();
}

void C_JOIN_SERVER::makeWorkerThread()
{
	m_vecWorkerThreads.reserve(m_nCountOfThread);
	for (int i = 0; i < m_nCountOfThread; i++)
	{
		m_vecWorkerThreads.push_back(new std::thread(&C_JOIN_SERVER::workerThread, this));
	}
}

void C_JOIN_SERVER::threadJoin()
{
	m_pThreadAccept->join();

	std::vector<std::thread*>::iterator iter = m_vecWorkerThreads.begin();
	while (iter != m_vecWorkerThreads.end())
	{
		(*iter)->join();
		delete (*iter);
	}

	delete m_pThreadAccept;
	m_pThreadAccept = nullptr;
}

void C_JOIN_SERVER::workerThread()
{
	HANDLE hCompletionPort = (HANDLE)m_hIOCP;
	S_JOIN_HANDLE_DATE* pJoinHandleData = nullptr;
	S_JOIN_IO_DATA* pJoinIoData = nullptr;
	DWORD dwBytesTransferred = 0;
	DWORD dwFlag = 0;
	DWORD dwBytes = 0;
	int nRetval = 0;

	while (1)
	{
		BOOL bResult = GetQueuedCompletionStatus(hCompletionPort, &dwBytesTransferred, (PULONG_PTR)&pJoinHandleData,
			(LPOVERLAPPED*)&pJoinIoData, INFINITE);
		if (dwBytesTransferred == 0)
		{
			if (pJoinHandleData->nId != -1)
			{
				m_mtxData.lock();
				m_mapJoinClients.erase(pJoinHandleData->iter);
				m_mtxData.unlock();
			}
			closesocket(pJoinHandleData->sockClient);
			delete pJoinHandleData;
			pJoinHandleData = nullptr;
			delete pJoinIoData;
			pJoinIoData = nullptr;
			continue;
		}
		else
		{
			switch (pJoinIoData->eType)
			{
			case E_JOIN_PACKET_TYPE::E_ID_CHECK:
			{

			}
				break;
			case E_JOIN_PACKET_TYPE::E_NICK_CHECK:
			{

			}
				break;
			case E_JOIN_PACKET_TYPE::E_JOIN:
			{

			}
				break;
			}
		}

		memset(&pJoinIoData->overlapped, 0, sizeof(OVERLAPPED));
		pJoinIoData->wsaBuf.len = E_PACKET_TYPE_LENGTH_SIZE;
		pJoinIoData->wsaBuf.buf = (char*)&pJoinIoData->eType;

		dwFlag = 0;

		int nRetval = WSARecv(pJoinHandleData->sockClient, &pJoinIoData->wsaBuf, 1, NULL, &dwFlag, &pJoinIoData->overlapped, NULL);
		if (nRetval == SOCKET_ERROR)
		{
			int nErrNo = WSAGetLastError();
			if (nErrNo != ERROR_IO_PENDING) {
				//				errorMessage("WorkerThreadStart::WSARecv", nErrNo, __LINE__);
				WCHAR strErr[128] = L"";
				int len = swprintf_s(strErr, 128, L"WorkerThreadStart::WSARecv Error [%d - %d]", nErrNo, __LINE__);
				MessageBox(m_hWnd, strErr, L"오류", MB_OK);
			}
		}
	}
}

