#pragma once
#include "../../../../shared/ConcurrentLib/channels/single/QueueThreadChannel.h"
#include "IUiThreadChannel.h"
#include "../../../../../Win32/ConcurrentLib/core/Mutex.h"

class UiEvent;

class UiQueuedThreadChannel : public IUiThreadChannel, public QueueThreadChannel<UiEvent>
{
public:
	UiQueuedThreadChannel();
	~UiQueuedThreadChannel();

	void submitUiEvent(UiEvent* event) override;
	UiEvent* takeUiEvent() override;
	

private:
	Mutex* mutex;
};
