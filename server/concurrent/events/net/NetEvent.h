#pragma once
#include <string>

enum NetEventType
{
	ServerError,
	ServerStatusChange,

	PacketReceived,
	// Client event types
	NewClientConnection,
	ClientError,
	ClientStatusChange,
	ClientSysInfoReceived,
};

class NetEvent
{
public:
	NetEvent();
	NetEvent(NetEventType eventType);
	NetEvent(NetEventType eventType, const char* message);
	NetEvent(NetEventType eventType, void* data);
	virtual ~NetEvent();
	NetEventType getType();
	std::string getMessage();
	void* getData();
private:
	NetEventType eventType;
	std::string description;
	void* data;
};
