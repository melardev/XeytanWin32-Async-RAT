#pragma once
#include "../../../../shared/ConcurrentLib/channels/single/QueueThreadChannel.h"

class AppEvent;
class Mutex;

class NetAppQueuedThreadChannel : public QueueThreadChannel<AppEvent>
{
public:
	NetAppQueuedThreadChannel();
	~NetAppQueuedThreadChannel();

	void submitAppEvent(AppEvent* event);
	AppEvent* takeAppEvent();


private:
	Mutex* mutex;
};
