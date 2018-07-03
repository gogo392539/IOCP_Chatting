#include "stdafx.h"

C_MAINSERVER::C_MAINSERVER() :
	m_sockListen(0),
	m_hIOCP(nullptr),
	m_nCountOfThread(0),
	m_mtxData(),
	m_vecWorkerThreads(),
	m_mapClients(),
	m_mapAccessClientInfo(),
	m_hWnd(NULL),
	m_nLoginNum(0),
	m_pThreadAccept(nullptr)
{
	m_vecWorkerThreads.clear();
	m_mapClients.clear();
	m_mapAccessClientInfo.clear();
}

void C_MAINSERVER::init(HWND hWnd)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("[ WSAStartup Error - %d ]", __LINE__);
	}

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
	sockAddrListen.sin_port = htons(E_SERVER_PORT);

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

	m_pThreadAccept = new std::thread(&C_MAINSERVER::acceptClient, this);
}

void C_MAINSERVER::acceptClient()
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

		S_HANDLE_DATE* pHandleData = new S_HANDLE_DATE();
		pHandleData->sockClient = sockClient;
		pHandleData->nId = -1;

		CreateIoCompletionPort((HANDLE)sockClient, m_hIOCP, (DWORD)pHandleData, 0);

		S_IO_DATA* pIoData = new S_IO_DATA();
		memset(&pIoData->overlapped, 0, sizeof(OVERLAPPED));
		pIoData->wsaBuf.len = E_PACKET_TYPE_LENGTH;
		pIoData->wsaBuf.buf = (char*)&pIoData->eType;

		dwFlag = 0;

		int nRetval = WSARecv(sockClient, &pIoData->wsaBuf, 1, NULL, &dwFlag, &pIoData->overlapped, NULL);
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

void C_MAINSERVER::release()
{
	threadJoin();

	closesocket(m_sockListen);
	WSACleanup();
}

void C_MAINSERVER::makeWorkerThread()
{
	m_vecWorkerThreads.reserve(m_nCountOfThread);
	for (int i = 0; i < m_nCountOfThread; i++)
	{
		m_vecWorkerThreads.push_back(new std::thread(&C_MAINSERVER::workerThread, this));
	}
}

void C_MAINSERVER::threadJoin()
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

