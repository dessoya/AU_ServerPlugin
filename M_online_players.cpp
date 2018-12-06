#include "M_online_players.h"
#include "Messages.h"
#include "EventMessage.h"
#include <API/UE/Math/ColorList.h>

OnlinePlayers *onlinePlayers = 0;

void _t_collect_online_and_position(TimerContext *tctx) {
	if (onlinePlayers) {
		onlinePlayers->_t_collect_online_and_position(tctx);
	}
}

bool M_online_players_init() {

	onlinePlayers = new OnlinePlayers();

	addTimer(_t_collect_online_and_position);

	return true;
}

bool M_online_players_done() {
	delete onlinePlayers;
	onlinePlayers = 0;
	return true;
}

OnlinePlayers::OnlinePlayers() {

	auto world = ArkApi::GetApiUtils().GetWorld();
	if (!world) return;


	auto player_controllers = world->PlayerControllerListField();
	try {
		for (TWeakObjectPtr<APlayerController> player_controller : player_controllers) {
			AShooterPlayerController* shooter_pc = static_cast<AShooterPlayerController*>(player_controller.Get());
			if (shooter_pc) {
				FString s2;
				auto steamId = shooter_pc->GetUniqueNetIdAsString(&s2);
				auto sid = stoull(steamId->ToString());
				insertPlayer(shooter_pc, sid);
			}
		}

		auto mode = ArkApi::GetApiUtils().GetShooterGameMode();
		if (mode) {
			mode->RemoveInactivePlayersAndTribes();
			auto tids = mode->TribesIdsField();
			auto tidsarr = tids.Array();
			for (int i = 0, m = tidsarr.Max(); i < m; i++) {
				auto tid = tidsarr[i];
				// Log::GetLog()->info("[*] enum tribe id {}", tid);

				
				// FTribeData td;
				// sizeof(td);

				FTribeData* tribe_data = static_cast<FTribeData*>(FMemory::Malloc(0x128 + 0x28));
				RtlSecureZeroMemory(tribe_data, 0x128 + 0x28);

				if (ArkApi::GetApiUtils().GetShooterGameMode()->GetOrLoadTribeData(tid, tribe_data)) {

				// if (mode->GetOrLoadTribeData(tid, &td)) {
					// td.b
					// Log::GetLog()->info("[*] enum tribe name {}", tribe_data->TribeNameField().ToString());
					/*
					auto td2 = mode->GetTribeData(&td, tid);
					if (td2) {
					}
					*/
					
				}

				FMemory::Free(tribe_data);
			}
		}
		
	}
	catch (...) {
		Log::GetLog()->info("[*] can't read player controller list");
	}
}

OnlinePlayers::~OnlinePlayers() {
	auto it = playerIdMap.begin();
	while (it != playerIdMap.end()) {
		delete it->second;
		it++;
	}
}

unsigned int OnlinePlayers::count() {
	return (unsigned int)playerIdMap.size();
}

void OnlinePlayers::erase(unsigned long long id) {

	auto it = playerIdMap.find(id);
	if (it != playerIdMap.end()) {
		delete it->second;
		playerIdMap.erase(id);
	}	
}

