
#include "UiEvent.h"


UiEvent::UiEvent(EventType eventType): eventType(eventType)
{
}

UiEvent::UiEvent(EventType event, void* data):eventType(event), data(data)
{
}

UiEvent::~UiEvent()
{
}

void* UiEvent::getData()
{
	return data;
}
