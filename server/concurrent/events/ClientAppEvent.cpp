#include "ClientAppEvent.h"

ClientAppEvent::ClientAppEvent()
{
}

ClientAppEvent::ClientAppEvent(Subject subject, Action action, Client* client)
	: AppEvent(Target::Client, subject, action), client(client)
{
}

ClientAppEvent::ClientAppEvent(Subject subject, Action action, Client* client, void* object)
	: AppEvent(Target::Client, subject, action, object), client(client)
{
}

ClientAppEvent::ClientAppEvent(Subject subject, Action action, unsigned long long clientId)
	: AppEvent(Target::Client, subject, action, nullptr), clientId(clientId)
{
}

ClientAppEvent::ClientAppEvent(Subject subject, Client* client): AppEvent(
	Target::Client, subject, Action::InvalidAction)
{
}

ClientAppEvent::ClientAppEvent(Subject subject, Client* client, void* object)
	: AppEvent(Target::Client, subject, Action::InvalidAction, object), client(client)
{
}


ClientAppEvent::~ClientAppEvent()
{
	// Do not delete Client
}

Client* ClientAppEvent::getClient()
{
	return client;
}
