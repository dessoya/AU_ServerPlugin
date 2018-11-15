#include "ConsoleHelper.h"
#include <API/ARK/Ark.h>

DWORD WINAPI ThreadFunction_ch(LPVOID lpParam);
char sp[] = "                                                                                                                                                                                                                                                                                                                                       ";

CHElement *ConsoleHelper::getElement(std::string name) {
	CHElement *e = 0;
	elementsLock.lock();
	auto it = elements.find(name);
	if (it != elements.end()) {
		e = it->second;
	}
	elementsLock.unlock();
	return e;
}

void ConsoleHelper::delElement(CHElement *element) {
	elementsLock.lock();
	{
		auto it = elements.begin();
		while (it != elements.end()) {
			auto e = it->second;
			if (e == element) {
				e->lock();
				elements.erase(it);
				e->unlock();
				break;
			}
			it++;
		}
	}
	{
		auto it = renderElements.begin();
		while (it != renderElements.end()) {
			auto e = *it;
			if (e == element) {
				e->lock();
				renderElements.erase(it);
				e->unlock();
				break;
			}
			it++;
		}
	}
	elementsLock.unlock();
	clear();
}

void ConsoleHelper::addElement(CHElement *element) {

	CHRenderElement *r = 0;
	elementsLock.lock();
	elements.insert(std::pair<std::string, CHElement *>(element->name, element));
	if (element->isRender) {
		r = reinterpret_cast<CHRenderElement *>(element);
		renderElements.push_back(r);
	}
	elementsLock.unlock();
	element->onResize(w, h);

	if (r) {
		r->onResize(w, h);
		drawElement(r);
	}
}

void ConsoleHelper::moveElement(CHRenderElement *element, int nx, int ny) {
	
	if (nx == element->x && ny == element->y) return;

	auto ox = element->x, oy = element->y;
	element->x = nx;
	element->y = ny;
	this->drawElement(element);

	outLock.lock();
	for (auto i = oy, idx = 0, s = i + element->h; i < s; i++, idx++) {
		setPos(ox, i);
		outWithSpaces("", false, element->w, element->waling);
	}
	outLock.unlock();

}

void ConsoleHelper::drawElement(CHRenderElement *element) {

	if (!element->isRender) return;

	element->lock();
	outLock.lock();

	SetConsoleTextAttribute(_stdout, element->color);
	auto x = element->x;
	bool nc;
	for (auto i = element->y, idx = 0, s = i + element->h; i < s; i++, idx++) {
		setPos(x, i);
		nc = true;
		auto str = element->getLine(idx, &nc);
		outWithSpaces(str, nc, element->w, element->waling);
	}
	SetConsoleTextAttribute(_stdout, 7);

	outLock.unlock();
	element->unlock();
}

void ConsoleHelper::outWithSpaces(std::string s, bool nc, int w, bool waling) {


	char *ss = (char *)s.c_str();
	int len = 0;
	while (*ss) len += (*ss++ & 0xc0) != 0xc0;

	if (waling) {
		if (len < w) {
			auto needs = w - len;
			std::cout << &sp[sizeof(sp) - needs - 1];
		}
		outstr(s, nc, w);
	}
	else {
		outstr(s, nc, w);
		if (len < w) {
			auto needs = w - len;
			std::cout << &sp[sizeof(sp) - needs - 1];
		}
	}

}

void ConsoleHelper::outstr(std::string s, bool nc, int w) {

	char *ss = (char *)s.c_str();
	int len = 0;
	while (*ss) len += (*ss++ & 0xc0) != 0xc0;

	// if (len > w) {

		ss = (char *)s.c_str();
		int nlen = 0, b = 0;
		while (*ss && nlen < w) {
			b++;
			nlen += (*ss++ & 0xc0) != 0xc0;
		}
		
		DWORD writen;

		if (nc) {
			TCHAR buf[4096];
			auto sz = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), b, buf, sizeof(buf) / sizeof(TCHAR));

			WriteConsole(_stdout, buf, nlen, &writen, 0);
		}
		else {
			WriteConsole(_stdout, s.c_str() , nlen, &writen, 0);
		}

		/*
	}
	else {
		std::cout << s.c_str();
	}
	*/
}

ConsoleHelper::ConsoleHelper() {

	curhovered = 0;
	instance = this;
	needExit = false;
	executeState = true;
	w = h = 4;

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
		auto e = it->second;
		if (!e->isRender) {
			e->onKey(ke);
		}
		it++;
	}
	elementsLock.unlock();
}

