#pragma once

#include <io.h>  
#include <fcntl.h>  

#include <string>

enum DataTypes {
	dt_null = 0,
	dt_int8,
	dt_uint8,
	dt_int16,
	dt_uint16,
	dt_int24,
	dt_uint24,
	dt_int32,
	dt_uint32,
	dt_int64,
	dt_uint64,
	dt_float,
	dt_double,
	dt_string8,
	dt_string16,
	dt_string24,
	dt_array8,
	dt_array16,
	dt_array24,
	dt_map8,
	dt_map16,
	dt_map24,
	dt_string_lib8,	// index of string from library
	dt_string_lib16,
	dt_string_lib24,
	dt_string_lib32,
	dt_string_lib64,
	dt_nint8,
	dt_nint16,
	dt_nint24,
	dt_nint32,
	dt_nint64
};

enum MessageTypes {

	m_none = 0,
	m_system_add_string_to_library,
	m_server_start,
	m_stop_work,
	m_player_online,					// 4

	m_player_create = 16,				// 
	m_player_login,						// 17
	m_player_logout,					// 18
	m_build_structure,					// 19
	m_player_set_pos,					// 20
	m_player_active_online,				// 21
	m_player_pasive_online,				// 22
	m_player_set_pos_and_activity,
	m_player_update_inventory,						// 24 depricated
	m_server_ping,						// 25
	m_player_die,
	m_player_learn_engram,
	m_player_update_inventory_with_quality,			// 28 depricated 

	m_player_harvest2,					// 29
	m_player_update_inventory2,			// 30
};

enum HarvestType {
	ht_hand = 0,
	ht_dino,
	ht_dam,
};

enum PlayerActivity {
	pa_pasive_online = 0,
	pa_active_online
};


struct EventMessagePart;
struct EventMessagePart {
	EventMessagePart *next;
	char *buffer;
	unsigned int size, used;
};

class EventMessage {
	EventMessagePart *first, *current;
	unsigned int pos, size;

public:

	int getSize();

	EventMessage();
	~EventMessage();

	void write(char *buffer);
	void write(int fd);

	void push_memory(void *m, unsigned int msize);

	void push_uint8(unsigned char i);
	void push_uint16(unsigned short i);
	void push_uint24(unsigned long i);
	void push_uint32(unsigned long i);
	void push_uint64(unsigned long long i);
	void push_string(std::string &s);
	void push_float(float f);
	void push_array(int count);

	void push_uint(unsigned long long i);
	void push_int(long long i);
	void push_stringid(unsigned long long i);
	void set_size();
};
