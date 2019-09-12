#pragma once
#include "../../../shared/NetLib/services/BaseNetClientService.h"
#include "../concurrent/channels/single/NetQueuedThreadChannel.h"


class Packet;
enum class PacketType;
class Thread;

class NetClientService : public BaseNetClientService
{
public:
	NetClientService(NetQueuedThreadChannel* netEventChannel);
	~NetClientService();
	bool ReceivePacket(PacketType packet);

	void handlePacket(const Packet* packet);
	bool HandleSendFile(int Id);

	UUID* getUuid();
	DWORD start();

	virtual void onDisconnected() override;
	virtual void onPacketReceived(PacketType packet, char* payloadBuffer, size_t payloadLength) override;
private:

	UUID* clientUuid;
	sockaddr_in addr;
	NetQueuedThreadChannel* netEventChannel;
};
