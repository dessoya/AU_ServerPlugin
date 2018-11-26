#include "Messages.h"
#include "M_event_writer.h"
#include "M_timer.h"

void _msg_m_player_die(ull pid) {

	EventMessage *m = new EventMessage();
	m->push_array(3);

	m->push_uint(m_player_die);
	m->push_uint(__timestamp);

	m->push_uint(pid);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_player_learn_engram(ull pid, std::string engramName) {

	EventMessage *m = new EventMessage();
	m->push_array(4);

	m->push_uint(m_player_learn_engram);
	m->push_uint(__timestamp);
	m->push_uint(pid);
	eventWriter->push_string_to_message(m, engramName);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_player_update_inventory2(ull pid, UPrimalItem *item, int q) {
	

	auto name = _bp(item);
	auto req = item->BaseCraftingResourceRequirementsField();
	auto req2 = item->CraftingResourceRequirementsField();

	/*
	Log::GetLog()->info("inventory2 {} {} {} {} {}", req.Max(), req2.Max(),
		item->ResourceRequirementIncreaseRatingPowerField(),
		item->ResourceRequirementRatingIncreasePercentageField(),
		item->ResourceRequirementRatingScaleField()
		);
	for (int i = 0, sz = req2.Max(); i < sz; i++) {
		Log::GetLog()->info("req2 {}", req2[i]);
	}
	*/

	auto reqCount = req.Max();

	EventMessage *m = new EventMessage();
	m->push_array(7 + 8 + 1 + reqCount * 2);

	m->push_uint(m_player_update_inventory2);
	m->push_uint(__timestamp);
	m->push_uint(pid);
	eventWriter->push_string_to_message(m, name.ToString());
	m->push_int(q);
	m->push_uint(item->bIsBlueprint().Get() ? 1 : 0);
	m->push_uint(item->ItemQualityIndexField());

	int sz = 8;
	for (int idx = 0; idx < sz; idx++) {
		auto v = item->GetItemStatModifier((EPrimalItemStat::Type)idx) * 100;
		m->push_int((int)v);
	}

	m->push_uint(reqCount);

	for (int i = 0, sz = req.Max(); i < sz; i++) {
		auto r = req[i];
		auto t = _bp(r.ResourceItemType.uClass);
		auto v = req2[i] * 100;
		m->push_int((int)v);
		eventWriter->push_string_to_message(m, t.ToString());		
	}

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_player_update_inventory_with_quality(ull pid, std::string itemName, unsigned int isbp,
	std::string quality, unsigned int quality_index, std::string req, int q) {

	EventMessage *m = new EventMessage();
	m->push_array(9);

	m->push_uint(m_player_update_inventory_with_quality);
	m->push_uint(__timestamp);
	m->push_uint(pid);
	eventWriter->push_string_to_message(m, itemName);

	m->push_uint(isbp);
	eventWriter->push_string_to_message(m, quality);
	m->push_uint(quality_index);
	eventWriter->push_string_to_message(m, req);
	m->push_int(q);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_player_update_inventory(ull pid, std::string itemName, int q, int h, unsigned int isbp) {

	EventMessage *m = new EventMessage();
	m->push_array(7);

	m->push_uint(m_player_update_inventory);
	m->push_uint(__timestamp);
	m->push_uint(pid);
	eventWriter->push_string_to_message(m, itemName);

	m->push_int(q);
	m->push_uint(h);
	m->push_uint(isbp);


	m->set_size();
	eventWriter->push(m);
}

void _msg_m_player_harvest2(ull pid, std::string itemName, unsigned int q, unsigned int ht) {

	EventMessage *m = new EventMessage();
	m->push_array(6);

	m->push_uint(m_player_harvest2);
	m->push_uint(__timestamp);
	m->push_uint(pid);
	eventWriter->push_string_to_message(m, itemName);

	m->push_uint(q);
	m->push_uint(ht);

	m->set_size();
	eventWriter->push(m);

}

void _msg_m_build_structure(ull pid, std::string structureName, FVector *loc) {
	EventMessage *m = new EventMessage();

	m->push_array(7);

	m->push_uint(m_build_structure);

	m->push_uint(__timestamp);
	m->push_uint(pid);

	eventWriter->push_string_to_message(m, structureName);

	m->push_float(loc->X);
	m->push_float(loc->Y);
	m->push_float(loc->Z); // 8

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_player_set_pos(ull pid, FVector *loc) {
	EventMessage *m = new EventMessage();
	m->push_array(6);

	m->push_uint(m_player_set_pos);
	m->push_uint(__timestamp);
	m->push_uint(pid);


	m->push_float(loc->X);
	m->push_float(loc->Y);
	m->push_float(loc->Z);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_player_logout(ull pid, std::string steamId, std::string steamName, std::string name) {

	auto m = new EventMessage();

	m->push_array(6);

	m->push_uint(m_player_logout);

	m->push_uint(__timestamp);
	m->push_uint(stoull(steamId));
	m->push_string(steamName);
	m->push_uint(pid);
	m->push_string(name);

	m->set_size();
	eventWriter->push(m);

}

void _msg_m_player_login(ull pid, unsigned int messageType, ull sid, std::string steamName, std::string name) {
	EventMessage *m = new EventMessage();

	m->push_array(6);

	m->push_uint(messageType);	
	m->push_uint(__timestamp);
	m->push_uint(sid);
	m->push_string(steamName);
	m->push_uint(pid);
	m->push_string(name);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_server_ping(unsigned int onlinePlayersCount) {

	EventMessage *m = new EventMessage();
	m->push_array(3);

	m->push_uint(m_server_ping);
	m->push_uint(__timestamp);

	m->push_uint(onlinePlayersCount);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_player_pasive_online(ull pid, unsigned int ms) {

	EventMessage *m = new EventMessage();
	m->push_array(4);

	m->push_uint(m_player_pasive_online);
	m->push_uint(__timestamp);
	m->push_uint(pid);

	m->push_uint(ms);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_player_set_pos_and_activity(ull pid, unsigned int ms, unsigned int activity_id, FVector *loc) {


	EventMessage *m = new EventMessage();
	m->push_array(8);

	m->push_uint(m_player_set_pos_and_activity);
	m->push_uint(__timestamp);
	m->push_uint(pid);

	m->push_float(loc->X);
	m->push_float(loc->Y);
	m->push_float(loc->Z);

	m->push_uint(activity_id);
	m->push_uint(ms);

	m->set_size();
	eventWriter->push(m);
}


void _msg_m_tribe_create(ull tid, ull pid, std::string name) {

	EventMessage *m = new EventMessage();
	m->push_array(5);

	m->push_uint(m_tribe_create);
	m->push_uint(___timestamp());
	m->push_uint(tid);
	m->push_uint(pid);

	m->push_string(name);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_tribe_remove(ull tid) {

	EventMessage *m = new EventMessage();
	m->push_array(3);

	m->push_uint(m_tribe_remove);
	m->push_uint(___timestamp());
	m->push_uint(tid);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_tribe_log(ull tid, std::string message) {

	EventMessage *m = new EventMessage();
	m->push_array(4);

	m->push_uint(m_tribe_log);
	m->push_uint(___timestamp());
	m->push_uint(tid);

	m->push_string(message);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_tribe_remove_player(ull tid, ull pid) {

	EventMessage *m = new EventMessage();
	m->push_array(4);

	m->push_uint(m_tribe_remove_player);
	m->push_uint(___timestamp());
	m->push_uint(tid);
	m->push_uint(pid);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_tribe_add_player(ull tid, ull pid) {
	EventMessage *m = new EventMessage();
	m->push_array(4);

	m->push_uint(m_tribe_add_player);
	m->push_uint(___timestamp());
	m->push_uint(tid);
	m->push_uint(pid);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_tribe_change_name(ull tid, std::string name) {
	EventMessage *m = new EventMessage();
	m->push_array(4);

	m->push_uint(m_tribe_change_name);
	m->push_uint(___timestamp());
	m->push_uint(tid);
	m->push_string(name);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_tribe_set_owner(ull tid, ull pid) {
	EventMessage *m = new EventMessage();
	m->push_array(4);

	m->push_uint(m_tribe_set_owner);
	m->push_uint(___timestamp());
	m->push_uint(tid);
	m->push_uint(pid);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_player_all_stats(ull pid, UPrimalCharacterStatusComponent *s) {

	auto points = s->NumberOfLevelUpPointsAppliedField();
	auto values = s->MaxStatusValuesField();

	unsigned int l = (unsigned int)points.GetSize();
	unsigned int level = 0;

	EventMessage *m = new EventMessage();
	m->push_array(4 + l * 2);

	m->push_uint(m_player_all_stats);
	m->push_uint(__timestamp);
	m->push_uint(pid);

	for (unsigned int i = 0; i < l; i++) {
		auto point = points()[i];
		auto value = values()[i];
		level += point;

		m->push_uint((unsigned int)point);
		m->push_float(value);
	}

	m->push_uint(level);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_player_level_up(ull pid, unsigned short stat_id, unsigned short points, float value) {

	EventMessage *m = new EventMessage();
	m->push_array(6);

	m->push_uint(m_player_level_up);
	m->push_uint(___timestamp());
	m->push_uint(pid);

	m->push_uint(stat_id);
	m->push_uint(points);
	m->push_float(value);

	m->set_size();
	eventWriter->push(m);

}

void _msg_m_dino_knockdown(ull pid, ull tid, APrimalDinoCharacter *dino) {

	unsigned long long did = makeItemId(dino->DinoID1Field(), dino->DinoID2Field());

	// auto pos = dino->Control()->DefaultActorLocationField();
	// auto pos = dino->RootComponentField()->RelativeLocationField();
	
	auto name = _bp(dino);

	auto s = dino->MyCharacterStatusComponentField();
	auto points = s->NumberOfLevelUpPointsAppliedField();
	auto values = s->MaxStatusValuesField();

	unsigned int l = (unsigned int)points.GetSize();
	unsigned int level = 0;
	
	EventMessage *m = new EventMessage();
	m->push_array(10 + l * 2);

	m->push_uint(m_dino_knockdown);
	m->push_uint(___timestamp());
	m->push_uint(did);
	m->push_uint(pid);
	m->push_uint(tid);

	eventWriter->push_string_to_message(m, name.ToString());

	auto pos = dino->RootComponentField()->RelativeLocationField();
	m->push_float(pos.X);
	m->push_float(pos.Y);
	m->push_float(pos.Z);

	for (unsigned int i = 0; i < l; i++) {
		auto point = points()[i];
		auto value = values()[i];
		level += point;

		m->push_uint((unsigned int)point);
		m->push_float(value);
	}

	m->push_uint(level);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_dino_tame(ull pid, ull tid, APrimalDinoCharacter *dino) {

	unsigned long long did = makeItemId(dino->DinoID1Field(), dino->DinoID2Field());

	auto s = dino->MyCharacterStatusComponentField();
	auto points = s->NumberOfLevelUpPointsAppliedField();
	auto values = s->MaxStatusValuesField();

	unsigned int l = (unsigned int)points.GetSize();
	unsigned int level = 0;

	EventMessage *m = new EventMessage();
	m->push_array(6 + l * 2);

	m->push_uint(m_dino_tame);
	m->push_uint(___timestamp());
	m->push_uint(did);
	m->push_uint(pid);
	m->push_uint(tid);

	for (unsigned int i = 0; i < l; i++) {
		auto point = points()[i];
		auto value = values()[i];
		level += point;

		m->push_uint((unsigned int)point);
		m->push_float(value);
	}

	m->push_uint(level);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_dino_wake(APrimalDinoCharacter *dino) {

	unsigned long long did = makeItemId(dino->DinoID1Field(), dino->DinoID2Field());

	EventMessage *m = new EventMessage();
	m->push_array(3);

	m->push_uint(m_dino_wake);
	m->push_uint(___timestamp());
	m->push_uint(did);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_dino_die(APrimalDinoCharacter *dino) {

	unsigned long long did = makeItemId(dino->DinoID1Field(), dino->DinoID2Field());

	EventMessage *m = new EventMessage();
	m->push_array(3);

	m->push_uint(m_dino_die);
	m->push_uint(___timestamp());
	m->push_uint(did);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_dino_level_up(APrimalDinoCharacter *dino, ull pid, unsigned short stat_id) {

	unsigned long long did = makeItemId(dino->DinoID1Field(), dino->DinoID2Field());

	EventMessage *m = new EventMessage();
	m->push_array(7);

	m->push_uint(m_dino_level_up);
	m->push_uint(___timestamp());
	m->push_uint(did);
	ull tid = dino->TargetingTeamField();
	m->push_uint(tid);
	m->push_uint(pid);

	m->push_uint(stat_id);
	auto s = dino->MyCharacterStatusComponentField();
	auto c = s->MaxStatusValuesField();
	auto v = c()[stat_id];
	m->push_float(v);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_dino_spawn_wild(APrimalDinoCharacter *dino) {

	unsigned long long did = makeItemId(dino->DinoID1Field(), dino->DinoID2Field());

	auto s = dino->MyCharacterStatusComponentField();
	auto points = s->NumberOfLevelUpPointsAppliedField();
	auto values = s->MaxStatusValuesField();

	unsigned int l = (unsigned int)points.GetSize();
	unsigned int level = 0;

	EventMessage *m = new EventMessage();
	m->push_array(8 + l * 2);

	m->push_uint(m_dino_spawn_wild);
	m->push_uint(___timestamp());
	m->push_uint(did);
	auto name = _bp(dino);
	eventWriter->push_string_to_message(m, name.ToString());

	auto pos = dino->RootComponentField()->RelativeLocationField();
	m->push_float(pos.X);
	m->push_float(pos.Y);
	m->push_float(pos.Z);

	for (unsigned int i = 0; i < l; i++) {
		auto point = points()[i];
		auto value = values()[i];
		level += point;

		m->push_uint((unsigned int)point);
		m->push_float(value);
	}

	m->push_uint(level);

	m->set_size();
	eventWriter->push(m);
}

void _msg_m_dino_change_name(APrimalDinoCharacter *dino, std::string newName) {

	unsigned long long did = makeItemId(dino->DinoID1Field(), dino->DinoID2Field());

	EventMessage *m = new EventMessage();
	m->push_array(4);

	m->push_uint(m_dino_change_name);
	m->push_uint(___timestamp());
	m->push_uint(did);
	m->push_string(newName);

	m->set_size();
	eventWriter->push(m);
}

void _msg_server_config() {

	AShooterGameMode *GameMode = ArkApi::GetApiUtils().GetShooterGameMode();
	if (GameMode) {
		struct ServerConfigParam {
			float value;
			const char *name;
		};
		ServerConfigParam params[] = {
			{ GameMode->HarvestAmountMultiplierField(), "harvestAmount" },
			{ GameMode->HarvestHealthMultiplierField(), "harvestHealth" },
			{ GameMode->XPMultiplierField(), "XP" },
			{ GameMode->TamingSpeedMultiplierField(), "tamingSpeed" },
			{ GameMode->BabyMatureSpeedMultiplierField(), "babyMature" },
			{ GameMode->MateBoostEffectMultiplierField(), "mateBoost" },
			{ GameMode->MatingIntervalMultiplierField(), "matingInterval" },

			{ GameMode->MatingSpeedMultiplierField(), "matingSpeed" },
			{ GameMode->EggHatchSpeedMultiplierField(), "eggHatchSpeed" },
			{ GameMode->LayEggIntervalMultiplierField(), "layEggInterval" },
			{ GameMode->ResourcesRespawnPeriodMultiplierField(), "resourceRespawn" }
		};

		// GameMode->KickAllPlayersAndReload();
		// GameMode->KickPlayerController();

		auto cnt = sizeof(params) / sizeof(ServerConfigParam);
		for (int i = 0; i < cnt; i++) {

			EventMessage *m = new EventMessage();
			m->push_array(3);
			m->push_uint(m_server_config_param);
			m->push_string(std::string(params[i].name));
			m->push_float(params[i].value);
			m->set_size();
			eventWriter->push(m);

		}
	}
}
