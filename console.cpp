
#include "pch.h"
#include <iostream>
#include <vector>
#include "windows.h"

#include "ConsoleHelper.h"

class MyCH : public ConsoleHelper {

public:

	MyCH() : ConsoleHelper() {

		auto keytitle = new CHTitle("keytitle", 0, 0, "", 40, true);
		keytitle->waling = true;
		addElement(keytitle);

		auto mousetitle = new CHTitle("mousetitle", 0, 1, "", 40, true);
		mousetitle->waling = true;
		addElement(mousetitle);

	}

	virtual void onKey(KEY_EVENT_RECORD ke) {

		char buf[130];
		sprintf_s(buf, "кнопка %d %s state %d", ke.wVirtualKeyCode, (ke.bKeyDown ? "нажата" : "отжата"), ke.dwControlKeyState);
		auto keytitle = (CHTitle*)ConsoleHelper::_getElement("keytitle");
		if (keytitle) {
			keytitle->setTitle(buf);
			drawElement(keytitle);
		}

		ConsoleHelper::onKey(ke);
	}

	virtual void onMouse(MOUSE_EVENT_RECORD me) {

		char buf[130];
		short *w = (short *)&me.dwButtonState;
		int dir = w[1];
		sprintf_s(buf, "x %d y %d b %d f %d s %d", me.dwMousePosition.X, me.dwMousePosition.Y, w[0], me.dwEventFlags, dir);
		auto mousetitle = (CHTitle*)ConsoleHelper::_getElement("mousetitle");
		if (mousetitle) {
			mousetitle->setTitle(buf);
			drawElement(mousetitle);
		}

		ConsoleHelper::onMouse(me);
	}
};

class CtrlC : public CHElement {
public:
	volatile bool exitFlag;
	CtrlC() : CHElement("ctrlc", false), exitFlag(false) {

	}

	virtual bool onKey(KEY_EVENT_RECORD ke) {
		if (ke.wVirtualKeyCode == 67 && ke.dwControlKeyState & 8) {
			exitFlag = true;
		}
		return false;
	}
};


class CHMenu {
public:
	std::string name;
	std::vector<std::string> items;
	int x, y, w;

	CHMenu(std::string name_, int x_, int y_, int w_) : name(name_), x(x_), y(y_), w(w_) {

	}

	void addMenu(std::string title) {
		auto o = items.size();
		auto id = name + ":item:" + std::to_string(o);
		auto t = new CHTitle(id, x, y + (int)o, title, w, false);
		items.push_back(id);
		t->add();
	}
};

int main() {

	new MyCH();

	auto ctrlC = new CtrlC();
	ctrlC->add();

	auto box = new BoxWithVScroll("log1", 1, 3, -50, -80, true);

	auto title = new CHTitle("server", 0, 0, "ArkUpgrade server: hh-island", 50);
	title->add();

	title = new CHTitle("menu1 name", 4, 2, "Control", 20);
	title->color = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	title->add();

	title = new CHTitle("menu2 name", 4, 12, "Section actions", 20);
	title->color = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	title->add();

	auto menu = new CHMenu("menu1", 6, 4, 20);
	menu->addMenu("Inventory hooks");
	menu->addMenu("Players activity");
	menu->addMenu("line3");

	while (!ctrlC->exitFlag) {
		Sleep(300);	
		char b[100];
		sprintf_s(b, "%d", rand());		
		box->pushLine(b);
	}

	auto c = ConsoleHelper::instance;
	ConsoleHelper::instance = 0;
	delete c;

	return 0;
}
