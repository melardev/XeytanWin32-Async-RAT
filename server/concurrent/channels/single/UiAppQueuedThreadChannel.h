#pragma once
#include "../../events/AppEvent.h"
#include "../../../../shared/ConcurrentLib/channels/single/QueueThreadChannel.h"

class Mutex;

class UiAppQueuedThreadChannel : public QueueThreadChannel<AppEvent>
{
public:
	UiAppQueuedThreadChannel();
	~UiAppQueuedThreadChannel();

	void submitAppEvent(AppEvent* event);
	AppEvent* takeAppEvent();

private:
	Mutex* mutex;
};
