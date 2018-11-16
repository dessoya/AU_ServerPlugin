
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




class CHMenu : public CHElement, public AreaClickReceiver {
public:
	std::vector<std::string> items;
	int x, y, w;
	bool markerCreated;
	std::string currentItem;
	MenuItemClickReceiver *menuItemClickReceiver;

	CHMenu(std::string name_, int x_, int y_, int w_) : CHElement(name_), x(x_), y(y_), w(w_) {
		markerCreated = false;
		menuItemClickReceiver = 0;
	}

	void addMenu(std::string title) {
		auto o = items.size();
		auto id = name + ":item:" + std::to_string(o);
		auto t = new CHTitle(id, x, y + (int)o, title, w, false);
		t->areaClickReceiver = this;
		items.push_back(id);
		t->add();
	}

	virtual void onAreaClick(CHElement *e) {
		auto r = (CHRenderElement *)e;

		if (markerCreated) {
			if (currentItem != r->name) {
				currentItem = r->name;
				auto t = (CHRenderElement *)ConsoleHelper::_getElement(name + ":marker:left");
				ConsoleHelper::instance->moveElement(t, x - 2, r->y);
				t = (CHRenderElement *)ConsoleHelper::_getElement(name + ":marker:right");
				ConsoleHelper::instance->moveElement(t, x + w + 2, r->y);
				if (menuItemClickReceiver) menuItemClickReceiver->onMenuItemClick(currentItem);
			}
		}
		else {
			markerCreated = true;
			currentItem = r->name;
			auto t = new CHTitle(name + ":marker:left", x - 2, r->y, "[", 1, false);
			t->add();

			t = new CHTitle(name + ":marker:right", x + w + 2, r->y, "]", 1, false);
			t->add();
			if (menuItemClickReceiver) menuItemClickReceiver->onMenuItemClick(currentItem);
		}
	}
};

class Section : public CHElement {
public:

	Section(std::string name_) : CHElement(name_) {

	}

	virtual void activate() {

	}

	virtual void onKeyDown(KEY_EVENT_RECORD ke) {
	}

	static void setInfo(std::string info, int n = 0) {
		auto e = (CHTitle *)ConsoleHelper::_getElement("section_info" + std::to_string(n));
		if (e) {
			e->setTitle(info);
		}
	}
};

class Menu1Treator : public CHElement, public MenuItemClickReceiver {
public:

	std::map<std::string, LinesHolder *> linesHolders;
	CHMenu *menu;
	BoxWithVScroll *box;

	Section *currentSection;
	std::map<std::string, Section *> sections;

	Menu1Treator(std::string name, CHMenu *menu_, BoxWithVScroll *box_)
		: CHElement(name, false), menu(menu_), box(box_) {

		currentSection = 0;
		menu->menuItemClickReceiver = this;
	}

	virtual bool onKey(KEY_EVENT_RECORD ke) {
		if (ke.bKeyDown) {
			if (currentSection) {
				currentSection->onKeyDown(ke);
			}
			/*
			switch (ke.wVirtualKeyCode) {


			}
			*/
		}
		return false;
	}

	void addLinesHolder(std::string name) {
		auto lh = new LinesHolder();
		linesHolders.insert(std::pair<std::string, LinesHolder *>(name, lh));
	}

	LinesHolder *getLinesHolder(std::string name) {
		auto it = linesHolders.find(name);
		if (it == linesHolders.end()) return 0;
		return it->second;
	}

	void addSection(Section *s) {
		sections.insert(std::pair<std::string, Section *>(s->name, s));
	}

	void activateSection(std::string name) {
		for(int i = 0; i < 2; i++) Section::setInfo("", i);
		Section *newSection = 0;
		auto it = sections.find(name);
		if (it != sections.end()) {
			newSection = it->second;
		}
		

		currentSection = newSection;
		if (currentSection) {
			currentSection->activate();
		}

	}

	virtual void onMenuItemClick(std::string name) {

		if(name == "menu1:item:0") {
			box->getBox()->setupLinesHolder(getLinesHolder("Inventory"));
			activateSection("Inventory");
		}
		else if (name == "menu1:item:1") {
			box->getBox()->setupLinesHolder(getLinesHolder("Players"));
			activateSection("Players");
		}
		else {
			box->getBox()->setupLinesHolder(0);
			activateSection("");
		}
	}

	void pushLine(std::string linesHolderName, std::string line) {
		auto it = linesHolders.find(linesHolderName);
		if (it != linesHolders.end()) {
			auto lh = it->second;
			lh->pushLine(line);
		}
	}
};

int main() {

	new MyCH();

	auto ctrlC = new CtrlC();
	ctrlC->add();

	auto title = new CHTitle("server", 0, 0, "ArkUpgrade server: hh-island", 50);
	title->add();

	title = new CHTitle("menu1 name", 4, 2, "Control", 20);
	title->color = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	title->add();

	title = new CHTitle("menu2 name", 4, 12, "Section actions", 20);
	title->color = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	title->add();

	auto box = new BoxWithVScroll("log1", 1, 3, -50, -80, true);
	box->add();

	auto menu = new CHMenu("menu1", 6, 4, 20);
	menu->addMenu("Inventory hooks");
	menu->addMenu("Players activity");
	menu->addMenu("line3");
	menu->add();

	auto mt = new Menu1Treator("mt1", menu, box);
	mt->addLinesHolder("Inventory");
	mt->addLinesHolder("Players");
	mt->add();

	title = new CHTitle("section_info0", 4, 14, "", 30);
	title->add();
	title = new CHTitle("section_info1", 4, 15, "", 30);
	title->add();

	class InventorySection : public Section {
	public:
		LinesHolder *linesHolder;
		bool i_state;
		InventorySection(LinesHolder *linesHolder_) : Section("Inventory"), linesHolder(linesHolder_), i_state(true) { }
		virtual void activate() {
			if (i_state) {
				setInfo("[S] stop logging");
			}
			else {
				setInfo("[S] start logging");
			}
			setInfo("[C] clear", 1);
		}
		virtual void onKeyDown(KEY_EVENT_RECORD ke) {
			if (ke.dwControlKeyState != 0) return;
			// C - key
			if (ke.wVirtualKeyCode == 67) {
				
				std::string s;
				auto it = linesHolder->lines.begin();
				while (it != linesHolder->lines.end()) {
					auto line = *it;
					s += "\n" + line;
					it++;
				}
				
				OpenClipboard(0);
				EmptyClipboard();
				HGLOBAL clipbuffer;
				char * buffer;
				clipbuffer = GlobalAlloc(GMEM_DDESHARE, s.length() + 1);
				buffer = (char*)GlobalLock(clipbuffer);
				strcpy_s(buffer, s.length() + 1, s.c_str());
				GlobalUnlock(clipbuffer);
				SetClipboardData(CF_TEXT, clipbuffer);
				CloseClipboard();

				linesHolder->clear();
			}
			// S - key
			else if (ke.wVirtualKeyCode == 83) {
				i_state = !i_state;
				activate();
			}
		}
	};

	auto s1 = new InventorySection(mt->getLinesHolder("Inventory"));
	mt->addSection(s1);


	while (!ctrlC->exitFlag) {
		Sleep(300);	
		char b[100];

		if (s1->i_state) {
			sprintf_s(b, "-- %d", rand());
			mt->pushLine("Inventory", b);
		}

		sprintf_s(b, "++ %d", rand());
		mt->pushLine("Players", b);
		
	}

	auto c = ConsoleHelper::instance;
	ConsoleHelper::instance = 0;
	delete c;

	return 0;
}
