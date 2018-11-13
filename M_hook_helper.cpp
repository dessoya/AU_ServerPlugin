#include "M_hook_helper.h"


HookList *activeHooks = 0;
CmdList *activeCmd = 0;

bool M_hook_helper_init() {

	activeHooks = new HookList;
	activeCmd = new CmdList;

	return true;
}

bool M_hook_helper_done() {

	{
		auto it = activeHooks->begin();
		while (it != activeHooks->end()) {
			std::string name = it->second.entity + "." + it->second.method;
			ArkApi::GetHooks().DisableHook(name, it->second.hook);
			it++;
		}
	}
	delete activeHooks;


	auto& commands = ArkApi::GetCommands();
	{
		auto it = activeCmd->begin();
		while (it != activeCmd->end()) {			
			commands.RemoveRconCommand(it->cmdName.c_str());
			it++;
		}
	}
	delete activeCmd;

	return true;
}

void SendRconReply(RCONClientConnection* rcon_connection, int packet_id, const FString& msg) {
	FString reply = msg + "\n";
	rcon_connection->SendMessageW(packet_id, 0, &reply);
}

void __register_rcon_command(const std::string cmdName, RCONCallback cmd) {

	auto info = CmdInfo{ cmdName };
	activeCmd->push_back(info);
	auto& commands = ArkApi::GetCommands();
	commands.AddRconCommand(cmdName.c_str(), cmd);
}

void unregisterHook(HookList *hookList, std::string entity, std::string method) {
	std::string name = entity + "." + method;
	auto it = hookList->find(name);
	if (it != hookList->end()) {
		ArkApi::GetHooks().DisableHook(name, it->second.hook);
		hookList->erase(name);
	}
}