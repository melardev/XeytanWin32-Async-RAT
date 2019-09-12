#pragma once
#include "NetEvent.h"

class Packet;

class PacketNetEvent : public NetEvent
{
public:
	PacketNetEvent(Packet* packet);
	~PacketNetEvent();
	Packet* packet;
};
