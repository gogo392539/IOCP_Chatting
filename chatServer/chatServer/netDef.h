#pragma once

enum
{
	E_BUF_ID_MAX	= 13,
	E_BUF_NICK_MAX	= 13,
	E_BUF_PW_MAX	= 13,
	E_BUF_CHAT_MAX	= 128,
	E_PACKET_MAX	= 153,
	E_SERVER_PORT	= 20000,
};

enum class E_PACKET_TYPE
{
	E_NONE,
	E_LOGIN,
	E_LOGOUT,
	E_DATA,
	E_VOICE_ACTIVATION,
	E_VOICE_DEACTIVATION,
	E_MAX
};

struct S_PACKET//
{
	int nBufLen;
	E_PACKET_TYPE eType;
	int nId;
	char dataBuf[E_BUF_CHAT_MAX];
};

struct S_PACKET_CHAT
{
	E_PACKET_TYPE	eType;
	int				nNickLen;
	WCHAR			strNick[E_BUF_NICK_MAX];
	int				nChatLen;
	WCHAR			strChat[E_BUF_CHAT_MAX];
};

struct S_PACKET_LOGIN
{
	E_PACKET_TYPE	eType;
	int				nIdLen;
	WCHAR			strId[E_BUF_ID_MAX];
	int				nPwLen;
	WCHAR			strPw[E_BUF_PW_MAX];
};

struct S_PACKET_LOGOUT
{
	E_PACKET_TYPE	eType;
	int				nNickLen;
	WCHAR			strNick[E_BUF_NICK_MAX];
};

struct S_PACKET_JOIN
{

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
	//S_PACKET packetData;
};
