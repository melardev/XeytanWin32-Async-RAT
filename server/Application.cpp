#include "Application.h"
#include "ui/console/mediators/SimpleConsoleUiMediator.h"
#include "services/net/NetServerService.h"
#include <iostream>
#include "concurrent/events/ui/ClientUiEvent.h"
#include "concurrent/events/ClientAppEvent.h"
#include "../shared/ConcurrentLib/core/Thread.h"
#include "../shared/NetLib/packets/PacketDownload.h"
#include "../shared/IOLib/services/FileSystemService.h"
#include "models/Client.h"
#include "../shared/NetLib/packets/PacketMediaResponse.h"
#include "services/DesktopService.h"


DWORD WINAPI listenNetEventsThreadFunc(LPVOID threadArg)
{
	auto app = static_cast<Application*>(threadArg);
	app->loopNetEvents();
	return 0;
}

DWORD WINAPI listenUiEventsThreadFunc(LPVOID threadArg)
{
	auto app = static_cast<Application*>(threadArg);
	app->loopUiEvents();
	return 0;
}

DWORD WINAPI serverThreadFunc(LPVOID threadArg)
{
	auto server = static_cast<NetServerService*>(threadArg);
	server->start();
	return 0;
}

Application::Application(const CommandLineArguments& cmd): cli(cmd)
{
}

Application::~Application()
{
}


void Application::run()
{
	running = true;
	startUiSubsystem();
	startNetSubsystem();

	uiMediator->loop();
}


void Application::startUiSubsystem()
{
	startUi();
	loopUiEventsAsync();
}


void Application::startUi()
{
	if (cli.getUiMode() == Win32Console)
	{
		// uiMediator = new SimpleConsoleUiMediator(uiEventChannel);
		// uiMediator = new SimpleConsoleUiMediator(outUiEventsQueue, inUiEventsQueue);
		uiMediator = new SimpleConsoleUiMediator(uiAppEventChannel);
		uiMediator->showMainUi();
	}
	else if (cli.getUiMode() == Win32Graphical)
	{
		// TODO throw not implemented
	}
	else
	{
		// TODO throw not implemented
	}
}

void Application::loopUiEventsAsync()
{
	uiEventLooperThread = new Thread((LPTHREAD_START_ROUTINE)listenUiEventsThreadFunc, (PVOID)this, 0, 0, TRUE);
	uiEventLooperThread->run();
}


// Initializes the server and the thread that loops looking for Events comming from the server
void Application::startNetSubsystem()
{
	startServerAsync();
	loopNetEventsAsync();
}

void Application::startServerAsync()
{
	// netServerService = new NetServerService(outNetEventsQueue, inNetEventsQueue);
	netServerService = new NetServerService(netEventChannel);
	serverThread = new Thread(static_cast<LPTHREAD_START_ROUTINE>(serverThreadFunc),
	                          static_cast<PVOID>(netServerService));
	serverThread->run();
}

void Application::loopNetEventsAsync()
{
	netEventLooperThread = new Thread((LPTHREAD_START_ROUTINE)listenNetEventsThreadFunc, (PVOID)this, 0, 0, TRUE);
	netEventLooperThread->run();
}

void Application::loopNetEvents()
{
	while (running)
	{
		AppEvent* event = netEventChannel.takeFromNet();

		// NetEvent event = inNetEventsQueue.front();

		if (event == nullptr)
			continue;
		if (event->target == Target::Client)
		{
			ClientAppEvent* clientEvent = (ClientAppEvent*)event;
			Client* client = clientEvent->client;
			if (event->subject == Subject::Connection && event->action == Action::Start)
			{
				// TODO: save into database
			}
			else if (event->subject == Subject::Download)
			{
				auto packet = static_cast<PacketDownload*>(event->object);

				FileSystemService::writeBinaryFile(L"./Downloads/temp.out", packet->fileBytes, packet->fileSize);
				delete packet;
				event->object = nullptr;
				continue;
			}
			else if (event->subject == Subject::PacketReceived)
			{
				const auto packet = (Packet*)event->object;
				if (packet->getType() == PacketType::Desktop)
				{
					const auto packetDesktop = static_cast<PacketMediaResponse*>(packet);

					const auto imagePath = new std::wstring(L"D:/temp/desktop_out.jpeg");
					DesktopService::saveBmpFromByteArray(packetDesktop->imageBytes, packetDesktop->imageSize,
					                                     imagePath->c_str());
					// Delete packet(release image bytes buffer)
					delete packet;
					event->object = nullptr;

					// reuse the same event object so update the fields
					event->subject = Subject::Desktop;
					event->action = Action::Update;
					event->object = imagePath;
				}
			}
		}

		uiAppEventChannel.sendToUi(event);
	}
}


void Application::loopUiEvents()
{
	while (running)
	{
		AppEvent* event = uiAppEventChannel.takeFromUi();
		bool eventProcessed = false;
		if (event == nullptr)
			continue;

		netEventChannel.sendToNet(event);

		// We are done with the Event, delete it
		if (eventProcessed)
			delete event;
	}
}

void Application::onUiEvent(const UiEvent& event)
{
	std::cerr << "Should not be used" << std::endl;
}
