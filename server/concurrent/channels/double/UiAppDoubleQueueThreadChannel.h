#pragma once
#include "../../events/AppEvent.h"
#include "../../../../shared/ConcurrentLib/channels/double/DoubleQueueThreadChannel.h"

class UiAppDoubleQueueThreadChannel : public DoubleQueueThreadChannel<AppEvent>
{
public:
	UiAppDoubleQueueThreadChannel();
	~UiAppDoubleQueueThreadChannel();

	AppEvent* takeFromUi();
	void sendToUi(AppEvent* event);
	AppEvent* takeFromApp();
	void sendToApp(AppEvent* event);
};
