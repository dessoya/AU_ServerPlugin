#include "ConsoleHelper.h"
#include <API/ARK/Ark.h>

DWORD WINAPI ThreadFunction_ch(LPVOID lpParam);
char sp[] = "                                                                                                                                                                                                                                                                                                                                       ";

void ConsoleHelper::addElement(CHElement *element) {
	elementsLock.lock();
	elements.push_back(element);
	elementsLock.unlock();
	element->onResize(w, h);
	drawElement(element);
}

void ConsoleHelper::moveElement(CHElement *element, int nx, int ny) {
	
	if (nx == element->x && ny == element->y) return;

	auto ox = element->x, oy = element->y;
	element->x = nx;
	element->y = ny;
	this->drawElement(element);

	for (auto i = oy, idx = 0, s = i + element->getH(); i < s; i++, idx++) {
		setPos(ox, i);
		outWithSpaces("", element->getW(), element->waling);
	}

}

void ConsoleHelper::drawElement(CHElement *element) {
	if (!element->isRender) return;
	SetConsoleTextAttribute(_stdout, element->color);
	auto x = element->getX();
	for (auto i = element->getY(), idx = 0, s = i + element->getH(); i < s; i++, idx++) {
		setPos(x, i);
		outWithSpaces(element->getLine(idx), element->getW(), element->waling);
	}
	SetConsoleTextAttribute(_stdout, 7);
}

void ConsoleHelper::outWithSpaces(std::string s, int w, bool waling) {

	char *ss = (char *)s.c_str();
	int len = 0;
	while (*ss) len += (*ss++ & 0xc0) != 0x80;

	if (waling) {
		if (len < w) {
			auto needs = w - len;
			std::cout << &sp[sizeof(sp) - needs - 1];
		}
		std::cout << s.c_str();
	}
	else {
		std::cout << s.c_str();
		if (len < w) {
			auto needs = w - len;
			std::cout << &sp[sizeof(sp) - needs - 1];
		}
	}
}
ConsoleHelper::ConsoleHelper() {

	curhovered = 0;
	instance = this;
	needExit = false;
	executeState = true;

	/*
			if (SetConsoleCtrlHandler(_HandlerRoutine, true) == 0) {
			}
		*/

	_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
	_stdin = GetStdHandle(STD_INPUT_HANDLE);

	DWORD fdwMode; 

	if (_stdin == INVALID_HANDLE_VALUE) {
		// Log::GetLog()->info("_stdin == INVALID_HANDLE_VALUE");
	}

	// GetConsoleMode(_stdin, &fdwMode);

	fdwMode = ENABLE_EXTENDED_FLAGS;
	if (!SetConsoleMode(_stdin, fdwMode)) {}

	fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	if (!SetConsoleMode(_stdin, fdwMode)) {}

	clear();

	hthread = CreateThread(NULL, 0, ThreadFunction_ch, this, 0, NULL);
}

ConsoleHelper *ConsoleHelper::instance = 0;

DWORD WINAPI ThreadFunction_ch(LPVOID lpParam) {
	auto con = (ConsoleHelper *)lpParam;
	return con->thread();
}

void ConsoleHelper::setPos(int x, int y) {
	// Log::GetLog()->info("setPos {} {}", x, y);
	COORD pos = { (short)x, (short)y };
	SetConsoleCursorPosition(_stdout, pos);
}

void ConsoleHelper::onKey(KEY_EVENT_RECORD ke) {
	elementsLock.lock();
	auto it = elements.begin();
	while (it != elements.end()) {
		auto e = *it;
		if (!e->isRender) {
			e->onKey(ke);
		}
		it++;
	}
	elementsLock.unlock();
}

void ConsoleHelper::onMouse(MOUSE_EVENT_RECORD me) {

	elementsLock.lock();
	auto it = elements.begin();
	CHElement *finded = 0;
	while (it != elements.end()) {
		auto e = *it;
		if(e->isRender) {
			if (e->isArea(me.dwMousePosition.X, me.dwMousePosition.Y)) {
				finded = e;
				break;
			}
		}
		it++;
	}
	elementsLock.unlock();

	if (finded != curhovered) {

		if (curhovered) {
			curhovered->lostFocus();
		}
		curhovered = finded;
		if (curhovered) {
			curhovered->setFocus();
		}
	}

	if (me.dwEventFlags == MOUSE_WHEELED) {
		if (curhovered) {
			short *w = (short *)&me.dwButtonState;
			int dir = w[1];
			curhovered->onWHeeled(dir);
		}
	}

}


void ConsoleHelper::processInpitEvents() {

	elementsLock.lock();
	auto it = elements.begin();
	while (it != elements.end()) {
		auto e = *it;
		e->onTimer();
		it++;
	}
	elementsLock.unlock();

	DWORD lpcNumberOfEvents, lpNumberOfEventsRead;
	INPUT_RECORD record;

	if (GetNumberOfConsoleInputEvents(_stdin, &lpcNumberOfEvents)) {
		while (lpcNumberOfEvents) {
			if (ReadConsoleInput(_stdin, &record, 1, &lpNumberOfEventsRead)) {

				if (lpNumberOfEventsRead > 0) {
					// Log::GetLog()->info("lpNumberOfEventsRead > 0 - {}", record.EventType);
					switch (record.EventType) {
					case WINDOW_BUFFER_SIZE_EVENT:
					{
						// Log::GetLog()->info("WINDOW_BUFFER_SIZE_EVENT {} {}", record.Event.WindowBufferSizeEvent.dwSize.X , record.Event.WindowBufferSizeEvent.dwSize.Y);
						if (record.Event.WindowBufferSizeEvent.dwSize.X > 4 && record.Event.WindowBufferSizeEvent.dwSize.Y > 4) {

							clear();
							onClear();
						}
					}
					break;
					case MOUSE_EVENT:
					{
						auto me = record.Event.MouseEvent;
						onMouse(me);
					}
					break;
					case KEY_EVENT:
					{
						auto ke = record.Event.KeyEvent;
						onKey(ke);
					}
					break;
					}
				}

				if (GetNumberOfConsoleInputEvents(_stdin, &lpcNumberOfEvents)) {
				}
				else {
					break;
				}
			}
		}
	}
}