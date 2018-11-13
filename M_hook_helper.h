#pragma once

#include <API/ARK/Ark.h>
#include <vector>

bool M_hook_helper_init();
bool M_hook_helper_done();

struct HookInfo {
	std::string entity, method;
	void *hook, *original;
};

typedef std::map<std::string, HookInfo> HookList;

template <typename T>
void registerHook(HookList *hookList, std::string entity, std::string method, LPVOID hook, T** original) {
	auto info = HookInfo{ entity, method, hook, original };
	std::string name = entity + "." + method;
	hookList->insert(std::pair<std::string, HookInfo>(name, info));
	ArkApi::GetHooks().SetHook(name, (LPVOID)hook, original);
}

void unregisterHook(HookList *hookList, std::string entity, std::string method);

#define __cc4(a, b, c, d) a ## b ## c ## d

#define __cc_hook(e, m) __cc4(Hook_, e, _, m)
#define __cc_original(e, m) __cc4(e, _, m, _original)

#define __register_hook(entity, method) registerHook(activeHooks, #entity, #method, & __cc_hook(entity, method), & __cc_original(entity, method))
#define __unregister_hook(entity, method) unregisterHook(activeHooks, #entity, #method)

struct CmdInfo {
	const std::string cmdName;
};

typedef std::vector<CmdInfo > CmdList;

extern HookList *activeHooks;
extern CmdList *activeCmd;

typedef void (*RCONCallback)(RCONClientConnection *, RCONPacket *, UWorld *);
void __register_rcon_command(const std::string cmdName, RCONCallback cmd);

void SendRconReply(RCONClientConnection* rcon_connection, int packet_id, const FString& msg);
