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
		auto v = r.BaseResourceRequirement * 100;
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