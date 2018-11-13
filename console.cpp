
#include "pch.h"
#include <iostream>
#include <vector>
#include "windows.h"

/*
	if (classId_mutex == 0) {
		classId_mutex = CreateMutex(NULL, FALSE, NULL);
	}

	auto dwWaitResult = WaitForSingleObject(classId_mutex, INFINITE);
	if (WAIT_OBJECT_0 != dwWaitResult) {
		OutputDebugStringA("RegisterClassId WaitForSingleObject problem\n");
	}

	classIdIterator++;
	auto id = classIdIterator;

	ReleaseMutex(classId_mutex);

*/

class ConsoleHelper;
ConsoleHelper *curcon = 0;

class CHElement {
	int ox, oy;
	int x, y, w, h;
	HANDLE mutex;
public:

	bool waling;
	bool stickRight;
	int color;
	CHElement(int x_, int y_, int w_, int h_) : x(x_), ox(x_), oy(y_), y(y_), w(w_), h(h_) {
		mutex = CreateMutex(NULL, FALSE, NULL);
		waling = false;
		stickRight = false;
		color = 7;
	}

	void lock() {
		auto dwWaitResult = WaitForSingleObject(mutex, INFINITE);
		if (WAIT_OBJECT_0 != dwWaitResult) {
			// OutputDebugStringA("RegisterClassId WaitForSingleObject problem\n");
		}
	}

	void unlock() {
		ReleaseMutex(mutex);
	}

	int getX() { return x; }
	int getY() { return y; }
	int getH() { return h; }
	int getW() { return w; }
	virtual std::string getLine(int idx) { return ""; }
	virtual void onResize(int cw, int ch) {
		if (stickRight) {
			x = cw - w - ox;
		}
	}
	bool isArea(int hx, int hy) {
		if (hx >= x && hx < x + w && hy >= y && hy < y + h) {
			return true;
		}
		return false;
	}
	void update();
	virtual void lostFocus() { }
	virtual void setFocus() { }

	virtual void onWHeeled(int dir) {}
};

class CHTitle : public CHElement {
	std::string title;
	int ocolor;
public:
	CHTitle(int x_, int y_, std::string title_, int w_) : CHElement(x_, y_, w_, 1), title(title_) {

	}

	virtual std::string getLine(int idx) {
		if (idx == 0) return title;
		return "";
	}

	void setTitle(std::string title_) { title = title_; }

	virtual void lostFocus() {
		color = ocolor;
		update();
	}
	virtual void setFocus() {
		ocolor = color;
		color = 8;
		update();
	}

	virtual void onWHeeled(int dir) {
		if (dir > 0) {
			color++;
		}
		else {
			color--;
		}
		update();
	}
};

char sp[] = "                                                                                                                                                                                                                                                                                                                                       ";
class ConsoleHelper {

	HANDLE _stdout, _stdin;
	int w, h;


public:
	std::vector<CHElement *> elements;

	void addElement(CHElement *element) {
		elements.push_back(element);
		element->onResize(w, h);
		drawElement(element);
	}

	void drawElement(CHElement *element) {
		SetConsoleTextAttribute(_stdout, element->color);
		auto x = element->getX();
		for (auto i = element->getY(), idx = 0, s = i + element->getH(); i < s; i++, idx++) {
			setPos(x, i);
			outWithSpaces(element->getLine(idx), element->getW(), element->waling);
		}
		SetConsoleTextAttribute(_stdout, 7);
	}

	void outWithSpaces(std::string s, int w, bool waling) {
		
		char *ss = (char *)s.c_str();
		int len = 0;
		while (*ss) len += (*ss++ & 0xc0) != 0x80;

		if (waling) {
			if (len < w) {
				auto needs = w - len;
				std::cout << &sp[sizeof(sp) - needs];
			}
			std::cout << s.c_str();
		}
		else {
			std::cout << s.c_str();
			if (len < w) {
				auto needs = w - len;
				std::cout << &sp[sizeof(sp) - needs];
			}
		}
	}

	ConsoleHelper() {

/*
		if (SetConsoleCtrlHandler(_HandlerRoutine, true) == 0) {
		}
	*/

		_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
		_stdin = GetStdHandle(STD_INPUT_HANDLE);

		DWORD fdwMode = ENABLE_EXTENDED_FLAGS;
		if (!SetConsoleMode(_stdin, fdwMode)) {}
		fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
		if (!SetConsoleMode(_stdin, fdwMode)) {}

		clear();
	}

