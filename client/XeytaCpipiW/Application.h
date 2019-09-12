#pragma once

// #pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000

#include "concurrent/channels/single/NetQueuedThreadChannel.h"

class Packet;
class NetClientService;
class NetEvent;
class ShellService;
class ProcessService;
class Thread;

class Application
{
public:

	Application();
	static Application* start();

	void startNetSubsystem();

	int run();

	void handlePacket(Packet* packet);
	void onShellClosed() const;
	void onShellOutput(char* buffer, unsigned long bufferLen) const;
	void handleNetEvent(NetEvent* event);
	int reactToNetEvents();

	~Application();

private:
	NetClientService* netClientService;
	Thread* netClientThread;
	NetQueuedThreadChannel netEventChannel;
	bool running;

	ShellService* shellService;
};
