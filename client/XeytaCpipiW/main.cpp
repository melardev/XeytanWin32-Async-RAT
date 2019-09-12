#include "Application.h"

#pragma comment (lib, "Wbemuuid.lib")

Application app;

void testSome()
{
	
}

// int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
int main()
{
	// Main Thread: gui -> mediator -> queues Event to Communicator Queue
	// Background Thread: Communicator takes event dispatches it to App
	// App takes event and submits task to ThreadPool to free the Queue Thread as 
	// quick as possible
	testSome();
	return app.run();
}
