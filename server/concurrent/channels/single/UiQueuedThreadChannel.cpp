
#include "UiQueuedThreadChannel.h"


UiQueuedThreadChannel::UiQueuedThreadChannel()
{
	mutex = new Mutex("UiQueuedThreadChannel");
}


UiQueuedThreadChannel::~UiQueuedThreadChannel()
{
}

void UiQueuedThreadChannel::submitUiEvent(UiEvent* event)
{
	submitSync(event);
}

UiEvent* UiQueuedThreadChannel::takeUiEvent()
{
	return takeSync();
}
