
#include "ServerAppEvent.h"

ServerAppEvent::ServerAppEvent(ServerEventType serverEventType): serverEventType(serverEventType)
{
}

ServerAppEvent::ServerAppEvent(ServerEventType serverEventType, Subject subject, Action action)
	: AppEvent(Target::Server, subject, action), serverEventType(serverEventType)
{
}

ServerAppEvent::ServerAppEvent(ServerEventType serverEventType, const char* str)
	: AppEvent(Target::Server), serverEventType(serverEventType)
{
	description = str;
}

ServerAppEvent::~ServerAppEvent()
{
}
