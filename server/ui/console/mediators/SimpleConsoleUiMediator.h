#pragma once
#include "../../../appdefs.h"
#include "../../mediators/IUiMediator.h"
#include "../AppConsole.h"
// #include "../../../concurrent/events/ui/UiEvent.h"
#include "../../../../shared/ConcurrentLib/core/Thread.h"
#include "../../../concurrent/channels/double/UiAppDoubleQueueThreadChannel.h"

// class UiDoubleQueueThreadChannel;

class UiEvent;
class AppEvent;
class Application;

class SimpleConsoleUiMediator : public IUiMediator
{
public:
	SimpleConsoleUiMediator(const UiAppDoubleQueueThreadChannel& uiChannel);

	// Not used
	// SimpleConsoleUiMediator(const UiDoubleQueueThreadChannel& uiCommunicator);
	// SimpleConsoleUiMediator(const std::queue<UiEvent*>& inEventsQueue, const std::queue<UiEvent*>& outEventsQueue);

	~SimpleConsoleUiMediator();
	void showMainUi() override;


	// UiEvent* getEventForInstruction(char* instruction);

#ifdef MANUAL_MEM_MANAGEMENT
	void dispatchEvent(UiEvent* event)
	{
	};
	void dispatchEvent(AppEvent* event);
#else

#endif


	void printHelp();
	bool handleInstruction(char* instruction);
	void loop() override;
	std::wstring* getFilePath(char* instruction);
	AppEvent* getEventForInstruction(char* instruction);
	void stop();
	void loopUiEvents();
	virtual void updateStatus(const std::string& statusMessage) override;
	virtual void showErrorMessage(const char* str) override;
	void showErrorMessage(const std::string& str) const;
	virtual void addFeedbackMessage(const std::string& message) override;
	void onShellOutput(Client* client, const std::string& data);
	void onShellClosed(Client* client);
	bool running;
	AppConsole appConsole;

	// UiDoubleQueueThreadChannel uiChannel;

	UiAppDoubleQueueThreadChannel uiAppChannel;


	Thread* uiEventLooperThread;
	bool isStreamingDesktop;


	// std::queue<UiEvent*> inUiEventsQueue;
	// std::queue<UiEvent*> outUiEventsQueue;
	//
	SRWLOCK viewLock;
	SRWLOCK clientLock;
};
