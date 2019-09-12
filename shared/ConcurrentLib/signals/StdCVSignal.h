#pragma once
#include "ISignal.h"
#include <mutex>

class StdCVSignal : public ISignal
{
public:
	
	StdCVSignal();
	~StdCVSignal();
	virtual bool wait() override;
	virtual bool emit() override;
	std::mutex mutex_;
};

