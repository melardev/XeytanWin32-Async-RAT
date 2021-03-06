#include "models/CommandLineArguments.h"
#include "Application.h"
#include <iostream>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "rpcrt4.lib")



void testany()
{
	std::wstring result;
	std::vector<std::wstring> fsHistory;
	fsHistory.push_back(L"C:\\");
	fsHistory.push_back(L"workspace");
	fsHistory.push_back(L"cpp\\3party");

	for (const auto& pathStr : fsHistory)
		result.append(pathStr + L"\\");

	std::wcout << result;
}


// int APIENTRY wWinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance,_In_ LPWSTR lpCmdLine,_In_ int nCmdShow)


int main()
{
	testany();
	// CommandLineArguments parsing is not implemented yet, but this is how it should look like in the future
	const CommandLineArguments cmd;
	Application app(cmd);
	app.run();
	return 0;
}
