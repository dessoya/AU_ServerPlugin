#include "M_timer.h"
#include "EventMessage.h"
#include "M_event_writer.h"
#include "M_online_players.h"
#include <API/ARK/Ark.h>
#include "M_hook_helper.h"

__time cur_time, next_time, last_treat_timer;
bool firstTime;
int timer_interval_msecs = 3500;
long long __timestamp = 0;

void __Timer();

typedef std::vector<TimerCallback> TimerList;
TimerList timerList;

void addTimer(TimerCallback tcb) {
	timerList.push_back(tcb);
}

HANDLE con = 0;
int ccolumns = 0, crows = 0;


/*
struct TimerContext {
	std::chrono::time_point<std::chrono::system_clock> now;
	long long timestamp;
};
*/

/*

void ti_playerPosition(void *ctx, OnlinePlayerInfo *info) {

	auto tctx = (TimerContext *)ctx;

	FVector pos = info->shooterPlayer->DefaultActorLocationField();

	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(tctx->now - info->last_time);
	auto ms = diff.count();

	int activity_id;
	if (info->x == pos.X && info->y == pos.Y && info->z == pos.Z) {
		activity_id = pa_pasive_online;

		EventMessage *m = new EventMessage();
		m->push_array(4);

		m->push_uint(m_player_pasive_online);
		m->push_uint(tctx->timestamp);
		m->push_uint(info->id);

		m->push_uint(ms);

		m->set_size();
		eventWriter->push(m);
	}
	else {
		activity_id = pa_active_online;
		info->x = pos.X;
		info->y = pos.Y;
		info->z = pos.Z;

		EventMessage *m = new EventMessage();
		m->push_array(8);

		m->push_uint(m_player_set_pos_and_activity);
		m->push_uint(tctx->timestamp);
		m->push_uint(info->id);

		m->push_float(pos.X);
		m->push_float(pos.Y);
		m->push_float(pos.Z);

		m->push_uint(activity_id);
		m->push_uint(ms);

		m->set_size();
		eventWriter->push(m);
	}


	Log::GetLog()->info("[*] update player activity {} id {} ms {}", activity_id, info->id, ms);
}

void ti_playerActivity(void *ctx, OnlinePlayerInfo *info) {

	auto tctx = (TimerContext *)ctx;

	FVector pos = info->shooterPlayer->DefaultActorLocationField();
	int message_id;
	if (info->x == pos.X && info->y == pos.Y && info->z == pos.Z) {
		message_id = m_player_pasive_online;

	}
	else {
		message_id = m_player_active_online;
		info->x = pos.X;
		info->y = pos.Y;
		info->z = pos.Z;
	}

	EventMessage *m = new EventMessage();
	m->push_array(4);

	m->push_uint(message_id);
	m->push_uint(tctx->timestamp);
	m->push_uint(info->id);

	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(tctx->now - info->last_time);
	auto ms = diff.count();

	m->push_uint(ms);

	m->set_size();
	eventWriter->push(m);


}


void ti_updateLastTime(void *ctx, OnlinePlayerInfo *info) {
	auto tctx = (TimerContext *)ctx;
	info->last_time = tctx->now;
}


void *timerIterators[] = {
	ti_playerPosition,
	// ti_playerActivity,
	ti_updateLastTime,
	0
};
*/

