#pragma once
#include "UiEvent.h"
#include <string>


class Client;

class ClientUiEvent : public UiEvent
{
public:
	ClientUiEvent(EventType event, Client* clientInfo);
	virtual ~ClientUiEvent();
	std::string getUuid();
	Client* getDto();
protected:
	std::string uuid;
	Client* clientInfo;
};
