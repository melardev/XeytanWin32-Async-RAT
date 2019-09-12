#include "stdafx.h"
#include "Win32CVSignal.h"

// TODO: recheck this, I think I have to EnterCriticalSection before Sleep
// https://docs.microsoft.com/en-us/windows/desktop/sync/using-condition-variables
// My Idea is to Crete Win32CriticalSection Mutex, then use it as my mLock
// Because that mutex has to be used before reading some queue, then SleepCo.. will release the lock and again reown it when
// it is signaled
Win32CVSignal::Win32CVSignal(const Win32CritialSectionMutex mu): mMutex(mu)
{
	InitializeConditionVariable(&mConditionVariable);
}


Win32CVSignal::~Win32CVSignal()
{
}

bool Win32CVSignal::wait()
{
	mMutex.lock();
	SleepConditionVariableCS(&mConditionVariable, &mMutex.mCriticalSection, INFINITE);
	mMutex.unlock();
	return true;
}

bool Win32CVSignal::emit()
{

	WakeConditionVariable(&mConditionVariable);
	return true;
}
