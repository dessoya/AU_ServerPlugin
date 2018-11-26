#pragma once

#include <API/ARK/Ark.h>
#include "M_timer.h"

extern std::chrono::time_point<std::chrono::system_clock> cur_time;

struct OnlinePlayerInfo {
	AShooterPlayerController *shooterPlayer;
	unsigned long long id;
	unsigned long long steamId;
	unsigned long long tid;
	float x, y, z;

	// bool mergeItemStack;
	bool skipAddRemoveUpdateItems;

	// harvest mechanic
	UPrimalItem *lastUpdatedItem;
	// int harvestCount;
	bool inAddItem;
	bool needharvest;
	bool inTransferFrom;
	bool inTransferAll;

	bool onDino;

	APrimalDinoCharacter *lastHitDino;

	// typedef std::map<unsigned long long, bool> Inventory;
	// Inventory inventory;

	std::chrono::time_point<std::chrono::system_clock> create_tm;
	std::chrono::time_point<std::chrono::system_clock> last_time;

	OnlinePlayerInfo(AShooterPlayerController *shooterPlayer_, unsigned long long id_, unsigned long long steamId_, std::chrono::time_point<std::chrono::system_clock> last_time_, float x_, float y_, float z_) :
		shooterPlayer(shooterPlayer_), id(id_), steamId(steamId_), last_time(last_time_), x(x_), y(y_), z(z_) {
		create_tm = cur_time;
		tid = 0;

		// harvestCount = 0;
		lastUpdatedItem = 0;
		inAddItem = false;
		needharvest = false;
		inTransferFrom = false;
		onDino = false;
		inTransferAll = false;

		lastHitDino = 0;

		skipAddRemoveUpdateItems = false;
		// mergeItemStack = false;
	}

	/*
	void __add_add_item_marker(unsigned long id1, unsigned long id2) {

	}
	*/

	/*
	bool itemInInventory(unsigned long long iid) {
		return inventory.find(iid) != inventory.end();

	}
	void addItemToInventory(unsigned long long iid) {
		inventory.insert(std::pair<unsigned long long, bool>(iid, true));
	}

	void delItemFromInventory(unsigned long long iid) {
		inventory.erase(iid);
	}
	*/

};


typedef void (*PlayerIterator)(void *ctx, OnlinePlayerInfo *info);

class OnlinePlayers {
public:

	typedef std::map<unsigned long long, OnlinePlayerInfo *> PlayerIdMap;
	PlayerIdMap playerIdMap;

	OnlinePlayers();
	~OnlinePlayers();

	unsigned int count();
	OnlinePlayerInfo *get(unsigned long long id) {
		auto it = playerIdMap.find(id);
		if (it == playerIdMap.end()) return 0;
		return it->second;
	}
	OnlinePlayerInfo *getByDinoDamage(APrimalDinoCharacter *dino) {
		auto it = playerIdMap.begin();
		while (it != playerIdMap.end()) {
			auto oi = it->second;
			if (oi->lastHitDino == dino) {
				return oi;
			}
			it ++;
		}
		return 0;
	}

	void erase(unsigned long long id);
	void insertPlayer(AShooterPlayerController* shooter_pc, unsigned long long steamId);
	void iterate(void *ctx, void **funcs);
	void _t_collect_online_and_position(TimerContext *tctx);

};

bool M_online_players_init();
bool M_online_players_done();
extern OnlinePlayers *onlinePlayers;
