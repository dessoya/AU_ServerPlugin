#pragma once

#include "Lock.h"
#include "CHElement.h"
#include "windows.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
// #include <API/ARK/Ark.h>

class ConsoleHelper {

	HANDLE _stdout, _stdin;
	CHRenderElement *curhovered;

public:
	int w, h;
	static ConsoleHelper *instance;

	std::map<std::string, CHElement *> elements;
	std::vector<CHRenderElement *> renderElements;

	Lock elementsLock, outLock;

	HANDLE hthread;
	volatile bool needExit, executeState;

	ConsoleHelper();
	virtual ~ConsoleHelper();

	void addElement(CHElement *element);
	void delElement(CHElement *element);
	CHElement *getElement(std::string name);

	void moveElement(CHRenderElement *element, int, int);
	void drawElement(CHRenderElement *element);

	void outstr(std::string, bool nc, int w);
	void outWithSpaces(std::string s, bool nc, int w, bool waling);

	void processInpitEvents();

	virtual void onClear() {}
	virtual void onKey(KEY_EVENT_RECORD ke);
	virtual void onMouse(MOUSE_EVENT_RECORD me);

	void clear();

	// ctrl + c
	/*
	static BOOL WINAPI _HandlerRoutine(DWORD dwCtrlType) {
		return true;
	}
	*/

	void setPos(int x, int y);
	DWORD thread();

	static CHElement *_getElement(std::string name);
};