void OnlinePlayers::insertPlayer(AShooterPlayerController *shooter_pc, unsigned long long steamId) {

	// delete old steamId record
	{
		auto it = playerIdMap.begin();
		while (it != playerIdMap.end()) {
			if (it->second->steamId == steamId) {
				delete it->second;
				playerIdMap.erase(it);
				break;
			}
			it++;
		}
	}

	auto id = shooter_pc->LinkedPlayerIDField();
	if (id == 0) return;
	
	auto info = playerIdMap[id];

	if (info == 0) {

		auto state = shooter_pc->PlayerStateField();
		AShooterPlayerState *shstate = 0;
		unsigned long long tid = 0;
		if (state) {
			shstate = static_cast<AShooterPlayerState *>(shooter_pc->PlayerStateField());
			if (shstate) {
				tid = shstate->MyPlayerDataField()->MyDataField()->TribeIDField();
				Log::GetLog()->info("[*] OnlinePlayers track pid {} tid {}", id, tid);
			}
		}

		FVector pos = shooter_pc->DefaultActorLocationField();
		const auto now = std::chrono::system_clock::now();
		info = new OnlinePlayerInfo(shooter_pc, id, steamId, now, pos.X, pos.Y, pos.Z);
		info->tid = tid;
		playerIdMap[id] = info;

		/*
		if (tid == 0) {
			auto world = ArkApi::GetApiUtils().GetWorld();
			// world->Team
			Log::GetLog()->info("[*] pid {} try to add to tribe", id);
			auto mode = ArkApi::GetApiUtils().GetShooterGameMode();
			if (mode) {
				auto newtid = mode->GenerateTribeId();
				FString tribeName = FString::Format("ttgghf1 {0}", newtid);
				// tribeName.Format("ttgghf1 %d", newtid);
				Log::GetLog()->info("[*] generate tribe name {}", tribeName.ToString());

				Log::GetLog()->info("[*] GenerateTribeId {}", newtid);
				auto tid_ = mode->ForceCreateTribe(&tribeName, newtid);
				Log::GetLog()->info("[*] ForceCreateTribe {}", tid_);
				if (shstate) {
					auto fadd = mode->ForceAddPlayerToTribe(shstate, &tribeName);					
					Log::GetLog()->info("[*] ForceAddPlayerToTribe {}", fadd);

					FTribeData* tribe_data = static_cast<FTribeData*>(FMemory::Malloc(0x128 + 0x28));
					RtlSecureZeroMemory(tribe_data, 0x128 + 0x28);

					if (mode->GetOrLoadTribeData(newtid, tribe_data)) {

						// if (mode->GetOrLoadTribeData(tid, &td)) {
							// td.b
						Log::GetLog()->info("[*] enum tribe name {}", tribe_data->TribeNameField().ToString());
						tribe_data->OwnerPlayerDataIDField() = id;
						tribe_data->TribeAdminsField().Add(id);
						mode->UpdateTribeData(tribe_data);
						// tribe_data->
						// tribe_data->TribeGovernmentField().

					}

					FMemory::Free(tribe_data);
				}
			}
		}
		*/

	}
}

