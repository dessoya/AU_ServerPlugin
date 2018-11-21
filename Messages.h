#pragma once
#include "Utils.h"
#include "EventMessage.h"

void _msg_m_player_die(ull pid);
void _msg_m_player_learn_engram(ull pid, std::string engramName);

void _msg_m_player_update_inventory2(ull pid, UPrimalItem *item, int q);
void _msg_m_player_update_inventory_with_quality(ull pid, std::string itemName, unsigned int isbp,
	std::string quality, unsigned int quality_index, std::string req, int q);
void _msg_m_player_update_inventory(ull pid, std::string itemName, int q, int h, unsigned int isbp);
void _msg_m_player_harvest2(ull pid, std::string itemName, unsigned int q, unsigned int ht);

void _msg_m_build_structure(ull pid, std::string structureName, FVector *loc);
void _msg_m_player_set_pos(ull pid, FVector *loc);
void _msg_m_player_logout(ull pid, std::string steamId, std::string steamName, std::string name);
void _msg_m_player_login(ull pid, unsigned int messageType, ull sid, std::string steamName, std::string name);
void _msg_m_server_ping(unsigned int onlinePlayersCount);
void _msg_m_player_pasive_online(ull pid, unsigned int ms);
void _msg_m_player_set_pos_and_activity(ull pid, unsigned int ms, unsigned int activity_id, FVector *loc);

void _msg_m_tribe_create(ull tid, ull pid, std::string name);
void _msg_m_tribe_remove(ull tid);
void _msg_m_tribe_log(ull tid, std::string message);
void _msg_m_tribe_remove_player(ull tid, ull pid);
void _msg_m_tribe_add_player(ull tid, ull pid);
void _msg_m_tribe_change_name(ull tid, std::string name);
void _msg_m_tribe_set_owner(ull tid, ull pid);