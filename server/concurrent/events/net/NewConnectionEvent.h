#pragma once
#include "NetEvent.h"
#include <memory>


class NewConnectionEvent : public NetEvent
{
public:

	NewConnectionEvent() = default;
	virtual ~NewConnectionEvent() = default;
};
