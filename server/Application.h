#pragma once
#include "concurrent/events/net/NetEvent.h"
#include "services/net/NetServerService.h"
#include "models/CommandLineArguments.h"
#include "concurrent/events/ui/UiEvent.h"


// #include "concurrent/channels/double/UiDoubleQueueThreadChannel.h"
#include "concurrent/channels/double/UiAppDoubleQueueThreadChannel.h"

class SimpleConsoleUiMediator;



class Application
{
public:
	Application(const CommandLineArguments& cmd);
	~Application();
	void startServerAsync();

	void startUi();
	void loopNetEventsAsync();
	void startNetSubsystem();
	void loopUiEventsAsync();
	void startUiSubsystem();
	void run();
	void loopNetEvents();
	void loopUiEvents();
	void onUiEvent(const UiEvent& event);
	CommandLineArguments cli;
	SimpleConsoleUiMediator* uiMediator;
	Thread* netEventLooperThread;
	bool running;

	// UiDoubleQueueThreadChannel uiEventChannel;
	UiAppDoubleQueueThreadChannel uiAppEventChannel;
	NetAppDoubleQueueThreadChannel netEventChannel;
	// NetDoubleQueueThreadChannel netEventChannel;

	// std::queue<UiEvent*> inUiEventsQueue;
	// std::queue<UiEvent*> outUiEventsQueue;

	std::queue<NetEvent> inNetEventsQueue;
	std::queue<NetEvent*> outNetEventsQueue;

	NetServerService* netServerService;
	Thread* serverThread;
	Thread* uiEventLooperThread;
};
