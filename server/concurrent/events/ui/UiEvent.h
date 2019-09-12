#pragma once

enum EventType
{
	ListSessions,
	BeginInteraction,
	UpdateListSessions,
	AppendNewClient,
};

class UiEvent
{
public:
	enum EventLocation
	{
		MainGuiEvent,
		CameraGuiEvent,
		DesktopGuiEvent
	};


	UiEvent(EventType eventType);
	UiEvent(EventType event, void* data);
	~UiEvent();

	EventType getType() const
	{
		return eventType;
	}

	void* getData();

private:
	EventType eventType;
	void* data;
};
