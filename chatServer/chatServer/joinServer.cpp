#include "stdafx.h"

C_JOIN_SERVER::C_JOIN_SERVER() :
	m_sockListen(0),
	m_hIOCP(nullptr),
	m_nCountOfThread(0),
	m_mtxData(),
	m_vecWorkerThreads(),
	m_mapJoinClients(),
	m_hWnd(NULL),
	m_nMaxSerialId(0),
	m_pThreadAccept(nullptr),
	m_cDbServer(),
	m_nClientCount(0),
	m_bAcceptThreadSet(true)
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

	m_hWnd = hWnd;
	m_cDbServer.init();
	m_cDbServer.selectMaxSerialId(&m_nMaxSerialId);
	m_nMaxSerialId++;

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

	SOCKADDR_IN sockAddrListen = {};
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

	ULONG isNonBlocking = 1;
	ioctlsocket(m_sockListen,        //Non-Blocking으로 변경할 소켓
		FIONBIO,       //변경할 소켓의 입출력 모드
		&isNonBlocking //넘기는 인자, 여기서는 nonblocking설정 값
	);

	while (m_bAcceptThreadSet)
	{
		SOCKADDR_IN sockAddrClient;
		int addrLen = sizeof(SOCKADDR_IN);
		SOCKET sockClient = WSAAccept(m_sockListen, (SOCKADDR*)&sockAddrClient, &addrLen, NULL, NULL);
		if (sockClient == INVALID_SOCKET)
		{
			int nErrNo = WSAGetLastError();
			if (nErrNo != WSAEWOULDBLOCK) {
				//			errorMessage("WSAAccept Error", nErrNo, __LINE__);
				WCHAR strErr[128] = L"";
				int len = swprintf_s(strErr, 128, L"WSAAccept Error [%d - %d]", nErrNo, __LINE__);
				MessageBox(m_hWnd, strErr, L"오류", MB_OK);
			}
			else
			{
				continue;
			}
		}
		m_nClientCount++;
		S_JOIN_HANDLE_DATE* pJoinHandleData = new S_JOIN_HANDLE_DATE();
		pJoinHandleData->sockClient = sockClient;
		pJoinHandleData->nId = m_nClientCount;
		pJoinHandleData->iter = m_mapJoinClients.find(pJoinHandleData->nId);
		m_mtxData.lock();
		pJoinHandleData->iter = m_mapJoinClients.insert(pJoinHandleData->iter,
			std::map<int, S_JOIN_HANDLE_DATE*>::value_type(m_nClientCount, pJoinHandleData));
		m_mtxData.unlock();

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
	m_cDbServer.release();
	closesocket(m_sockListen);
	WSACleanup();
}

void C_JOIN_SERVER::serverEnd()
{
	S_JOIN_IO_DATA* pJoinIoData = new S_JOIN_IO_DATA();
	memset(&pJoinIoData->overlapped, 0, sizeof(OVERLAPPED));
	pJoinIoData->eType = E_JOIN_PACKET_TYPE::E_SERVER_END;
	DWORD dwBytesTransferred = 4;
	for (int i = 0; i < m_nCountOfThread; i++)
	{
		PostQueuedCompletionStatus((HANDLE)m_hIOCP, dwBytesTransferred, NULL, (LPOVERLAPPED)pJoinIoData);
	}

	m_bAcceptThreadSet = false;
}

