#pragma once
#include "../../../../shared/ConcurrentLib/channels/double/DoubleQueueThreadChannel.h"

class AppEvent;

class NetAppDoubleQueueThreadChannel : public DoubleQueueThreadChannel<AppEvent>
{
public:
	NetAppDoubleQueueThreadChannel();
	~NetAppDoubleQueueThreadChannel();

	AppEvent* takeFromApp();
	void sendToApp(AppEvent* event);

	AppEvent* takeFromNet();
	void sendToNet(AppEvent* event);
};
