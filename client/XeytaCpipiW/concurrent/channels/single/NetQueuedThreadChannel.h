#pragma once
#include <mutex>
#include <queue>

class NetEvent;

class NetQueuedThreadChannel
{
public:
	NetQueuedThreadChannel() = default;
	virtual ~NetQueuedThreadChannel();
	
	void submitToApp(NetEvent* event);
	NetEvent* takeFromNetService();
protected:
	std::queue<NetEvent*> queue_;
	std::condition_variable cv;
	std::mutex mu;
};
