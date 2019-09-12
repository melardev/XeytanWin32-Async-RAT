#pragma once
#include "../../events/ui/UiEvent.h"

class IUiThreadChannel 
{
public:
	IUiThreadChannel();
	virtual ~IUiThreadChannel();

	virtual void submitUiEvent(UiEvent* event) = 0;
	virtual UiEvent* takeUiEvent() = 0;
};
