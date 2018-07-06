#pragma once

enum
{
	E_PACKET_TYPE_LENGTH_SIZE	= 4,
	E_DATA_LENGTH_SIZE			= 4,
	E_MAX_ID_LENGTH				= 13,
	E_MAX_NICK_LENGTH			= 13,
	E_MAX_PW_LENGTH				= 13,
	E_MAX_MSG_LENGTH			= 128,
	E_SERVER_PORT				= 20000,
	E_JOIN_SERVER_PORT			= 20000,
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
	E_MESSAGE,
	E_MAX
};

#pragma pack(push, 1)  
struct S_CTS_LOGIN_PACKET
{
//	E_PACKET_TYPE	eType;
	int				nDataSize;
	int				nIdLen;
	int				nPwLen;
	WCHAR			wstrData[E_MAX_ID_LENGTH + E_MAX_PW_LENGTH];
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
	WCHAR			wstrNick[E_MAX_ID_LENGTH];
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
	WCHAR			wstrData[E_MAX_MSG_LENGTH + E_MAX_NICK_LENGTH];
};

struct S_CTS_MSG_PACKET
{
	E_PACKET_TYPE	eType;
	int				nDataSize;
	int				nMgsLen;
	WCHAR			wstrMsg[E_MAX_MSG_LENGTH];
};
#pragma pack(pop)


// IOCP_STRCUT

struct S_HANDLE_DATE
{
	SOCKET			sockClient;
	int				nId;
	std::map<int, std::wstring>::iterator iterNickList;
	std::map<int, S_HANDLE_DATE*>::iterator iter;
};

struct S_IO_DATA
{
	OVERLAPPED		overlapped;
	WSABUF			wsaBuf;
	E_PACKET_TYPE	eType;
};

//Join

enum class E_JOIN_PACKET_TYPE
{
	E_NONE,
	E_ID_CHECK,
	E_ID_CHECK_SUCCESS,
	E_ID_CHECK_FAIL,
	E_NICK_CHECK,
	E_NICK_CHECK_SUCCESS,
	E_NICK_CHECK_FAIL,
	E_JOIN,
	E_EXIT,
	E_MAX
};

#pragma pack(push, 1)  
struct S_CTS_ID_CHECK_PACKET
{
	E_JOIN_PACKET_TYPE	eType;
	int				nIdLen;
	WCHAR			wstrId[E_MAX_ID_LENGTH];
};

struct S_CTS_NICK_CHECK_PACKET
{
	E_JOIN_PACKET_TYPE	eType;
	int				nNickLen;
	WCHAR			wstrNick[E_MAX_NICK_LENGTH];
};

struct S_CTS_JOIN_PACKET
{
	E_JOIN_PACKET_TYPE	eType;
	int				nDataSize;
	int				nIdLen;
	int				nNickLen;
	int				nPwLen;
	WCHAR			wstrData[E_MAX_ID_LENGTH + E_MAX_NICK_LENGTH + E_MAX_PW_LENGTH];
};

struct S_STC_ID_CHECK_PACKET
{
	E_JOIN_PACKET_TYPE	eType;
};

struct S_STC_NICK_CHECK_PACKET
{
	E_JOIN_PACKET_TYPE	eType;
};

struct S_STC_JOIN_PACKET
{
	E_JOIN_PACKET_TYPE	eType;
};

struct S_EXIT_MESSATE_PACKET
{
	E_JOIN_PACKET_TYPE	eType;
};
#pragma pack(pop)


// JOIN_IOCP_STRCUT

struct S_JOIN_HANDLE_DATE
{
	SOCKET			sockClient;
	int				nId;
	std::map<int, S_JOIN_HANDLE_DATE*>::iterator iter;
};

struct S_JOIN_IO_DATA
{
	OVERLAPPED			overlapped;
	WSABUF				wsaBuf;
	E_JOIN_PACKET_TYPE	eType;
};
