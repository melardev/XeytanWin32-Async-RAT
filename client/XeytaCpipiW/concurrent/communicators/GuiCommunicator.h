#pragma once

#include <queue>
#include "../events/GuiEvent.h"

class GuiCommunicator
{
public:
	GuiCommunicator();
	~GuiCommunicator();
	GuiEvent& takeFromGui();

	std::queue<GuiEvent> guiEventsQueue;
};
