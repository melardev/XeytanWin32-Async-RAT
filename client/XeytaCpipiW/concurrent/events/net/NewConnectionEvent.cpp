#include "NewConnectionEvent.h"
#include <utility>

/*
// TODO: by value or by ref ?
NewConnectionEvent::NewConnectionEvent(std::shared_ptr<NetClientService> client)
	: clientService(std::move(client))
{
}

NewConnectionEvent::~NewConnectionEvent()
{
}

std::shared_ptr<NetClientService> NewConnectionEvent::getClientService() const
{
	return clientService;
}

*/