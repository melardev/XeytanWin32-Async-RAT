#include "NetDoubleQueueThreadChannel.h"
#include "../single/NetQueuedThreadChannel.h"


NetDoubleQueueThreadChannel::~NetDoubleQueueThreadChannel()
{
}

NetDoubleQueueThreadChannel::NetDoubleQueueThreadChannel() : DoubleQueueThreadChannel<NetEvent>(
	new NetQueuedThreadChannel, new NetQueuedThreadChannel)
{
}

NetEvent* NetDoubleQueueThreadChannel::takeFromNetService()
{
	return takeFromLeft();
}

void NetDoubleQueueThreadChannel::sendToNetService(NetEvent* event)
{
	submitToRight(event);
}

NetEvent* NetDoubleQueueThreadChannel::takeFromApp()
{
	return takeFromRight();
}

void NetDoubleQueueThreadChannel::sendToApp(NetEvent* event)
{
	submitToLeft(event);
}

