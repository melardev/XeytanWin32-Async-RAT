#include "PacketNetEvent.h"


PacketNetEvent::PacketNetEvent(Packet* packet) : NetEvent(NetEventType::PacketReceived)
                                                 , packet(packet)
{
}

PacketNetEvent::~PacketNetEvent()
{
}
