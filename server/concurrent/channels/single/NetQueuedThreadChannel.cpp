
#include "NetQueuedThreadChannel.h"
#include <iostream>


NetQueuedThreadChannel::NetQueuedThreadChannel()
{
}


NetQueuedThreadChannel::~NetQueuedThreadChannel()
{
}


void NetQueuedThreadChannel::submitToNetService(NetEvent* event)
{
	submitSync(event);
}

NetEvent* NetQueuedThreadChannel::takeFromNetService()
{
	return takeSync();
}
