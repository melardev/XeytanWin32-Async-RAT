
#include "UiDoubleQueueThreadChannel.h"
#include "../single/UiQueuedThreadChannel.h"
#include "../../events/ui/UiEvent.h"


UiDoubleQueueThreadChannel::UiDoubleQueueThreadChannel()
	: DoubleQueueThreadChannel<UiEvent>(new UiQueuedThreadChannel, new UiQueuedThreadChannel)
{
}

UiEvent* UiDoubleQueueThreadChannel::takeFromUi()
{
	return takeFromLeft();
}

void UiDoubleQueueThreadChannel::sendToUi(UiEvent* event)
{
	submitToRight(event);
}

UiEvent* UiDoubleQueueThreadChannel::takeFromApp()
{
	return takeFromRight();
}

void UiDoubleQueueThreadChannel::sendToApp(UiEvent* event)
{
	submitToLeft(event);
}
