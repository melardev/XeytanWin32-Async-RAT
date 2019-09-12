#pragma once
#include "AppEvent.h"


enum class ServerEventType
{
	ServerError,
	ServerStatusChange,
};

class ServerAppEvent : public AppEvent
{
public:
	ServerAppEvent(ServerEventType serverEventType);
	ServerAppEvent(ServerEventType serverEventType,
	               Subject subject, Action action);
	ServerAppEvent(ServerEventType serverEventType, const char* str);
	~ServerAppEvent();

	ServerEventType serverEventType;
};
