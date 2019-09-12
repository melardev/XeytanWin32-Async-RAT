#pragma once
#include "NetEvent.h"

class Client;

class ClientNetEvent : public NetEvent
{
public:
	ClientNetEvent(NetEventType event, Client* clientInfo);
	~ClientNetEvent();

protected:
	Client* client;
};