void C_JOIN_SERVER::closeClient()
{
	std::map<int, S_JOIN_HANDLE_DATE*>::iterator iter = m_mapJoinClients.begin();
	while (iter != m_mapJoinClients.end())
	{
		closesocket(iter->second->sockClient);
		iter = m_mapJoinClients.erase(iter->second->iter);
	}
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
		iter++;
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
	bool bWorkerThreadSet = true;

	while (bWorkerThreadSet)
	{
		BOOL bResult = GetQueuedCompletionStatus(hCompletionPort, &dwBytesTransferred, (PULONG_PTR)&pJoinHandleData,
			(LPOVERLAPPED*)&pJoinIoData, INFINITE);
		if (dwBytesTransferred == 0)
		{
			m_mtxData.lock();
			m_mapJoinClients.erase(pJoinHandleData->iter);
			m_mtxData.unlock();
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
			case E_JOIN_PACKET_TYPE::E_SERVER_END:
			{
				bWorkerThreadSet = false;
				continue;
			}
				break;
			case E_JOIN_PACKET_TYPE::E_ID_CHECK:
			{
				S_CTS_ID_CHECK_PACKET sCTSIdCheckPacket = {};
				WSABUF wsaBuf = {};
				wsaBuf.len = E_DATA_LENGTH_SIZE;
				wsaBuf.buf = (char*)&sCTSIdCheckPacket + E_PACKET_TYPE_LENGTH_SIZE;

				dwBytes = 0;
				dwFlag = 0;

				nRetval = WSARecv(pJoinHandleData->sockClient, &wsaBuf, 1, &dwBytes, &dwFlag, NULL, NULL);
				if (nRetval == SOCKET_ERROR) {
					int nErrNo = WSAGetLastError();
					if (ERROR_IO_PENDING != nErrNo) {
						//errorMessage("Accept::WSARecv", nErrNo, __LINE__);
						WCHAR strErr[128] = L"";
						int len = swprintf_s(strErr, 128, L"E_LOGIN_CALL::WSARecv Error [%d - %d]", nErrNo, __LINE__);
						MessageBox(m_hWnd, strErr, L"오류", MB_OK);
					}
				}

				int nUserIdLen = 0;
				WCHAR wstrId[E_MAX_ID_LENGTH] = {};

				wsaBuf.len = sCTSIdCheckPacket.nIdLen;
				wsaBuf.buf = (char*)&sCTSIdCheckPacket + E_PACKET_TYPE_LENGTH_SIZE + E_DATA_LENGTH_SIZE;

				dwBytes = 0;
				dwFlag = 0;

				nRetval = WSARecv(pJoinHandleData->sockClient, &wsaBuf, 1, &dwBytes, &dwFlag, NULL, NULL);
				if (nRetval == SOCKET_ERROR) {
					int nErrNo = WSAGetLastError();
					if (ERROR_IO_PENDING != nErrNo) {
						//errorMessage("Accept::WSARecv", nErrNo, __LINE__);
						WCHAR strErr[128] = L"";
						int len = swprintf_s(strErr, 128, L"E_LOGIN_CALL::WSARecv Error [%d - %d]", nErrNo, __LINE__);
						MessageBox(m_hWnd, strErr, L"오류", MB_OK);
					}
				}

				nUserIdLen = lstrlenW(sCTSIdCheckPacket.wstrId);
				lstrcpyW(wstrId, sCTSIdCheckPacket.wstrId);

				E_JOIN_PACKET_TYPE eType = E_JOIN_PACKET_TYPE::E_NONE;
				if (m_cDbServer.selectUserId(wstrId))
				{
					eType = E_JOIN_PACKET_TYPE::E_ID_CHECK_SUCCESS;
				}
				else 
				{
					eType = E_JOIN_PACKET_TYPE::E_ID_CHECK_FAIL;
				}

				S_STC_ID_CHECK_PACKET sSTCIdCheckPacket = {};
				sSTCIdCheckPacket.eType = eType;

				wsaBuf.len = E_PACKET_TYPE_LENGTH_SIZE;
				wsaBuf.buf = (char*)&sSTCIdCheckPacket;

				dwBytes = 0;//
				dwFlag = 0;

				nRetval = WSASend(pJoinHandleData->sockClient, &wsaBuf, 1, &dwBytes, dwFlag, NULL, NULL);
				if (nRetval == SOCKET_ERROR) {
					int nErrNo = WSAGetLastError();
					if (ERROR_IO_PENDING != nErrNo) {
						//errorMessage("Accept::WSARecv", nErrNo, __LINE__);
						WCHAR strErr[128] = L"";
						int len = swprintf_s(strErr, 128, L"E_LOGIN_CALL::WSASend Error [%d - %d]", nErrNo, __LINE__);
						MessageBox(m_hWnd, strErr, L"오류", MB_OK);
					}
				}

			}
				break;
			case E_JOIN_PACKET_TYPE::E_NICK_CHECK:
			{
				S_CTS_NICK_CHECK_PACKET sCTSNickCheckPacket = {};
				WSABUF wsaBuf = {};
				wsaBuf.len = E_DATA_LENGTH_SIZE;
				wsaBuf.buf = (char*)&sCTSNickCheckPacket + E_PACKET_TYPE_LENGTH_SIZE;

				dwBytes = 0;
				dwFlag = 0;

				nRetval = WSARecv(pJoinHandleData->sockClient, &wsaBuf, 1, &dwBytes, &dwFlag, NULL, NULL);
				if (nRetval == SOCKET_ERROR) {
					int nErrNo = WSAGetLastError();
					if (ERROR_IO_PENDING != nErrNo) {
						//errorMessage("Accept::WSARecv", nErrNo, __LINE__);
						WCHAR strErr[128] = L"";
						int len = swprintf_s(strErr, 128, L"E_LOGIN_CALL::WSARecv Error [%d - %d]", nErrNo, __LINE__);
						MessageBox(m_hWnd, strErr, L"오류", MB_OK);
					}
				}

				int nUserNickLen = 0;
				WCHAR wstrNick[E_MAX_ID_LENGTH] = {};

				wsaBuf.len = sCTSNickCheckPacket.nNickLen;
				wsaBuf.buf = (char*)&sCTSNickCheckPacket + E_PACKET_TYPE_LENGTH_SIZE + E_DATA_LENGTH_SIZE;

				dwBytes = 0;
				dwFlag = 0;

				nRetval = WSARecv(pJoinHandleData->sockClient, &wsaBuf, 1, &dwBytes, &dwFlag, NULL, NULL);
				if (nRetval == SOCKET_ERROR) {
					int nErrNo = WSAGetLastError();
					if (ERROR_IO_PENDING != nErrNo) {
						//errorMessage("Accept::WSARecv", nErrNo, __LINE__);
						WCHAR strErr[128] = L"";
						int len = swprintf_s(strErr, 128, L"E_LOGIN_CALL::WSARecv Error [%d - %d]", nErrNo, __LINE__);
						MessageBox(m_hWnd, strErr, L"오류", MB_OK);
					}
				}

				nUserNickLen = lstrlenW(sCTSNickCheckPacket.wstrNick);
				lstrcpyW(wstrNick, sCTSNickCheckPacket.wstrNick);

				E_JOIN_PACKET_TYPE eType = E_JOIN_PACKET_TYPE::E_NONE;
				if (m_cDbServer.selectUserNickName(wstrNick))
				{
					eType = E_JOIN_PACKET_TYPE::E_NICK_CHECK_SUCCESS;
				}
				else
				{
					eType = E_JOIN_PACKET_TYPE::E_NICK_CHECK_FAIL;
				}

				S_STC_ID_CHECK_PACKET sSTCIdCheckPacket = {};
				sSTCIdCheckPacket.eType = eType;

				wsaBuf.len = E_PACKET_TYPE_LENGTH_SIZE;
				wsaBuf.buf = (char*)&sSTCIdCheckPacket;

				dwBytes = 0;//
				dwFlag = 0;

				nRetval = WSASend(pJoinHandleData->sockClient, &wsaBuf, 1, &dwBytes, dwFlag, NULL, NULL);
				if (nRetval == SOCKET_ERROR) {
					int nErrNo = WSAGetLastError();
					if (ERROR_IO_PENDING != nErrNo) {
						//errorMessage("Accept::WSARecv", nErrNo, __LINE__);
						WCHAR strErr[128] = L"";
						int len = swprintf_s(strErr, 128, L"E_LOGIN_CALL::WSASend Error [%d - %d]", nErrNo, __LINE__);
						MessageBox(m_hWnd, strErr, L"오류", MB_OK);
					}
				}
			}
				break;
			case E_JOIN_PACKET_TYPE::E_JOIN:
			{
				S_CTS_JOIN_PACKET sCTSJoinPacket = {};
				WSABUF wsaBuf = {};
				wsaBuf.len = E_DATA_LENGTH_SIZE;
				wsaBuf.buf = (char*)&sCTSJoinPacket + E_PACKET_TYPE_LENGTH_SIZE;

				dwBytes = 0;
				dwFlag = 0;

				nRetval = WSARecv(pJoinHandleData->sockClient, &wsaBuf, 1, &dwBytes, &dwFlag, NULL, NULL);
				if (nRetval == SOCKET_ERROR) {
					int nErrNo = WSAGetLastError();
					if (ERROR_IO_PENDING != nErrNo) {
						//errorMessage("Accept::WSARecv", nErrNo, __LINE__);
						WCHAR strErr[128] = L"";
						int len = swprintf_s(strErr, 128, L"E_LOGIN_CALL::WSARecv Error [%d - %d]", nErrNo, __LINE__);
						MessageBox(m_hWnd, strErr, L"오류", MB_OK);
					}
				}

				int nIdLen = 0;
				int nNickLen = 0;
				int nPwLen = 0;
				WCHAR wstrId[13] = {};
				WCHAR wstrNick[13] = {};
				WCHAR wstrPw[13] = {};

				wsaBuf.len = sCTSJoinPacket.nDataSize;
				wsaBuf.buf = (char*)&sCTSJoinPacket + E_PACKET_TYPE_LENGTH_SIZE + E_DATA_LENGTH_SIZE;

				nRetval = WSARecv(pJoinHandleData->sockClient, &wsaBuf, 1, &dwBytes, &dwFlag, NULL, NULL);
				if (nRetval == SOCKET_ERROR) {
					int nErrNo = WSAGetLastError();
					if (ERROR_IO_PENDING != nErrNo) {
						//errorMessage("Accept::WSARecv", nErrNo, __LINE__);
						WCHAR strErr[128] = L"";
						int len = swprintf_s(strErr, 128, L"E_LOGIN_CALL::WSARecv Error [%d - %d]", nErrNo, __LINE__);
						MessageBox(m_hWnd, strErr, L"오류", MB_OK);
					}
				}

				nIdLen = sCTSJoinPacket.nIdLen;
				nNickLen = sCTSJoinPacket.nNickLen;
				nPwLen = sCTSJoinPacket.nPwLen;
				lstrcpynW(wstrId, sCTSJoinPacket.wstrData, nIdLen + 1);
				lstrcpynW(wstrNick, sCTSJoinPacket.wstrData + nIdLen, nNickLen + 1);
				lstrcpynW(wstrPw, sCTSJoinPacket.wstrData + nIdLen + nNickLen, nPwLen + 1);

				m_cDbServer.insertUserInfo(m_nMaxSerialId, wstrId, wstrNick, wstrPw);
				m_nMaxSerialId++;

				S_STC_JOIN_PACKET sSTCJoinPacket = {};
				sSTCJoinPacket.eType = E_JOIN_PACKET_TYPE::E_JOIN;

				wsaBuf.len = E_PACKET_TYPE_LENGTH_SIZE;
				wsaBuf.buf = (char*)&sSTCJoinPacket;

				dwBytes = 0;//
				dwFlag = 0;

				nRetval = WSASend(pJoinHandleData->sockClient, &wsaBuf, 1, &dwBytes, dwFlag, NULL, NULL);
				if (nRetval == SOCKET_ERROR) {
					int nErrNo = WSAGetLastError();
					if (ERROR_IO_PENDING != nErrNo) {
						//errorMessage("Accept::WSARecv", nErrNo, __LINE__);
						WCHAR strErr[128] = L"";
						int len = swprintf_s(strErr, 128, L"E_LOGIN_CALL::WSASend Error [%d - %d]", nErrNo, __LINE__);
						MessageBox(m_hWnd, strErr, L"오류", MB_OK);
					}
				}
			}
				break;
			case E_JOIN_PACKET_TYPE::E_EXIT:
			{
				S_EXIT_MESSATE_PACKET sExitMsgPacket = {};
				WSABUF wsaBuf = {};
				sExitMsgPacket.eType = E_JOIN_PACKET_TYPE::E_EXIT;
				wsaBuf.len = E_PACKET_TYPE_LENGTH_SIZE;
				wsaBuf.buf = (char*)&sExitMsgPacket;

				dwBytes = 0;//
				dwFlag = 0;

				nRetval = WSASend(pJoinHandleData->sockClient, &wsaBuf, 1, &dwBytes, dwFlag, NULL, NULL);
				if (nRetval == SOCKET_ERROR) {
					int nErrNo = WSAGetLastError();
					if (ERROR_IO_PENDING != nErrNo) {
						//errorMessage("Accept::WSARecv", nErrNo, __LINE__);
						WCHAR strErr[128] = L"";
						int len = swprintf_s(strErr, 128, L"E_LOGIN_CALL::WSASend Error [%d - %d]", nErrNo, __LINE__);
						MessageBox(m_hWnd, strErr, L"오류", MB_OK);
					}
				}

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

