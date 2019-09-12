#pragma once
#include "InetThreadChannel.h"
#include "../../../../shared/ConcurrentLib/channels/single/QueueThreadChannel.h"

class NetEvent;

class NetQueuedThreadChannel : public QueueThreadChannel<NetEvent>, public INetThreadChannel
{
public:
	NetQueuedThreadChannel();
	~NetQueuedThreadChannel();
	// virtual NetEvent* takeSync();
	// virtual void submitSync(NetEvent*);
	void submitToNetService(NetEvent* event) override;
	NetEvent* takeFromNetService() override;
};