	void processInpitEvents() {

		DWORD lpcNumberOfEvents, lpNumberOfEventsRead;
		INPUT_RECORD record;

		GetNumberOfConsoleInputEvents(_stdin, &lpcNumberOfEvents);
		while (lpcNumberOfEvents) {
			ReadConsoleInput(_stdin, &record, 1, &lpNumberOfEventsRead);
			
			switch (record.EventType) {
			case WINDOW_BUFFER_SIZE_EVENT:
			{
				clear();
				onClear();
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

			GetNumberOfConsoleInputEvents(_stdin, &lpcNumberOfEvents);
		}
	}

	virtual void onClear() {}
	virtual void onKey(KEY_EVENT_RECORD ke) {}
	virtual void onMouse(MOUSE_EVENT_RECORD me) {}

	void clear() {

		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(_stdout, &csbi);
		auto columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		auto rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		w = columns;
		h = rows;

		COORD topLeft = { 0, 0 };
		DWORD written;
		SetConsoleTextAttribute(_stdout, 7);
		FillConsoleOutputCharacterA(
			_stdout, ' ', columns * rows, topLeft, &written
		);

		CONSOLE_CURSOR_INFO     cursorInfo;
		GetConsoleCursorInfo(_stdout, &cursorInfo);
		cursorInfo.bVisible = false;
		SetConsoleCursorInfo(_stdout, &cursorInfo);

		SetConsoleOutputCP(CP_UTF8);

		COORD NewSBSize;
		NewSBSize.X = columns;
		NewSBSize.Y = rows;
		SetConsoleScreenBufferSize(_stdout, NewSBSize);

		FlushConsoleInputBuffer(_stdout);

		auto it = elements.begin();
		while (it != elements.end()) {
			auto e = *it;
			e->onResize(w, h);
			drawElement(e);
			it++;
		}
		FlushConsoleInputBuffer(_stdout);
		setPos(w - 1, h - 1);
	}

	// ctrl + c
	static BOOL WINAPI _HandlerRoutine(DWORD dwCtrlType) {
		return true;
	}

	void setPos(int x, int y) {
		COORD pos = { (short)x, (short)y };
		SetConsoleCursorPosition(_stdout, pos);
	}
};
void CHElement::update() {
	 if (curcon) {
		 curcon->drawElement(this);
	}
}

class MyCH : public ConsoleHelper {

	CHTitle *keytitle, *mousetitle;
	CHElement *curhovered;

public:

	MyCH() : ConsoleHelper() {

		curhovered = 0;

		keytitle = new CHTitle(0, 0, "", 36);
		keytitle->waling = true;
		keytitle->stickRight = true;
		addElement(keytitle);

		mousetitle = new CHTitle(0, 1, "", 26);
		mousetitle->waling = true;
		mousetitle->stickRight = true;
		addElement(mousetitle);
	}
	virtual void onKey(KEY_EVENT_RECORD ke) {
		
		char buf[60];
		sprintf_s(buf, "кнопка %d %s state %d", ke.wVirtualKeyCode, (ke.bKeyDown ? "нажата" : "отжата"), ke.dwControlKeyState);
		keytitle->setTitle(buf);
		drawElement(keytitle);


		if (ke.wVirtualKeyCode == 67 && ke.dwControlKeyState & 8) {
			exit(1);
		}
	}

	virtual void onMouse(MOUSE_EVENT_RECORD me) {
		
		/*
		me.dwEventFlags == MOUSE_WHEELED
		tt > 0 or < 0 = direction
		*/

		char buf[30];
		short *w = (short *)&me.dwButtonState;
		int tt = w[1];
		sprintf_s(buf, "x %d y %d f %d s %d", me.dwMousePosition.X, me.dwMousePosition.Y, me.dwEventFlags, tt);
		mousetitle->setTitle(buf);
		drawElement(mousetitle);

		auto it = elements.begin();
		CHElement *finded = 0;
		while (it != elements.end()) {
			auto e = *it;
			if (e->isArea(me.dwMousePosition.X, me.dwMousePosition.Y)) {
				finded = e;
				break;
			}
			it++;
		}

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
				curhovered->onWHeeled(tt);
			}
		}

		/*
		search element
		*/
	}
};


int main() {
	auto ch = curcon = new MyCH();
	auto title = new CHTitle(5, 5, "test title", 15);
	title->color = 2;
	ch->addElement(title);

	while (true) {
		Sleep(10);
		ch->processInpitEvents();
	}

	return 0;
}
