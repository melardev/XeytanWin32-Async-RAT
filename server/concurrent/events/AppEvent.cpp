#include "AppEvent.h"
#include <iostream>
#include <string>

AppEvent::AppEvent()
{
}

AppEvent::AppEvent(Target target, Subject subject, Action action)
	: target(target), subject(subject), action(action)
{
}

AppEvent::AppEvent(Target target, Subject subject, Action action, void* data)
	: target(target), subject(subject), action(action), object(data)
{
}

AppEvent::AppEvent(Target target, Subject subject, const char* message)
	: target(target), subject(subject), description(message)
{
}

AppEvent::AppEvent(Target target)
	: target(target), subject(Subject::InvalidSubject), action(Action::InvalidAction)
{
}

AppEvent::~AppEvent()
{
	std::cout
		<< "AppEvent destructor called; "
		<< targetStringMap[target] << " " << subjectStringMap[subject] << " " << actionStringMap[action]
		<< std::endl;
}