void OnlinePlayers::iterate(void *ctx, void **funcs) {

	auto it = playerIdMap.begin();
	while (it != playerIdMap.end()) {
		auto b = funcs;
		while (*b) {
			auto f = (PlayerIterator)*b;
			f(ctx, it->second);
			b++;
		}
		it++;
	}
}
void OnlinePlayers::_t_collect_online_and_position(TimerContext *tctx) {

	_msg_m_server_ping(count());


	auto it = playerIdMap.begin();
	while (it != playerIdMap.end()) {

		auto info = it->second;
		FVector pos = info->shooterPlayer->DefaultActorLocationField();

		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - info->last_time);
		auto ms = (unsigned int)diff.count();

		int activity_id;
		if (info->x == pos.X && info->y == pos.Y && info->z == pos.Z) {
			activity_id = pa_pasive_online;
			_msg_m_player_pasive_online(info->id, ms);
		}
		else {
			activity_id = pa_active_online;
			info->x = pos.X;
			info->y = pos.Y;
			info->z = pos.Z;

			_msg_m_player_set_pos_and_activity(info->id, ms, activity_id, &pos);

		}

		auto character = info->shooterPlayer->CharacterField();		
		if (character) {
			auto ct = __getClassType(character);
			// Log::GetLog()->info("[*] player got character {}", ct);
			if(ct == ot_APrimalDinoCharacter) {
				// Log::GetLog()->info("[*] player riding");
				auto rd = static_cast<APrimalDinoCharacter *>(character);
				auto dragged = rd->CarriedCharacterField().Get();
				if (dragged) {
					// Log::GetLog()->info("[*] riding dino drag");
					auto dt = __getClassType(dragged);
					if (dt == ot_APrimalDinoCharacter) {
						auto dino_ = static_cast<APrimalDinoCharacter *>(dragged);
						auto dinoName = _bp(dino_);
						// Log::GetLog()->info("[*] dragged dino {}", _bp(dino_).ToString());
						if (!dino_->IsCurrentlyPlayingAttackAnimation()) {
							if (dinoName.ToString() == "Ankylo_Character") {
								// dino_->bDisableHarvesting().Get()
								dino_->bTamedWanderHarvest().Set(true);
								dino_->DoAttack(0, true, true);
							}
							else {
								dino_->DoAttack(0, true, true);
							}
						}
					}
				}
			}
		}

		/*
		auto state = info->shooterPlayer->PlayerStateField();
		AShooterPlayerState *shstate = 0;
		unsigned long long tid = 0;
		if (state) {
			shstate = static_cast<AShooterPlayerState *>(info->shooterPlayer->PlayerStateField());
			if (shstate) {
				tid = shstate->MyPlayerDataField()->MyDataField()->TribeIDField();
				// Log::GetLog()->info("[*] OnlinePlayers track pid {} tid {}", id, tid);
			}
		}

		if (tid == 0) {
			auto world = ArkApi::GetApiUtils().GetWorld();
			Log::GetLog()->info("[*] pid {} try to add to tribe", info->id);
			auto mode = ArkApi::GetApiUtils().GetShooterGameMode();
			if (mode) {
				auto newtid = mode->GenerateTribeId();
				info->tid = newtid;
				FString tribeName = FString::Format("ttgghf1 {0}", newtid);
				// tribeName.Format("ttgghf1 %d", newtid);
				Log::GetLog()->info("[*] generate tribe name {}", tribeName.ToString());

				Log::GetLog()->info("[*] GenerateTribeId {}", newtid);
				auto tid_ = mode->ForceCreateTribe(&tribeName, newtid);
				Log::GetLog()->info("[*] ForceCreateTribe {}", tid_);
				if (shstate) {
					auto fadd = mode->ForceAddPlayerToTribe(shstate, &tribeName);
					Log::GetLog()->info("[*] ForceAddPlayerToTribe {}", fadd);

					FTribeData* tribe_data = static_cast<FTribeData*>(FMemory::Malloc(0x128 + 0x28));
					RtlSecureZeroMemory(tribe_data, 0x128 + 0x28);

					if (mode->GetOrLoadTribeData(newtid, tribe_data)) {

						Log::GetLog()->info("[*] enum tribe name {}", tribe_data->TribeNameField().ToString());
						tribe_data->OwnerPlayerDataIDField() = info->id;
						tribe_data->TribeAdminsField().Add(info->id);
						mode->UpdateTribeData(tribe_data);

						FString fs1("Force add to tribe");
						info->shooterPlayer->ClientServerChatDirectMessage(&fs1, FColorList::Red, false);
					}

					FMemory::Free(tribe_data);
				}
			}
		}
		*/

		info->last_time = cur_time;
		Log::GetLog()->info("[*] update player activity {} id {} ms {}", activity_id, info->id, ms);

		it++;
	}

}

/*


11/10/18 19:40 [ExtendedEventLog][info] [*] Harvest pid 667705515 q 40 name PrimalItemConsumable_Berry_Amarberry
11/10/18 19:40 [ExtendedEventLog][info]
11/10/18 19:40 [ExtendedEventLog][info] [*] update player activity 1 id 667705515 ms 1008
11/10/18 19:40 [ExtendedEventLog][info] __timer e:1008ms w:0ms
11/10/18 19:40 [ExtendedEventLog][info] [*] update player activity 1 id 667705515 ms 1030
11/10/18 19:40 [ExtendedEventLog][info] __timer e:1030ms w:1ms
11/10/18 19:40 [ExtendedEventLog][info] [*] update player activity 1 id 667705515 ms 1001
11/10/18 19:40 [ExtendedEventLog][info] __timer e:1001ms w:1ms

*/