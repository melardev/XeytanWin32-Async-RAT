#pragma once
#include "AppEvent.h"

class Client;

class ClientAppEvent : public AppEvent
{
public:
	ClientAppEvent();
	ClientAppEvent(Subject subject, Action action, Client* client);
	ClientAppEvent(Subject subject, Action start, Client* client, void* object);
	ClientAppEvent(Subject subject, Action start, unsigned long long clientId);
	ClientAppEvent(Subject subject, Client* client);
	ClientAppEvent(Subject subject, Client* client, void* packet);
	~ClientAppEvent();
	Client* getClient();
	Client* client;
	unsigned long long clientId;
};
