#pragma once
#include "../../../shared/ConsoleLib/Console.h"
#include <initializer_list>
#include <xstring>
#include "../../appdefs.h"
#include <vector>
#include "../../../shared/NetLib/packets/packetdefs.h"
#include <vector>
#include <vector>
#include <stack>


class Client;

enum class View
{
	Main,
	About,

	ClientInteraction,
	Filesystem,
	RemoteDesktop,
	ReverseShell,
	Camera,

	MinInteraction = ClientInteraction,
	MaxInteraction = Camera,
};

class AppConsole : public Console
{
public:
	AppConsole();
	~AppConsole();

	void showMainUi();
	bool handleInstruction(char* instruction);
	void getLine(char* buffer, unsigned bufferLen);
	View getCurrentView();
	void printErrorLine(const std::string& errorMessage) const;
	void printErrorLine(const char* errorMessage) const;
	void printFsPrompt();
	void printPrompt();
	void printHelp();
	void printCommands();
	void println(const std::string& message, bool showPrompt = false);
	void printf(const char* strFormat, int count,...);
	std::string getPromptMessage(bool withSuffix = true);
	void printf(bool showPrompt, const char* strFormat, ...);

	void updateListSessions(void* get_data);
	void trace(const char* str);
	void printClients(std::vector<Client*>* clients);
	Client* getClient();
	void setInteractionView(Client* client);
	void showProcesses(Client* client, std::vector<Process>* processes);
	void showFsRoots(Client* client, const std::vector<FileInfo>& files);
	void showFsFiles(Client* client, const std::wstring& path, const std::vector<FileInfo>& files);
	bool isInteracting();
	std::wstring getCurrentPath();
	void printBanner();

	View currentView;
	unsigned long long clientId;
	Client* client;
	std::vector<std::wstring> fsHistory;
};
