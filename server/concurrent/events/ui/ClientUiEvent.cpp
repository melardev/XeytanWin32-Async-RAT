
#include "ClientUiEvent.h"


ClientUiEvent::ClientUiEvent(EventType eventType, Client* clientInfo)
	: UiEvent(eventType),
	  clientInfo(clientInfo)
{
}

ClientUiEvent::~ClientUiEvent()
{
}

std::string ClientUiEvent::getUuid()
{
	return uuid;
}

Client* ClientUiEvent::getDto()
{
	return clientInfo;
}