void ConsoleHelper::onMouse(MOUSE_EVENT_RECORD me) {

	elementsLock.lock();
	auto it = renderElements.begin();
	CHRenderElement *finded = 0;
	while (it != renderElements.end()) {
		auto e = *it;
		if (e->isArea(me.dwMousePosition.X, me.dwMousePosition.Y)) {
			finded = e;
			break;
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

	if (curhovered) {
		curhovered->onMouse(me);
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
		auto e = it->second;
		e->onTimer();
		it++;
	}
	elementsLock.unlock();

	DWORD lpcNumberOfEvents, lpNumberOfEventsRead;
	INPUT_RECORD record;
	bool needClear = false;

	if (GetNumberOfConsoleInputEvents(_stdin, &lpcNumberOfEvents)) {
		while (lpcNumberOfEvents) {
			if (ReadConsoleInput(_stdin, &record, 1, &lpNumberOfEventsRead)) {

				if (lpNumberOfEventsRead > 0) {
					// Log::GetLog()->info("lpNumberOfEventsRead > 0 - {}", record.EventType);
					switch (record.EventType) {
					case WINDOW_BUFFER_SIZE_EVENT:
					{
						needClear = true;

						// Log::GetLog()->info("WINDOW_BUFFER_SIZE_EVENT {} {}", record.Event.WindowBufferSizeEvent.dwSize.X , record.Event.WindowBufferSizeEvent.dwSize.Y);
						// if (record.Event.WindowBufferSizeEvent.dwSize.X > 4 && record.Event.WindowBufferSizeEvent.dwSize.Y > 4) {

							// clear();
							// onClear();
						// }
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

	if (needClear) {
		clear();
		onClear();
	}
}

ConsoleHelper::~ConsoleHelper() {

	needExit = true;
	while (executeState) {
		Sleep(10);
	}

	elementsLock.lock();

	while (true) {
		auto s = elements.size();
		if (s < 1) break;
		auto it = elements.begin();
		auto e = it->second;
		delete e;
		elements.erase(it);
	}
	renderElements.clear();

	elementsLock.unlock();
}

DWORD ConsoleHelper::thread() {
	while (!needExit) {
		processInpitEvents();
		Sleep(50);
	}
	executeState = false;
	return 0;
}

void ConsoleHelper::clear() {

	elementsLock.lock();

	{
		auto it = elements.begin();
		while (it != elements.end()) {
			auto e = it->second;
			e->lock();
			it++;
		}
	}


	outLock.lock();

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

	// COORD topLeft = { 0, 0 };
	// DWORD written;

	CONSOLE_CURSOR_INFO     cursorInfo;
	GetConsoleCursorInfo(_stdout, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(_stdout, &cursorInfo);

	SetConsoleOutputCP(CP_UTF8);

	COORD NewSBSize;
	NewSBSize.X = w;
	NewSBSize.Y = h;
	SetConsoleScreenBufferSize(_stdout, NewSBSize);

	SetConsoleTextAttribute(_stdout, 7);
	for (int i = 0; i < h; i++) {
		setPos(0, i);
		std::cout << &sp[sizeof(sp) - w - 1];
	}
	// FillConsoleOutputCharacterA(_stdout, ' ', w * h, topLeft, &written);

	outLock.unlock();
	// FlushConsoleInputBuffer(_stdout);

	
	{
		auto it = elements.begin();
		while (it != elements.end()) {
			auto e = it->second;
			e->unlock();
			e->onResize(w, h);
			it++;
		}
	}
	{
		auto it = renderElements.begin();
		while (it != renderElements.end()) {
			auto e = *it;
			drawElement(e);
			it++;
		}
	}

	elementsLock.unlock();

	// FlushConsoleInputBuffer(_stdout);	
	// setPos(5, 5);

	// DWORD writen;

	/*
	char b3[] = "\u2193";
	char b4[100];
	WideCharToMultiByte(CP_UTF8, 0, (TCHAR *)b3, 2, b4, 4, 0, 0);
	WriteConsole(_stdout, b4, 2, &writen, 0);
	*/

	// char buf[] = "\x93\x21привет";
	// WriteConsole(_stdout, buf, 1, &writen, 0);

	/*
	TCHAR buf2[4096];
	auto sz = MultiByteToWideChar(CP_UTF8, MB_USEGLYPHCHARS, buf, sizeof(buf), buf2, sizeof(buf2) / sizeof(TCHAR));
	*/

	// WriteConsole(_stdout, buf2, 7, &writen, 0);
	// WriteConsole(_stdout, buf, 7, &writen, 0);
	
	// std::cout << "\x02\xc7";

	/*
	outLock.lock();
	setPos(w - 1, h - 1);
	outLock.unlock();
	*/
}

CHElement *ConsoleHelper::_getElement(std::string name) {
	if (instance) {
		return instance->getElement(name);
	}
	return 0;
}
