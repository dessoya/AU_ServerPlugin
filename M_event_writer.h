#pragma once

#include "EventMessage.h"

#include <queue>
#include <map>

#include "windows.h"

class EventWriter {

	typedef std::queue< EventMessage *> MessageQueue;
	MessageQueue *queue;

	typedef std::map<std::string, unsigned long long> StringLibrary;
	StringLibrary *stringLibrary;
	unsigned long long stringId;

	volatile HANDLE queue_mutex;
	HANDLE hthread;
	volatile bool exitFlag, running;

	std::string folder;

public:

	EventWriter *eventStringWriter;

	EventWriter(std::string folder_, EventWriter *eventStringWriter_ = 0, std::string stringfolder_ = std::string(""));
	~EventWriter();
	void start();

	void push(EventMessage *message);
	void push_string_to_message(EventMessage *message, std::string string);

	DWORD thread();

};

bool M_event_writer_init();
bool M_event_writer_done();

extern EventWriter *eventWriter;
extern EventWriter *eventStringWriter;
