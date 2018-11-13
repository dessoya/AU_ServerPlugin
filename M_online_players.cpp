#include "M_online_players.h"
#include "Messages.h"
#include "EventMessage.h"

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
	// Log::GetLog()->info("[*] OnlinePlayers::insertPlayer");
	auto id = shooter_pc->LinkedPlayerIDField();
	if (id == 0) return;
	// Log::GetLog()->info("[*] OnlinePlayers::insertPlayer {}", id);

	

	auto state = shooter_pc->PlayerStateField();
	unsigned long long tid = 0;
	if (state) {
		auto shstate = static_cast<AShooterPlayerState *>(shooter_pc->PlayerStateField());
		if (shstate) {
			tid = shstate->MyPlayerDataField()->MyDataField()->TribeIDField();
			Log::GetLog()->info("[*] OnlinePlayers track pid {} tid {}", id, tid);
		}
	}

	/*
	PlayerOwner->MyPlayerDataField()->MyDataField()
	c->
	MyDataField
	FPrimalPlayerDataStruct
	TribeIDField()
	*/


	auto it = playerIdMap.find(id);
	if (it == playerIdMap.end()) {
		// Log::GetLog()->info("[*] OnlinePlayers::insertPlayer added");
		FVector pos = shooter_pc->DefaultActorLocationField();
		const auto now = std::chrono::system_clock::now();
		OnlinePlayerInfo *info = new OnlinePlayerInfo(shooter_pc, id, steamId, now, pos.X, pos.Y, pos.Z);
		info->tid = tid;
		playerIdMap.insert(std::pair<unsigned long long, OnlinePlayerInfo *>(id, info));
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