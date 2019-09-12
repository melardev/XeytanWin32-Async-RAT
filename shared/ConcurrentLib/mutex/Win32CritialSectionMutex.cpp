#include "stdafx.h"
#include "Win32CritialSectionMutex.h"

Win32CritialSectionMutex::Win32CritialSectionMutex()
{
}

Win32CritialSectionMutex::~Win32CritialSectionMutex()
{
}

void Win32CritialSectionMutex::lock()
{
	EnterCriticalSection(&mCriticalSection);
}

void Win32CritialSectionMutex::unlock()
{
	LeaveCriticalSection(&mCriticalSection);
}