void C_MAINSERVER::workerThread()
{
	HANDLE hCompletionPort = (HANDLE)m_hIOCP;
	S_HANDLE_DATE* pHandleData = nullptr;
	S_IO_DATA* pIoData = nullptr;
	DWORD dwBytesTransferred = 0;
	DWORD dwFlag = 0;
	DWORD dwBytes = 0;
	int nRetval = 0;

	while (1)
	{
		BOOL bResult = GetQueuedCompletionStatus(hCompletionPort, &dwBytesTransferred, (PULONG_PTR)&pHandleData,
			(LPOVERLAPPED*)&pIoData, INFINITE);
		if (dwBytesTransferred == 0)
		{
			m_mtxData.lock();
			m_mapClients.erase(pHandleData->iter);
			m_mtxData.unlock();
			closesocket(pHandleData->sockClient);
			delete pHandleData;
			pHandleData = nullptr;
			delete pIoData;
			pIoData = nullptr;
			continue;
		}
		else
		{
			switch (pIoData->eType)
			{
			case E_PACKET_TYPE::E_LOGIN_CALL:
			{
				S_CTS_LOGIN_PACKET sCTSPacket = {};
				WSABUF wsaBuf;
				wsaBuf.len = E_DATA_LENGTH;
				wsaBuf.buf = (char*)&sCTSPacket;

				dwBytes = 0;//
				dwFlag = 0;

				nRetval = WSARecv(pHandleData->sockClient, &wsaBuf, 1, &dwBytes, &dwFlag, NULL, NULL);
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
				int nPwLen = 0;
				WCHAR strId[E_MAX_ID_LENGTH] = {};
				WCHAR strPw[E_MAX_PW_LENGTH] = {};

				wsaBuf.len = sCTSPacket.nDataSize;
				wsaBuf.buf = (char*)&sCTSPacket + E_DATA_LENGTH;

				dwBytes = 0;//
				dwFlag = 0;

				nRetval = WSARecv(pHandleData->sockClient, &wsaBuf, 1, &dwBytes, &dwFlag, NULL, NULL);
				if (nRetval == SOCKET_ERROR) {
					int nErrNo = WSAGetLastError();
					if (ERROR_IO_PENDING != nErrNo) {
						//errorMessage("Accept::WSARecv", nErrNo, __LINE__);
						WCHAR strErr[128] = L"";
						int len = swprintf_s(strErr, 128, L"E_LOGIN_CALL::WSARecv Error [%d - %d]", nErrNo, __LINE__);
						MessageBox(m_hWnd, strErr, L"오류", MB_OK);
					}
				}

				nIdLen = sCTSPacket.nIdLen;
				nPwLen = sCTSPacket.nPwLen;
				lstrcpynW(strId, sCTSPacket.strData, nIdLen + 1);
				lstrcpynW(strPw, sCTSPacket.strData + nIdLen, nPwLen + 1);

				//DB

				m_nLoginNum++;
				pHandleData->nId = m_nLoginNum;
				pHandleData->iter = m_mapClients.find(pHandleData->nId);
				m_mtxData.lock();
				pHandleData->iter = m_mapClients.insert(pHandleData->iter, std::map<int, S_HANDLE_DATE*>::value_type(m_nLoginNum, pHandleData));
				m_mtxData.unlock();

				m_mapAccessClientInfo.insert(std::map<int, std::wstring>::value_type(pHandleData->nId, strId));

				S_STC_LOGIN_PACKET sSTCPacket = {};
				sSTCPacket.eType = E_PACKET_TYPE::E_LOGIN_SUCCESS;
				sSTCPacket.nSerialId = pHandleData->nId;

				wsaBuf.len = 8;
				wsaBuf.buf = (char*)&sSTCPacket;

				dwBytes = 0;//
				dwFlag = 0;

				nRetval = WSASend(pHandleData->sockClient, &wsaBuf, 1, &dwBytes, dwFlag, NULL, NULL);
				if (nRetval == SOCKET_ERROR) {
					int nErrNo = WSAGetLastError();
					if (ERROR_IO_PENDING != nErrNo) {
						//errorMessage("Accept::WSARecv", nErrNo, __LINE__);
						WCHAR strErr[128] = L"";
						int len = swprintf_s(strErr, 128, L"E_LOGIN_CALL::WSASend Error [%d - %d]", nErrNo, __LINE__);
						MessageBox(m_hWnd, strErr, L"오류", MB_OK);
					}
				}

				//다른 클라 전송 추가
			}
				break;
			case E_PACKET_TYPE::E_LOGOUT:
			{
				int nLogoutId = 0;
				WSABUF wsaBuf;
				wsaBuf.len = 4;
				wsaBuf.buf = (char*)&nLogoutId;


				nRetval = WSARecv(pHandleData->sockClient, &wsaBuf, 1, &dwBytes, &dwFlag, NULL, NULL);
				if (nRetval == SOCKET_ERROR) {
					int nErrNo = WSAGetLastError();
					if (ERROR_IO_PENDING != nErrNo) {
						//errorMessage("Accept::WSARecv", nErrNo, __LINE__);
						WCHAR strErr[128] = L"";
						int len = swprintf_s(strErr, 128, L"E_LOGIN_CALL::WSARecv Error [%d - %d]", nErrNo, __LINE__);
						MessageBox(m_hWnd, strErr, L"오류", MB_OK);
					}
				}


				E_PACKET_TYPE eType = E_PACKET_TYPE::E_LOGOUT;
				wsaBuf.len = E_PACKET_TYPE_LENGTH;
				wsaBuf.buf = (char*)&eType;

				nRetval = WSASend(pHandleData->sockClient, &wsaBuf, 1, &dwBytes, dwFlag, NULL, NULL);
				if (nRetval == SOCKET_ERROR) {
					int nErrNo = WSAGetLastError();
					if (ERROR_IO_PENDING != nErrNo) {
						//errorMessage("Accept::WSARecv", nErrNo, __LINE__);
						WCHAR strErr[128] = L"";
						int len = swprintf_s(strErr, 128, L"E_LOGIN_CALL::WSASend Error [%d - %d]", nErrNo, __LINE__);
						MessageBox(m_hWnd, strErr, L"오류", MB_OK);
					}
				}

				
				//다른 클라 전송 추가
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
			case E_PACKET_TYPE::E_MESSAGE:
			{

			}
				break;
			}
		}

		memset(&pIoData->overlapped, 0, sizeof(OVERLAPPED));
		pIoData->wsaBuf.len = E_PACKET_TYPE_LENGTH;
		pIoData->wsaBuf.buf = (char*)&pIoData->eType;

		dwFlag = 0;

		int nRetval = WSARecv(pHandleData->sockClient, &pIoData->wsaBuf, 1, NULL, &dwFlag, &pIoData->overlapped, NULL);
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

void C_MAINSERVER::wcharToString(std::string & pStrDst, LPCWSTR strSrc)
{
	std::wstring wstrTmp = strSrc;
	std::string strTmp(wstrTmp.begin(), wstrTmp.end());
	pStrDst = strTmp;
}
