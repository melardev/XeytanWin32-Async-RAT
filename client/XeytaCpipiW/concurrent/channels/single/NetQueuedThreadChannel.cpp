#include "NetQueuedThreadChannel.h"




NetQueuedThreadChannel::~NetQueuedThreadChannel()
{
}

void NetQueuedThreadChannel::submitToApp(NetEvent* event)
{
	std::unique_lock<std::mutex> lk(mu);
	queue_.push(event);
	cv.notify_one(); // Notify worker
	cv.wait(lk);
}

NetEvent* NetQueuedThreadChannel::takeFromNetService()
{
	std::unique_lock<std::mutex> lock(mu);
	cv.notify_one();
	cv.wait(lock);
	auto event = queue_.front();
	queue_.pop();
	return event;
}
