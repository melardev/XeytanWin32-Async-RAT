#pragma once
#include "IDoubleThreadChannel.h"
#include "../single/QueueThreadChannel.h"


template <class T>
class DoubleQueueThreadChannel : public IDoubleThreadChannel<T>
{
public:
	DoubleQueueThreadChannel(QueueThreadChannel<T>* left, QueueThreadChannel<T>* right);
	virtual ~DoubleQueueThreadChannel();
protected:
	void submitToLeft(T* event) override;
	void submitToRight(T* event) override;
	T* takeFromLeft() override;
	T* takeFromRight() override;

	QueueThreadChannel<T>* getLeftThreadChannel();
	void setLeftThreadChannel(IThreadChannel<T>*);

	QueueThreadChannel<T>* getRightThreadChannel();
	void setRightThreadChannel(IThreadChannel<T>*);

protected:
	QueueThreadChannel<T>* leftThreadChannel;
	QueueThreadChannel<T>* rightThreadChannel;
};


template <class T>
DoubleQueueThreadChannel<T>::DoubleQueueThreadChannel(QueueThreadChannel<T>* left, QueueThreadChannel<T>* right):
	leftThreadChannel(left),
	rightThreadChannel(right)
{
}

template <class T>
DoubleQueueThreadChannel<T>::~DoubleQueueThreadChannel()
{
	delete rightThreadChannel;
	delete leftThreadChannel;
}


template <class T>
void DoubleQueueThreadChannel<T>::submitToLeft(T* event)
{
	leftThreadChannel->submitSync(event);
}

template <class T>
void DoubleQueueThreadChannel<T>::submitToRight(T* event)
{
	rightThreadChannel->submitSync(event);
}

template <class T>
T* DoubleQueueThreadChannel<T>::takeFromLeft()
{
	return leftThreadChannel->takeSync();
}

template <class T>
T* DoubleQueueThreadChannel<T>::takeFromRight()
{
	
	return rightThreadChannel->takeSync();
}

template <class T>
QueueThreadChannel<T>* DoubleQueueThreadChannel<T>::getLeftThreadChannel()
{
	return leftThreadChannel;
}

template <class T>
void DoubleQueueThreadChannel<T>::setLeftThreadChannel(IThreadChannel<T>* channel)
{
	leftThreadChannel = channel;
}

template <class T>
QueueThreadChannel<T>* DoubleQueueThreadChannel<T>::getRightThreadChannel()
{
	return rightThreadChannel;
}

template <class T>
void DoubleQueueThreadChannel<T>::setRightThreadChannel(IThreadChannel<T>* channel)
{
	rightThreadChannel = channel;
}
