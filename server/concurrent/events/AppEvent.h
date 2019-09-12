#pragma once
#include <vector>
#include <map>
#include <string>

enum class Target
{
	InvalidTarget = -1,
	Application,
	Server,
	Client,
	Ui
};

enum class Subject
{
	InvalidSubject = -1,
	Interaction, // Used for console
	Connection,
	Desktop,
	Camera,
	Process,
	FileSystem,
	ReverseShell,
	Download,
	Information,
	PacketReceived,
	Error,
};

enum class Action
{
	InvalidAction = -1,
	ListAvailable,
	Start,
	Update,
	Pause,
	Stop
};

class AppEvent
{
public:
	AppEvent();
	AppEvent(Target target, Subject subject, Action action);
	AppEvent(Target target, Subject subject, Action action, void* data);
	AppEvent(Target target, Subject subject, const char* message);
	AppEvent(Target target);
	virtual ~AppEvent();

public:
	Target target;
	Subject subject;
	Action action;

	void* object;
	std::string description;

	std::map<Target, std::string> targetStringMap = {
		{Target::Server, "Target is server"},
		{Target::Client, "Target is Client"},
		{Target::Application, "Target is Application"},
	};

	std::map<Subject, std::string> subjectStringMap = {
		{Subject::InvalidSubject, "Subject is Invalid"},
		{Subject::Connection, "Subject is Connection"},
		{Subject::Information, "Subject is Information"},
		{Subject::Desktop, "Subject is Desktop"},
		{Subject::Camera, "Subject is Camera"},
	};

	std::map<Action, std::string> actionStringMap = {
		{Action::InvalidAction, "Action is Invalid"},
		{Action::Start, "Action is start"},
		{Action::Pause, "Action is Pause"},
		{Action::ListAvailable, "Action is ListAvailable"},
	};
};
