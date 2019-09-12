#pragma once
// Not implemented yet, not used
#include "../channels/single/QueueThreadChannel.h"
#include "../core/Thread.h"
#include "../events/GuiEvent.h"

template <class T>
class IThreadCommunicator
{
public:
	IThreadCommunicator();

	virtual ~IThreadCommunicator()
	{
	};
protected:
	virtual void submit() =0;
};

template <class T>
IThreadCommunicator<T>::IThreadCommunicator()
{
}

template <class T>
class DoubleThreadCommunicator
{
public:
	DoubleThreadCommunicator(Thread* leftThread, Thread* rightThread): rightThread(rightThread), leftThread(leftThread)
	{
	}


	virtual ~DoubleThreadCommunicator()
	{
	};
	

	void start()
	{
		leftThread->run();
		rightThread->run();
	}

protected:
	Thread* rightThread;
	Thread* leftThread;
};


template <class T>
class DoubleQueuedThreadCommunicator : public DoubleThreadCommunicator<T>
{
public:
	DoubleQueuedThreadCommunicator(Thread* leftThread, Thread* rightThread): 
	DoubleThreadCommunicator<T>(leftThread, rightThread)
	{
	}
	;

	virtual ~DoubleQueuedThreadCommunicator()
	{
	};

	void submitToLeft(T* event)
	{
		leftThreadChannel->submitSync(event);
	};


	void submitToRight(T* event)
	{
		rightThreadChannel->submitSync(event);
	};

protected:
	QueueThreadChannel<T*>* leftThreadChannel;
	QueueThreadChannel<T*>* rightThreadChannel;
};


DWORD WINAPI loopLeft(LPVOID)
{
	return 0;
}

DWORD WINAPI loopRight(LPVOID)
{
	return 0;
}

class UiThreadCommunciator : public DoubleQueuedThreadCommunicator<GuiEvent>
{
public:
	UiThreadCommunciator():
		DoubleQueuedThreadCommunicator<GuiEvent>(
			new Thread((LPTHREAD_START_ROUTINE)loopLeft, nullptr, 0, 0, 0),
			new Thread((LPTHREAD_START_ROUTINE)loopLeft, nullptr, 0, 0, 0)
		)
	{
	}

	~UiThreadCommunciator() override;
	void submitToLeft(GuiEvent* event){} ;
	void submitToRight(GuiEvent* event){};
};

