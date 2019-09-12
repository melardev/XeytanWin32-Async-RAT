#include "NetAppDoubleQueueThreadChannel.h"
#include "../single/NetAppQueuedThreadChannel.h"


NetAppDoubleQueueThreadChannel::NetAppDoubleQueueThreadChannel()
	: DoubleQueueThreadChannel<AppEvent>(new NetAppQueuedThreadChannel, new NetAppQueuedThreadChannel)
{
}

NetAppDoubleQueueThreadChannel::~NetAppDoubleQueueThreadChannel()
{
}

AppEvent* NetAppDoubleQueueThreadChannel::takeFromNet()
{
	return takeFromLeft();
}

void NetAppDoubleQueueThreadChannel::sendToNet(AppEvent* event)
{
	submitToRight(event);
}

AppEvent* NetAppDoubleQueueThreadChannel::takeFromApp()
{
	return takeFromRight();
}

void NetAppDoubleQueueThreadChannel::sendToApp(AppEvent* event)
{
	submitToLeft(event);
}
