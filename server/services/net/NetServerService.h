#pragma once
#include "NetClientService.h"
#include "../../concurrent/events/net/NetEvent.h"
#include "../UuidService.h"
#include "../../concurrent/channels/double/NetAppDoubleQueueThreadChannel.h"
#include <map>
#include "../../../shared/NetLib/packets/PacketProcess.h"


class Client;
class Mutex;
// #include "Logger.h"
class NetClientService;

class NetServerService : public BaseNetService
{
public:
	NetServerService(const NetAppDoubleQueueThreadChannel& channel);
	NetServerService(const std::queue<NetEvent*>& inEventsQueue, const std::queue<NetEvent>& outEventsQueue);

	~NetServerService();
	void startSendPacketsLoopAsync();
	void start();

	NetError* getErrorOnBind();

	SOCKET createTCPServer(unsigned short port);

	// NetServerService(int port);

	void listenForClients();
	// NetClientService* getClientByUuid(UUID* uuid);
	void loopPacketsQueue();
	void sendPacketAsync(Packet* packet);
	void onError(NetError* appError);
	std::map<SOCKET, NetClientService*> getClientsMap();
	std::vector<Client*> getClients();
	std::vector<NetClientService*>::size_type getClientsCount() const;
	void onPacketReceived(Client* client, Packet* packet);
	void onClientDisconnected(Client* client);
	Client* getClient(unsigned long long clientId);
	void queuePacket(Packet* packet, unsigned long long clientId);
	void loopAppEvents();

private:
	// std::vector<std::shared_ptr<NetClientService>> clients;
	std::map<SOCKET, NetClientService*> clients;
	// std::mutex clientsMutex;
	Mutex* clientsMutex;
	// LoggingService* logger;
	int port;

	// boost::uuids::random_generator uuidGenerator;

	hostent* addrResolved;


	std::queue<Packet*> packetsToBeSent;
	Thread* packetSendThread;
	UuidService* uuidService;
	bool running;
	struct sockaddr_in serverAddr;

	NetAppDoubleQueueThreadChannel netEventChannel;

	std::condition_variable cv;
	std::mutex outPacketsMutex;
	Thread* appEventsThread;
};
