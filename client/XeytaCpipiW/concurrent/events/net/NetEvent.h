#pragma once
#include <string>

enum class NetEventType
{
	ClientServiceExit,
	SocketError,
	ConnectionOpened,
	PacketReceived
};

class NetEvent
{
public:
	NetEvent(NetEventType eventType);
	NetEvent(NetEventType eventType, const char* descr);
	NetEvent(NetEventType eventType, const std::string& descr);
	~NetEvent();
	NetEventType getEventType();
	const char* getDescription();
private:
	NetEventType eventType;
	const char* description;
};
