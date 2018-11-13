#pragma once

#include <chrono>

bool M_timer_init();
bool M_timer_done();

struct TimerContext {
	int elapsed_ms;
};

typedef void(*TimerCallback)(TimerContext *);

void addTimer(TimerCallback tcb);

typedef std::chrono::time_point<std::chrono::system_clock> __time;

extern __time next_time;
extern __time cur_time;
extern int timer_interval_msecs;
extern long long __timestamp;