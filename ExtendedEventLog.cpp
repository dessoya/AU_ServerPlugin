#include <API/ARK/Ark.h>
#include <fcntl.h>
#include "windows.h"
#include <queue>
#include <map>
#include <fstream>

#pragma comment(lib, "ArkApi.lib")

#include "EventMessage.h"
#include "Messages.h"
#include "Utils.h"

#include "M_config_reader.h"
#include "M_console_helper.h"
#include "M_event_writer.h"
#include "M_hook_helper.h"
#include "M_storage.h"

#include "M_timer.h"

#include "M_harvest_marker.h"
#include "M_online_players.h"

#include "M_player_common_activity.h"
#include "M_structures.h"
#include "M_inventory_and_harvest.h"
#include "M_tribe.h"

// int ForceCreateTribe(FString * TribeName, int TeamOverride) { return NativeCall<int, FString *, int>(this, "AShooterGameMode.ForceCreateTribe", TribeName, TeamOverride); }


void __rcon_TPDinoIdToPlayerId(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld*) {
	FString msg = rcon_packet->Body;

	TArray<FString> parsed;
	msg.ParseIntoArray(parsed, L" ", true);

	Log::GetLog()->info("__rcon_TPDinoIdToPlayerId");

	if (parsed.IsValidIndex(3)) {
		Log::GetLog()->info("got arguments");
		auto a1 = parsed[0];
		auto a2 = parsed[1];
		auto a3 = parsed[2];
		auto a4 = parsed[3];
		bool parseError = false;
		unsigned long long playerId;
		unsigned long long dinoid1;
		unsigned long long dinoid2;
		try {
			dinoid1 = std::stoull(a2.ToString());
			dinoid2 = std::stoull(a3.ToString());
			playerId = std::stoull(a4.ToString());
		}
		catch (...) {
			parseError = true;
		}
		if (parseError) {
			Log::GetLog()->info("syntax: TPDinoIdToPlayerId dinoid1, dinoid2, playerId");
		}
		else {
			Log::GetLog()->info("{} {} {}", dinoid1, dinoid2, playerId);
			auto info = onlinePlayers->get(playerId);
			if (info) {
				Log::GetLog()->info("got player info");

				APrimalDinoCharacter* dino = APrimalDinoCharacter::FindDinoWithID(ArkApi::GetApiUtils().GetWorld(), (unsigned int)dinoid1, (unsigned int)dinoid2);
				if (dino) {
					Log::GetLog()->info("got dino");
					// info->
					// check dino teamid
					FVector ppos = info->shooterPlayer->DefaultActorLocationField();
					
					FVector pos{ ppos.X + 1, ppos.Y + 1, ppos.Z + 1 };
					FRotator rot{ 0, 0, 0 };

					dino->TeleportTo(&pos, &rot, true, false);

					dino->TargetingTeamField() = info->tid;					

				}

				/*
				TArray<AActor*> found_actors;
				UGameplayStatics::GetAllActorsOfClass(reinterpret_cast<UObject*>(ArkApi::GetApiUtils().GetWorld()),
					APrimalDinoCharacter::GetPrivateStaticClass(), &found_actors);
				for (AActor* actor : found_actors) {
					APrimalDinoCharacter* dino = static_cast<APrimalDinoCharacter*>(actor);
					if (!dino) continue;
					const int dino_team = dino->TargetingTeamField();
					if (dino_team == info->tid &&  dino->DinoID1Field() == dinoid1 && dino->DinoID2Field() == dinoid2) {
						// let's tp

						break;
					}
				}
				*/
			}
		}
	}
	SendRconReply(rcon_connection, rcon_packet->Id, "ok");
}

