#pragma once
#include "BaseNetService.h"
#include <vector>
#include "../packets/packetdefs.h"

class Packet;
class Client;
class Thread;

class BaseNetClientService : public BaseNetService
{
public:
	BaseNetClientService(SOCKET clientSocket);
	BaseNetClientService();
	~BaseNetClientService();
	
	void interactAsync();
	void interact();

	bool readHeader();
	bool readBody() const;
	
	virtual void onDisconnected() = 0;
	virtual void onPacketReceived(PacketType packet, char* payloadBuffer, size_t payloadLength) = 0;

	void sendPacket(Packet* packet);

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

protected:
	Thread* thread;
	bool shouldBeRunning;
	Packet* receivedPacket;
};
