#pragma once
#include "../../../../shared/ConcurrentLib/channels/single/IThreadChannel.h"

class NetEvent;

class INetThreadChannel // : public IThreadChannel<NetEvent>
{
public:
	INetThreadChannel() = default;
	virtual ~INetThreadChannel() = default;

	virtual void submitToNetService(NetEvent* event) = 0;
	virtual NetEvent* takeFromNetService() = 0;
};
