#include "stdafx.h"
#include "StdCVSignal.h"
#include <mutex>


StdCVSignal::StdCVSignal()
{
}


StdCVSignal::~StdCVSignal()
{
}

bool StdCVSignal::wait()
{
	// cv.notify_one();
	// cv.wait(lock);
	return true;
}

bool StdCVSignal::emit()
{
	//  cv.notify_one(); // Notify worker
	//  cv.wait(lk);
	return true;
}
