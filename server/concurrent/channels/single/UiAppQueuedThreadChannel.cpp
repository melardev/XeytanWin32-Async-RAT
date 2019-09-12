#include "UiAppQueuedThreadChannel.h"
#include "../../../../shared/ConcurrentLib/core/Mutex.h"


UiAppQueuedThreadChannel::UiAppQueuedThreadChannel()
{
	mutex = new Mutex("UiQueuedThreadChannel");
}


UiAppQueuedThreadChannel::~UiAppQueuedThreadChannel()
{
}

void UiAppQueuedThreadChannel::submitAppEvent(AppEvent* event)
{
	submitSync(event);
}

AppEvent* UiAppQueuedThreadChannel::takeAppEvent()
{
	return takeSync();
}
