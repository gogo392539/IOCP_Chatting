#pragma once

enum
{
	E_PACKET_TYPE_LENGTH	= 4,
	E_DATA_LENGTH			= 4,
	E_MAX_ID_LENGTH			= 13,
	E_MAX_NICK_LENGTH		= 13,
	E_MAX_PW_LENGTH			= 13,
	E_MAX_MSG_LENGTH		= 128,
//	E_PACKET_MAX			= 516,
	E_SERVER_PORT			= 20000,
};

enum class E_PACKET_TYPE
{
	E_NONE,
	E_LOGIN_CALL,
	E_LOGOUT,
	E_LOGIN_SUCCESS,
	E_LOGIN_FAIL,
	E_VOICE_ACT,
	E_VOICE_DEACT,
	E_VOICE_ACT_CHECK,
	E_VOICE_DEACT_CHECK,
	E_MESSAGE,
	E_MAX
};

struct S_CTS_LOGIN_PACKET
{
//	E_PACKET_TYPE	eType;
	int				nDataSize;
	int				nIdLen;
	int				nPwLen;
	WCHAR			strData[E_MAX_ID_LENGTH + E_MAX_PW_LENGTH];
};

struct S_STC_LOGIN_PACKET
{
	E_PACKET_TYPE	eType;
	int				nSerialId;
};

struct S_CTS_LOGOUT_PACKET
{
	E_PACKET_TYPE	eType;
	int				nSerialId;
};

struct S_STC_LOGOUT_PACKET
{
	E_PACKET_TYPE	eType;
	int				nNickLen;
	WCHAR			strNick[E_MAX_ID_LENGTH];
};

struct S_VOICE_PACKET
{
	E_PACKET_TYPE	eType;
	int				nSerialId;
};

struct S_STC_MSG_PACKET
{
	E_PACKET_TYPE	eType;
	int				nDataSize;
	int				nNickLen;
	int				nMgsLen;
	WCHAR			strNick[E_MAX_NICK_LENGTH];
	WCHAR			strMsg[E_MAX_MSG_LENGTH];
};

struct S_CTS_MSG_PACKET
{
	E_PACKET_TYPE	eType;
	int				nDataSize;
	int				nSerialId;
	int				nMgsLen;
	WCHAR			strMsg[E_MAX_MSG_LENGTH];
};


// IOCP_STRCUT

struct S_HANDLE_DATE
{
	SOCKET			sockClient;
	int				nId;
	std::map<int, S_HANDLE_DATE*>::iterator iter;
};

struct S_IO_DATA
{
	OVERLAPPED		overlapped;
	WSABUF			wsaBuf;
	E_PACKET_TYPE	eType;
};
