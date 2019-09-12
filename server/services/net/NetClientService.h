#pragma once
#include <vector>
#include "../../../shared/NetLib/services/BaseNetClientService.h"

class Packet;
class Client;
class NetServerService;
class Thread;

class NetClientService : public BaseNetClientService
{
public:
	NetClientService(UUID* uuid, NetServerService* server, SOCKET clientSocket);
	~NetClientService();

	virtual void onDisconnected() override;
	virtual void onPacketReceived(PacketType packet, char* payloadBuffer, size_t payloadLength) override;

	Client* getClient();

	/*
	bool ReceivePacket(PacketType packet);
	std::tuple<bool, int> recvNBytes(char* buffer, unsigned long long numBytesToRead) const;
	bool receivePacketStruct(PacketStruct& packetStruct);
	void Interact();
	bool GetPacketType(PacketType& _PacketType);
	bool readInt(int32_t& int32);
	bool readIntAsPacketType(int32_t& _int32_t);
	bool recvall(int ID, char* data, int totalbytes);
	void handlePacket(const Packet* packet);
	bool HandleSendFile(int ID);

	

	
	UUID* getUuid();
	*/

	
private:
	
	NetServerService* netServerService;
	Client* client;
};