DECLARE_HOOK(UWorld_InitWorld, void, UWorld*, DWORD64);
void Hook_UWorld_InitWorld(UWorld *w, DWORD64 p) {
	UWorld_InitWorld_original(w, p);
	con = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(con, &csbi);
	auto columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	auto rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	// ccolumns = columns;
	// crows = rows;

	COORD topLeft = { 0, 0 };
	DWORD written;
	// CONSOLE_SCREEN_BUFFER_INFO screen;
	// GetConsoleScreenBufferInfo(con, &screen);
	FillConsoleOutputCharacterA(
		con, ' ', columns * rows, topLeft, &written
	);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(con, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(con, &cursorInfo);

	SetConsoleOutputCP(CP_UTF8);
}


void __Timer() {
	const auto now = cur_time = std::chrono::system_clock::now();

	{
		using namespace std::chrono;
		__timestamp = time_point_cast<milliseconds>(now).time_since_epoch().count();
	}

	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(next_time - now);

	if (diff.count() <= 0) {
		next_time = now + std::chrono::milliseconds(timer_interval_msecs);
		// Log::GetLog()->info("__timer"); 

		TimerContext tctx{ firstTime ? 0 : (int)(std::chrono::duration_cast<std::chrono::milliseconds>(now - last_treat_timer).count()) };
		firstTime = false;
		last_treat_timer = now;

		auto it = timerList.begin();
		while (it != timerList.end()) {
			(*it)(&tctx);
			it++;
		}

		auto workedtime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - last_treat_timer);

		/*
		if (droppedList) {
			auto it2 = droppedList->cbegin();
			while (it2 != droppedList->cend()) {

				auto diff2 = std::chrono::duration_cast<std::chrono::milliseconds>(now - it2->second->added);
				if (diff2.count() > 1000 * 60 * 10) {
					delete it2->second;
					droppedList->erase(it2++);
				}
				else {
					it2++;
				}
			}
		}
		*/

		/*
		int h1 = 0, h2 = 0;
		if (excludeHarvestTypeList) h1 = (int)excludeHarvestTypeList->size();
		if (droppedList) h2 = (int)droppedList->size();

		Log::GetLog()->info("__timer h1:{} h2:{}", h1, h2);
		*/

		/*
		long long timestamp;
		{
			using namespace std::chrono;
			auto now = system_clock::now();
			timestamp = time_point_cast<milliseconds>(now).time_since_epoch().count();
		}

		if (onlinePlayers) {
			TimerContext ctx{ now, timestamp };

			onlinePlayers->iterate(&ctx, timerIterators);

			EventMessage *m = new EventMessage();
			m->push_array(3);

			m->push_uint(m_server_ping);
			m->push_uint(timestamp);

			m->push_uint(onlinePlayers->count());

			m->set_size();
			eventWriter->push(m);
		}
		*/

		/*
		EventMessage *m = new EventMessage();
		m->push_array(3);

		m->push_uint(m_server_ping);
		m->push_uint(timestamp);

		m->push_uint(onlinePlayers->count());

		m->set_size();
		eventWriter->push(m);
		*/

		if (con) {

			CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo(con, &csbi);
			auto columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
			auto rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
			if (columns != ccolumns || rows != crows) {
				ccolumns = columns;
				crows = rows;

				COORD topLeft = { 0, 0 };
				DWORD written;
				// CONSOLE_SCREEN_BUFFER_INFO screen;
				// GetConsoleScreenBufferInfo(con, &screen);
				FillConsoleOutputCharacterA(
					con, ' ', columns * rows, topLeft, &written
				);

				CONSOLE_CURSOR_INFO     cursorInfo;

				GetConsoleCursorInfo(con, &cursorInfo);
				cursorInfo.bVisible = false;
				SetConsoleCursorInfo(con, &cursorInfo);
			}

			
			std::string s = ArkApi::Tools::Utf8Encode(L"Привед");
			// Log::GetLog()->info("__timer e:{}ms w:{}ms {} sz{}:{}", tctx.elapsed_ms, workedtime.count(), s.c_str(), columns, rows);
			// Log::GetLog()->

			// HANDLE hConsole_c = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
			// char * str = "Some Text\r\n";
			// DWDORD len = strlen(str);


			auto s1 = std::chrono::system_clock::now();

			COORD pos = { 1, 1 };
			SetConsoleCursorPosition(con, pos);
			printf("ts:%lld", __timestamp);

			auto e2 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - s1);

			pos = { 5, 5 };
			SetConsoleCursorPosition(con, pos);


			// UWorld::InitWorld

			char buf[1024];
			// sprintf_s(buf, "\x1b[%d;%dH",5,5);
			// _write(1, buf, strlen(buf));
			sprintf_s(buf, "__timer e2:%d e:%d ms w:%d ms %s sz%d:%d     ", (int)e2.count(), (int)tctx.elapsed_ms, (int)workedtime.count(), s.c_str(), ccolumns, crows);
			// _write(1, buf, strlen(buf));
			// DWORD dwBytesWritten;
			// WriteConsole(con, buf, strlen(buf), &dwBytesWritten, NULL);
			printf("%s", buf);
		}
	}
}


bool M_timer_init() {

	__register_hook(UWorld, InitWorld);

	con = GetStdHandle(STD_OUTPUT_HANDLE);

	timerList = TimerList();
	firstTime = true;

	const auto now = cur_time = std::chrono::system_clock::now();
	next_time = now + std::chrono::milliseconds(timer_interval_msecs);

	{
		using namespace std::chrono;
		__timestamp = time_point_cast<milliseconds>(now).time_since_epoch().count();
	}

	ArkApi::GetCommands().AddOnTimerCallback("__Timer", &__Timer);

	return true;
}

bool M_timer_done() {
	ArkApi::GetCommands().RemoveOnTimerCallback("__Timer");
	__unregister_hook(UWorld, InitWorld);
	return true;
}