void __rcon_GetTribeIdFromPlayerId(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld*) {

	FString msg = rcon_packet->Body;

	TArray<FString> parsed;
	msg.ParseIntoArray(parsed, L" ", true);

	Log::GetLog()->info("__rcon_GetTribeIdFromPlayerId");

	if (parsed.IsValidIndex(1)) {
		auto a1 = parsed[0];
		auto a2 = parsed[1];
		// Log::GetLog()->info("{}, {}", a1.ToString(), a2.ToString());
		bool parseError = false;
		unsigned long long playerId;
		try {
			playerId = std::stoull(a2.ToString());
		}
		catch (...) {
			parseError = true;
		}
		if (parseError) {
			Log::GetLog()->info("syntax: GetTribeIdFromPlayerId playerId");
		}
		else {
			Log::GetLog()->info("playerId {}", playerId);

			TArray<AActor*> found_actors;
			UGameplayStatics::GetAllActorsOfClass(reinterpret_cast<UObject*>(ArkApi::GetApiUtils().GetWorld()),
				APrimalDinoCharacter::GetPrivateStaticClass(), &found_actors);

			auto maxdinos = found_actors.Max();

			auto info = onlinePlayers->get(playerId);
			if (info) {
				Log::GetLog()->info("tid {} maxdinos {}", info->tid, maxdinos);

				// info->shooterPlayer->TargetingTeamField();

				const int player_team = info->shooterPlayer->TargetingTeamField();

				for (AActor* actor : found_actors) {
					APrimalDinoCharacter* dino = static_cast<APrimalDinoCharacter*>(actor);
					if (!dino) continue;


					const int dino_team = dino->TargetingTeamField();
					if (dino_team == player_team) {
						FString dino_name;
						dino->GetDinoDescriptiveName(&dino_name);
						auto dino_name_utf = ArkApi::Tools::Utf8Encode(*dino_name);
						Log::GetLog()->info("dino {} {}:{} team {}", dino_name_utf.c_str(), dino->DinoID1Field(), dino->DinoID2Field(), dino_team);
					}
					/*
				const int dino_team = dino->TargetingTeamField();
				if (dino_team == player_team)
				{
					FString dino_name;
					dino->GetDinoDescriptiveName(&dino_name);

					reply += FString::Format(TEXT("{}, ID1={}, ID2={}\n"), *dino_name, dino->DinoID1Field(), dino->DinoID2Field());
				}
				*/
				}
			}

		}
	}
	
	SendRconReply(rcon_connection, rcon_packet->Id, "ok");
}







typedef bool (*void_callback)();
struct ModuleHeader {
	std::string name;
	void_callback init, done;
};

ModuleHeader modulesList[] = {

	// core submodules

	{ "ConfigReader", M_config_reader_init, M_config_reader_done },
	{ "EventWriter", M_event_writer_init, M_event_writer_done },
	{ "HookHelper", M_hook_helper_init, M_hook_helper_done },

	{ "ConsoleHelper", M_console_helper_init, M_console_helper_done },

	// timer subsystem

	{ "Timer", M_timer_init, M_timer_done },

	// helpers

	{ "Storage", M_storage_init, M_storage_done },
	{ "HarvestMarker", M_harvest_marker_init, M_harvest_marker_done },
	{ "OnlinePlayers", M_online_players_init, M_online_players_done },

	// high level submodules

	{ "PlayerCommonActivity", M_player_common_activity_init, M_player_common_activity_done },
	{ "Structures", M_structures_init, M_structures_done },
	{ "InventoryAndHarvest", M_inventory_and_harvest_init, M_inventory_and_harvest_done },
	{ "Tribe", M_tribe_init, M_tribe_done },

};

int modulesCount = sizeof(modulesList) / sizeof(ModuleHeader);
bool modulesInited;

void Load() {

	Log::Get().Init("ExtendedEventLog");

	modulesInited = false;
	Log::GetLog()->info("[*] Init modules");
	for (int i = 0; i < modulesCount; i++) {
		auto h = modulesList[i];
		Log::GetLog()->info("[+] + {}", h.name.c_str());
		if (h.init) {
			if (!h.init()) {
				Log::GetLog()->info("[!] Failed -> rollback initialization ...");
				i--;
				while (i >= 0) {
					h = modulesList[i];
					Log::GetLog()->info("[-] - {}", h.name.c_str());
					if (h.done) h.done();
					i--;
				}
				return;
			}
		}
	}

	modulesInited = true;

	EventMessage *m = new EventMessage();
	m->push_array(1);
	m->push_uint(m_server_start);
	m->set_size();
	eventWriter->push(m);

	_msg_server_config();

	{
		auto it = onlinePlayers->playerIdMap.begin();
		while (it != onlinePlayers->playerIdMap.end()) {
			auto oi = it->second;

			EventMessage *m = new EventMessage();
			m->push_array(2);
			m->push_uint(m_player_online);
			m->push_uint(oi->id);
			m->set_size();
			eventWriter->push(m);

			it++;
		}
	}
	
	__register_rcon_command("GetTribeIdFromPlayerId", &__rcon_GetTribeIdFromPlayerId);
	__register_rcon_command("TPDinoIdToPlayerId", &__rcon_TPDinoIdToPlayerId);
}

void Unload() {
	if (modulesInited) {
		Log::GetLog()->info("[*] Done modules");
		for (int i = modulesCount - 1; i >= 0; i--) {
			auto h = modulesList[i];
			Log::GetLog()->info("[-] - {}", h.name.c_str());
			if (h.done) h.done();
		}
	}	
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH: Load(); break;
		case DLL_PROCESS_DETACH: Unload(); break;
	}
	return TRUE;
} 