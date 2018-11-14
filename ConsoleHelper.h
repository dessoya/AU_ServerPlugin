#pragma once

#include "Lock.h"
#include "CHElement.h"
#include "windows.h"
#include <vector>
#include <string>
#include <iostream>
#include <API/ARK/Ark.h>

class ConsoleHelper {

	HANDLE _stdout, _stdin;
	CHElement *curhovered;

public:
	int w, h;
	static ConsoleHelper *instance;
	std::vector<CHElement *> elements;
	Lock elementsLock;
	HANDLE hthread;
	volatile bool needExit, executeState;

	~ConsoleHelper() {
		needExit = true;
		while (executeState) {
			Sleep(10);
		}
	}

	ConsoleHelper();

	void moveElement(CHElement *element, int , int);
	void addElement(CHElement *element);
	void drawElement(CHElement *element);
	void outWithSpaces(std::string s, int w, bool waling);

	void processInpitEvents();

	virtual void onClear() {}
	virtual void onKey(KEY_EVENT_RECORD ke);
	virtual void onMouse(MOUSE_EVENT_RECORD me);

	void clear() {

		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(_stdout, &csbi);
		auto columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		auto rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		w = columns;
		h = rows;

		/*
		if (w > 200) {
			w = 200;
		}
		if (w < 4) w = 4;

		if (h > 100) {
			h = 100;
		}
		if (h < 4) h = 4;
		*/

		COORD topLeft = { 0, 0 };
		DWORD written;
		SetConsoleTextAttribute(_stdout, 7);
		FillConsoleOutputCharacterA(_stdout, ' ', w * h, topLeft, &written);

		CONSOLE_CURSOR_INFO     cursorInfo;
		GetConsoleCursorInfo(_stdout, &cursorInfo);
		cursorInfo.bVisible = false;
		SetConsoleCursorInfo(_stdout, &cursorInfo);

		SetConsoleOutputCP(CP_UTF8);

		COORD NewSBSize;
		NewSBSize.X = w;
		NewSBSize.Y = h; 
		SetConsoleScreenBufferSize(_stdout, NewSBSize);

		// FlushConsoleInputBuffer(_stdout);

		elementsLock.lock();
		auto it = elements.begin();
		while (it != elements.end()) {
			auto e = *it;
			e->onResize(w, h);
			drawElement(e);
			it++;
		}
		elementsLock.unlock();

		// FlushConsoleInputBuffer(_stdout);
		setPos(w - 1, h - 1);
	}

	// ctrl + c
	static BOOL WINAPI _HandlerRoutine(DWORD dwCtrlType) {
		return true;
	}

	void setPos(int x, int y);

	DWORD thread() {
		while (!needExit) {
			processInpitEvents();
			Sleep(50);
		} 
		executeState = false;
		return 0;
	}
};
