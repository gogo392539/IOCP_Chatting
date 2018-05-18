#pragma once

enum
{
	E_BUF_MAX = 256,
	E_PACKET_MAX = 268,
	E_SERVER_PORT = 20000,
};

enum class E_PACKET_TYPE
{
	E_NONE,
	E_LOGIN,
	E_LOGOUT,
	E_DATA,
	E_MAX
};

struct S_PACKET
{
	int nBufLen;
	E_PACKET_TYPE eType;
	int nId;
	char dataBuf[E_BUF_MAX];
};

struct S_HANDLE_DATE
{
	SOCKET sockClient;
	int nId;
	std::map<int, S_HANDLE_DATE*>::iterator iter;
};

struct S_IO_DATA
{
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	S_PACKET packetData;
};

