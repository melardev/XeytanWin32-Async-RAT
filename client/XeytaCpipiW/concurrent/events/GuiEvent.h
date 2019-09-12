#pragma once
class GuiEvent
{
public:
	enum EventType
	{
		MainGuiEvent,
		CameraGuiEvent,
		DesktopGuiEvent
	};

	GuiEvent();
	~GuiEvent();

	EventType getType() const
	{
		return m_type;
	}
private:
	EventType m_type;
};

