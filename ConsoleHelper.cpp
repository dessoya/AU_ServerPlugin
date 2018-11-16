#include "pch.h"
#include "ConsoleHelper.h"
// #include <API/ARK/Ark.h>


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
	deleteLock.lock();
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
	deleteLock.unlock();
	clear();
}

void ConsoleHelper::addElement(CHElement *element) {

	CHRenderElement *r = 0;
	elementsLock.lock();
	elements[element->name] = element;
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

	// outLock.lock();
	for (auto i = oy, idx = 0, s = i + element->h; i < s; i++, idx++) {
		setPos(ox, i);
		outWithSpaces("", false, element->w, element->waling);
	}
	// outLock.unlock();

}

void ConsoleHelper::updateElements() {
	deleteLock.lock();

	std::vector< CHRenderElement * > els;

	updateLock.lock();
	if (needRedrawElements.size()) {
		auto it = needRedrawElements.begin();
		while (it != needRedrawElements.end()) {
			auto e = it->second;
			els.push_back(e);
			it++;
		}
		needRedrawElements.clear();
	}
	updateLock.unlock();

	// outLock.lock();
	auto it = els.begin();
	while (it != els.end()) {
		auto e = *it;
		e->lock();

		SetConsoleTextAttribute(_stdout, e->color);
		auto x = e->x;
		bool nc;
		for (auto i = e->y, idx = 0, s = i + e->h; i < s; i++, idx++) {
			setPos(x, i);
			nc = true;
			auto str = e->getLine(idx, &nc);
			outWithSpaces(str, nc, e->w, e->waling);
		}
		SetConsoleTextAttribute(_stdout, 7);

		e->unlock();

		it++;
	}
	// outLock.unlock();

	deleteLock.unlock();
}


void ConsoleHelper::drawElement(CHRenderElement *element) {

	if (!element->isRender) return;

	updateLock.lock();
	needRedrawElements[element->name] = element;
	updateLock.unlock();
}

#define cout_space(c) std::cout << &sp[sizeof(sp) - c - 1]

void ConsoleHelper::outWithSpaces(std::string s, bool nc, int w, bool waling) {


	char *ss = (char *)s.c_str();
	int len = 0;
	while (*ss) len += (*ss++ & 0xc0) != 0xc0;
	auto needs = w - len;


	if ( waling && needs > 0) cout_space(needs);
	outstr(s, nc, w);
	if (!waling && needs > 0) cout_space(needs);

}

TCHAR tbuf[4096];
void ConsoleHelper::outstr(std::string s, bool nc, int w) {

	auto ss = (char *)s.c_str();
	int nlen = 0, b = 0;
	while (*ss && nlen < w) {
		b++;
		nlen += (*ss++ & 0xc0) != 0xc0;
	}
		
	DWORD writen;

	if (nc) {
		auto sz = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), b, tbuf, sizeof(tbuf) / sizeof(TCHAR));
		WriteConsole(_stdout, tbuf, nlen, &writen, 0);
	}
	else {
		WriteConsole(_stdout, s.c_str() , nlen, &writen, 0);
	}
}

ConsoleHelper::ConsoleHelper() {

	curhovered = 0;
	instance = this;
	needExit = false;
	executeState = true;
	w = h = 4;

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

	DWORD lpcNumberOfEvents, lpNumberOfEventsRead;
	INPUT_RECORD record;
	bool needClear = false;

	if (GetNumberOfConsoleInputEvents(_stdin, &lpcNumberOfEvents)) {
		while (lpcNumberOfEvents) {
			if (ReadConsoleInput(_stdin, &record, 1, &lpNumberOfEventsRead)) {
				if (lpNumberOfEventsRead > 0) {
					switch (record.EventType) {
					case WINDOW_BUFFER_SIZE_EVENT: needClear = true; break;
					case MOUSE_EVENT: onMouse(record.Event.MouseEvent); break;
					case KEY_EVENT: onKey(record.Event.KeyEvent); break;
					}
				}
				if (GetNumberOfConsoleInputEvents(_stdin, &lpcNumberOfEvents) == 0) break;
			}
		}
	}

	if (needClear) clear();
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
		updateElements();
		Sleep(150);
	}
	executeState = false;
	return 0;
}

void ConsoleHelper::clear() {

	// outLock.lock();

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(_stdout, &csbi);
	auto columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	auto rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	w = columns;
	h = rows;


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
	
	elementsLock.unlock();

	{
		auto it = elements.begin();
		while (it != elements.end()) {
			auto e = it->second;
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
}

CHElement *ConsoleHelper::_getElement(std::string name) {
	if (instance) {
		return instance->getElement(name);
	}
	return 0;
}
