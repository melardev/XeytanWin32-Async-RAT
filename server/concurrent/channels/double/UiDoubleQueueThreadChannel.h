#pragma once
#include "../../../../shared/ConcurrentLib/channels/double/DoubleQueueThreadChannel.h"
#include "../../events/ui/UiEvent.h"


class UiDoubleQueueThreadChannel : public DoubleQueueThreadChannel<UiEvent>
{
public:
	UiDoubleQueueThreadChannel();

	UiEvent* takeFromUi();
	void sendToUi(UiEvent* event);
	UiEvent* takeFromApp();
	void sendToApp(UiEvent* event);

};
