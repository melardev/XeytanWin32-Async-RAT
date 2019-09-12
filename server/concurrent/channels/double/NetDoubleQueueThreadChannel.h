#pragma once
#include "../../../../shared/ConcurrentLib/channels/double/DoubleQueueThreadChannel.h"

class NetEvent;

class NetDoubleQueueThreadChannel : public DoubleQueueThreadChannel<NetEvent>
{
public:
	NetDoubleQueueThreadChannel();
	~NetDoubleQueueThreadChannel();

	NetEvent* takeFromNetService();
	void sendToNetService(NetEvent* event);
	NetEvent* takeFromApp();
	void sendToApp(NetEvent* event);
protected:
	
};
