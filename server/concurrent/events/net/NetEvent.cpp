#include "NetEvent.h"


NetEvent::NetEvent()
{
}

NetEvent::NetEvent(NetEventType eventType) : eventType(eventType)
{
}

NetEvent::NetEvent(NetEventType eventType, const char* message): eventType(eventType), description(message)
{
}

NetEvent::NetEvent(NetEventType eventType, void* rawData): eventType(eventType), data(rawData)
{
}


NetEvent::~NetEvent()
{
}

NetEventType NetEvent::getType()
{
	return eventType;
}

std::string NetEvent::getMessage()
{
	return description;
}

void* NetEvent::getData()
{
	return data;
}
