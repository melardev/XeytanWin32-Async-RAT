#include "NetEvent.h"


NetEvent::NetEvent(NetEventType eventType) : eventType(eventType)
{
}

NetEvent::NetEvent(NetEventType eventType, const char* descr) : eventType(eventType), description(descr)
{
}



NetEvent::NetEvent(NetEventType eventType, const std::string& descr) : eventType(eventType), description(descr.c_str())
{
}

NetEvent::~NetEvent()
{
}

NetEventType NetEvent::getEventType()
{
	return eventType;
}

const char* NetEvent::getDescription()
{
	return description;
}
