
#include "ClientNetEvent.h"


ClientNetEvent::ClientNetEvent(NetEventType event, Client* clientInfo)
	: NetEvent(event, (void*)&clientInfo), client(clientInfo)
{
}

ClientNetEvent::~ClientNetEvent()
{
}
