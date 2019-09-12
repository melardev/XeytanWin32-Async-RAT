#pragma once
#include <Windows.h>
#include <string>
#include "../../../shared/NetLib/packets/packetdefs.h"
#include <vector>

class ProcessService
{
public:
	ProcessService();
	~ProcessService();

	DWORD FindBrocessByName(const std::wstring& processName);

	bool killProcessByName(const std::wstring& processName);

	bool killProcessById(DWORD processId);

	bool isProcessRunning(DWORD pid);


	bool isRunAsAdmin() throw();

	BOOL isProcessElevated() throw();
	static void getProcessList(std::vector<Process>& processes);
};
