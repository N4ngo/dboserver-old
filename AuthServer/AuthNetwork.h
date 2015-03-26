#ifndef _AUTHNETWORK_H
#define _AUTHNETWORK_H

#include <iostream>
#include <fstream>
#include <Network.h>
#include <Acceptor.h>
#include <Connector.h>
#include <Session.h>
#include <SessionFactory.h>
#include <Packet.h>
#include <Config.h>
#include <ServerApp.h>
#include <Database.h>
#include <Encoder.h>
#include "AuthProtocol.h"

enum AUTH_SESSION
{
	SESSION_CLIENT,
	SESSION_SERVER_ACTIVE,
};

const DWORD	MAX_NUMOF_GAME_CLIENT = 3;
const DWORD	MAX_NUMOF_SERVER = 1;
const DWORD	MAX_NUMOF_SESSION = MAX_NUMOF_GAME_CLIENT + MAX_NUMOF_SERVER;

class AuthClient;
class AuthClientFactory;

class AuthServer : public ServerApp
{
public:
	int	OnInitApp();
	int	OnCreate();
	void OnDestroy() {}
	int	OnConfiguration(const char* ConfigFile);
	int OnCommandArgument(int argc, _TCHAR* argv[]) { return 0; }
	int	OnAppStart();
	void Run()
	{
		DWORD TickCur, TickOld = ::GetTickCount();

		while (IsRunnable())
		{
			TickCur = ::GetTickCount();
			if (TickCur - TickOld >= 10000)
			{
				TickOld = TickCur;
			}
		}
	}

private:
	Acceptor _clientAcceptor;

public:
	Config* ServerCfg;
	Database* ServerDB;
	int ServerID;
};

class AuthClient : public Session
{
	friend class AuthClientFactory;
public:
	AuthClient(bool IsAliveCheck = false, bool IsOpcodeCheck = false);
	~AuthClient();

	int	OnAccept();
	void OnClose();
	int	OnDispatch(Packet* pPacket);
	void Send(void* pData, int nSize);

	// DATA Methods
	char* GenAuthKey();
	int GetDBAccountAcLevel();
	BYTE GetDBLastServerID();
	int GetDBAccountID();
	ResultCodes LoginVerifyAccount();

	// Opcode Control
	bool PacketControl(Packet* pPacket);

	// Protocol Resolvers
	void SendLoginRes(sUA_LOGIN_REQ* data);
	void SendDisconnectRes(sUA_LOGIN_DISCONNECT_REQ* data);

private:
	PacketEncoder _packetEncoder;
	AuthServer* pServer;

	WCHAR userName[MAX_USERNAME_SIZE + 1];
	WCHAR passWord[MAX_PASSWORD_SIZE + 1];
	BYTE AuthKey[MAX_AUTHKEY_SIZE];
	int AccountID;
	BYTE LastServerID;
	DWORD AcLevel;
	BYTE CurrServerID;
	BYTE CurrChannelID;
	unsigned int CurrCharID;
	bool goCharServer;
};

class AuthClientFactory : public SessionFactory
{
public:
	Session* CreateSession(unsigned short sessionType)
	{
		Session* pSession = NULL;
		switch (sessionType)
		{
		case SESSION_CLIENT:
		{
			pSession = new AuthClient;
		}
		break;

		default:
			break;
		}
		return pSession;
	}
};

#endif