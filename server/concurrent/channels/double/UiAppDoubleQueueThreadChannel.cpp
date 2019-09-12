#include "UiAppDoubleQueueThreadChannel.h"
#include "../single/UiAppQueuedThreadChannel.h"

UiAppDoubleQueueThreadChannel::UiAppDoubleQueueThreadChannel()
	: DoubleQueueThreadChannel<AppEvent>(new UiAppQueuedThreadChannel, new UiAppQueuedThreadChannel)
{
}

UiAppDoubleQueueThreadChannel::~UiAppDoubleQueueThreadChannel()
{
}

AppEvent* UiAppDoubleQueueThreadChannel::takeFromUi()
{
	// return takeFromLeft();
	return takeFromRight();
}

void UiAppDoubleQueueThreadChannel::sendToUi(AppEvent* event)
{
	// submitToRight(event);
	submitToLeft(event);
}

AppEvent* UiAppDoubleQueueThreadChannel::takeFromApp()
{
	// return takeFromRight();
	return takeFromLeft();
}

void UiAppDoubleQueueThreadChannel::sendToApp(AppEvent* event)
{
	// submitToLeft(event);
	submitToRight(event);
}
