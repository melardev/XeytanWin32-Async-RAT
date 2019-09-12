#pragma once
#include "IThreadChannel.h"
#include <queue>
#include <condition_variable>

template <class T>
class QueueThreadChannel : public IThreadChannel<T>
{
public:
	QueueThreadChannel();
	virtual ~QueueThreadChannel() = 0;

	T* takeSync() override;
	void submitSync(T*) override;
protected:
	std::queue<T*> queue_;
	std::condition_variable cv;
	std::mutex mu;
};

template <class T>
QueueThreadChannel<T>::QueueThreadChannel()
{
}

template <class T>
QueueThreadChannel<T>::~QueueThreadChannel()
{
}

template <class T>
T* QueueThreadChannel<T>::takeSync()
{
	std::unique_lock<std::mutex> lock(mu);
	// same as below:
	// while (queue.size() == 0) {
	//   empty.wait(lock);
	// }
	// cv.wait(lock, [&]() { return queue_.size() != 0; });
	if (queue_.size() > 0)
	{
		auto event = queue_.front();
		queue_.pop();
		return event;
	}
	else
	{
		cv.notify_one();
		cv.wait(lock);
		auto event = queue_.front();
		queue_.pop();
		return event;
	}
}

template <class T>
void QueueThreadChannel<T>::submitSync(T* event)
{
	// TODO: remove wait(lk)
	std::unique_lock<std::mutex> lk(mu);

	// same as below:
	// while (queue_.size() == capacity) {
	//   cv.wait(lock);
	// }
	// cv.wait(lk, [&]() { return queue_.size() != capacity; });

	queue_.push(event);
	cv.notify_one(); // Notify worker
	cv.wait(lk);
}
