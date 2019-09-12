#include "GuiCommunicator.h"


GuiCommunicator::GuiCommunicator()
{
}


GuiCommunicator::~GuiCommunicator()
{
}

GuiEvent& GuiCommunicator::takeFromGui()
{
	return guiEventsQueue.front();
}
