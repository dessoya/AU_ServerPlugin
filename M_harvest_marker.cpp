#include "M_harvest_marker.h"

#include <map>
#include <chrono>

extern std::chrono::time_point<std::chrono::system_clock> cur_time;

typedef std::map<unsigned long long, std::chrono::time_point<std::chrono::system_clock>> MapHarvest;
MapHarvest mapHarvest;

bool M_harvest_marker_init() {
	mapHarvest = MapHarvest();

	return true;
}

bool M_harvest_marker_done() {
	return true;
}


void __add_harvest_marker(unsigned long iid1, unsigned long iid2) {
	unsigned long long iid = (((unsigned long long)iid1) << 32) | iid2;
	mapHarvest.insert(std::pair<unsigned long long, std::chrono::time_point<std::chrono::system_clock>>(iid, cur_time));
}

bool __get_harvest_marker(unsigned long iid1, unsigned long iid2) {
	unsigned long long iid = (((unsigned long long)iid1) << 32) | iid2;
	auto iter = mapHarvest.find(iid);
	if (iter != mapHarvest.end()) {
		mapHarvest.erase(iid);
		return true;
	}
	return false;
}
