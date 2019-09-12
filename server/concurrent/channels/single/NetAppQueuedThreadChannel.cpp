#include "NetAppQueuedThreadChannel.h"
#include "../../../../shared/ConcurrentLib/core/Mutex.h"


NetAppQueuedThreadChannel::NetAppQueuedThreadChannel()
{
	mutex = new Mutex("NetAppQueuedThreadChannel");
}


NetAppQueuedThreadChannel::~NetAppQueuedThreadChannel()
{
}

void NetAppQueuedThreadChannel::submitAppEvent(AppEvent* event)
{
	submitSync(event);
}

AppEvent* NetAppQueuedThreadChannel::takeAppEvent()
{
	return takeSync();
}
