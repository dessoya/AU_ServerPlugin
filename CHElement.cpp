#include "pch.h"
#include "CHElement.h"
#include "ConsoleHelper.h"
// #include <API/ARK/Ark.h>


CHElement::CHElement(std::string name_, bool isRender_) : name(name_), isRender(isRender_) {
}

CHElement::~CHElement() {

}

void CHElement::onTimer() {
}
bool CHElement::onKey(KEY_EVENT_RECORD ke) {
	return false;
}

void CHElement::onResize(int cw, int ch) {

}

void CHElement::add() {
	if (ConsoleHelper::instance) {
		ConsoleHelper::instance->addElement(this);
	}
}



// ----------------------------------------------------------------------------------



CHRenderElement::CHRenderElement(std::string name_, int x_, int y_, int w_, int h_, bool stickRight_) : CHElement(name_, true),
	x(x_), ox(x_), oy(y_), y(y_), w(w_), h(h_), waling(false), stickRight(stickRight_), color(7) {
}

CHRenderElement::~CHRenderElement() {

}


void CHRenderElement::update() {
	if (ConsoleHelper::instance) {
		ConsoleHelper::instance->drawElement(this);
	}
}

std::string CHRenderElement::getLine(int idx, bool *nc) {
	return "";
}

void CHRenderElement::onResize(int cw, int ch) {
	if (stickRight) {
		lock();
		x = cw - w - ox;
		unlock();
	}
}

bool CHRenderElement::isArea(int hx, int hy) {
	if (hx >= x && hx < x + w && hy >= y && hy < y + h) {
		return true;
	}
	return false;
}

void CHRenderElement::lostFocus() {
}

void CHRenderElement::setFocus() {
}

void CHRenderElement::onWHeeled(int dir) {
}

void CHRenderElement::onMouse(MOUSE_EVENT_RECORD me) {

}


// --------------------------------------------------------------------------------------------


CHBox::CHBox(std::string name_, int x_, int y_, int w_, int h_, bool stickRight_)
	: CHRenderElement(name_, x_, y_, 1, 1, stickRight_), sw(w_), sh(h_) {
}

std::string CHBox::getLine(int idx, bool *nc) {
	return "..................................................................";
}

void CHBox::onResize(int cw, int ch) {

	lock();

	if (sw > 0) {
		w = sw;
	}
	else {
		double n = (double)cw;
		n = n * -1 * (double)sw / 100;
		w = (int)n;
		// if (w + x > cw) w = cw - x;
	}

	if (sh > 0) {
		h = sh;
	}
	else {
		double n = (double)ch;
		n = n * -1 * (double)sh / 100;
		h = (int)n;
		if (h + y > ch) h = ch - y;
	}

	unlock();

	CHRenderElement::onResize(cw, ch);
}


// --------------------------------------------------------------------------------------------


VScroll::VScroll(std::string name_, int x_, int y_, int h_, bool stickRight_, int total_, int pos_, int visible_)
	: CHBox(name_, x_, y_, 1, h_, stickRight_), total(total_), pos(pos_), visible(visible_) {
	onChangeReceiver = 0;
	ssize = 1;
	sstart = 0;
}

void VScroll::forceSetPos(int npos) {
	lock();
	pos = npos;
	unlock();
	updateRegions();
	update();
	if (onChangeReceiver) {
		onChangeReceiver->onVScrollChange(pos);
	}
}

void VScroll::changePos(int npos) {
	if (npos < 0) npos = 0;
	if (npos > total - visible) npos = total - visible;
	if (npos == pos) return;
	forceSetPos(npos);
}

void VScroll::onResize(int cw, int ch) {
	CHBox::onResize(cw, ch);

	lock();
	visible = h;
	if (pos > total - visible) {
		pos = total - visible;
	}
	if (pos < 0) pos = 0;
	unlock();

	updateRegions();
}

void VScroll::updateRegions() {
	lock();

	double t = (double)h - 2.0;

	ssize = (int)ceil(t / ((double)total / (double)visible));
	if (ssize < 1) ssize = 1;

	sstart = (int)ceil(t / ((double)total / (double)pos));
	if (sstart < 0) sstart = 0;

	unlock();
}

void VScroll::onMouse(MOUSE_EVENT_RECORD me) {
	if (me.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
		auto oy = me.dwMousePosition.Y - y;
		if (oy > 0 && oy < h - 1) {
			auto np = oy - 1 - ssize / 2;
			if (np < 0) np = 0;
			if (np > h - 2 - ssize) np = h - 2 - ssize;
			int npos = (int)ceil((double)(np) * ((double)(total - visible) / (double)(h - 2 - ssize)));
			if (npos > total - visible) {
				npos = total - visible;
			}
			changePos(npos);
		}
	}
}

std::string VScroll::getLine(int idx, bool *nc) {
	*nc = false;
	if (idx == 0) {
		return "\xb2\x25";
	}
	if (idx == h - 1) {
		return "\xbc\x25";
	}
	if (idx >= sstart + 1 && idx < sstart + ssize) {
		return "\x88\x25";
	}
	return "\x91\x25";
}



// --------------------------------------------------------------------------------------------



CHBoxVScroll::CHBoxVScroll(std::string name_, int x_, int y_, int w_, int h_, bool stickRight_, VScroll *vscroll_)
	: CHBox(name_, x_, y_, w_, h_, stickRight_), vscroll(vscroll_) {
	pos = 0;
	linesHolder = 0;
	vscroll->total = 1;
	vscroll->pos = 0;
	vscroll->updateRegions();
}

void CHBoxVScroll::setupLinesHolder(LinesHolder *lh) {
	
	if (linesHolder) linesHolder->receiver = 0;
	linesHolder = lh;
	if (linesHolder) linesHolder->receiver = this;

	pos = 0;
	int total = linesHolder ? (int)linesHolder->lines.size() : 1;
	
	vscroll->lock();
	vscroll->total = total;
	vscroll->unlock();

	vscroll->forceSetPos(0);
	vscroll->update();

	update();
}

void CHBoxVScroll::onPushLine() {

	bool lastPos = vscroll->total == vscroll->visible + vscroll->pos;

	vscroll->lock();
	vscroll->total = (int)linesHolder->lines.size();
	vscroll->unlock();

	if (lastPos) {
		vscroll->changePos(vscroll->total - vscroll->visible);
	}
	else {
		vscroll->updateRegions();
	}
	vscroll->update();

	if (linesHolder->lines.size() > pos && linesHolder->lines.size() < vscroll->visible + vscroll->pos) {
		update();
	}
}

void CHBoxVScroll::onPopLine() {

	bool lastPos = vscroll->total == vscroll->visible + vscroll->pos;

	vscroll->lock();
	vscroll->total = (int)linesHolder->lines.size();
	if (vscroll->pos > vscroll->total - vscroll->visible) vscroll->pos = vscroll->total - vscroll->visible;
	vscroll->unlock();
	
	vscroll->updateRegions();
	vscroll->update();

	update();

}

void CHBoxVScroll::onClearLines() {
	vscroll->lock();
	vscroll->total = 1;
	vscroll->unlock();
	vscroll->forceSetPos(0);
	vscroll->update();
	update();
}


void CHBoxVScroll::onWHeeled(int dir) {
	if (dir > 0) {
		if (vscroll->pos > 0) {
			vscroll->changePos(vscroll->pos - 1);
		}
	}
	else {
		if (vscroll->pos + vscroll->visible < vscroll->total) {
			vscroll->changePos(vscroll->pos + 1);
		}
	}
	pos = vscroll->pos;
	update();
}

void CHBoxVScroll::onVScrollChange(int npos) {
	pos = npos;
	update();
}

std::string CHBoxVScroll::getLine(int idx, bool *nc) {
	if (linesHolder) {
		int p = idx + pos;
		if (p >= 0 && p < linesHolder->lines.size()) {
			return linesHolder->lines[p];
		}
	}
	return "";
}



// --------------------------------------------------------------------------------------------


CHTitle::CHTitle(std::string name_, int x_, int y_, std::string title_, int w_, bool stickRight_)
	: CHRenderElement(name_, x_, y_, w_, 1, stickRight_), title(title_) {
	areaClickReceiver = 0;
}

std::string CHTitle::getLine(int idx, bool *nc) {
	if (idx == 0) return title;
	return "";
}

void CHTitle::setTitle(std::string title_) { title = title_; update(); }

void CHTitle::lostFocus() {
	color = ocolor;
	update();
}
void CHTitle::setFocus() {
	ocolor = color;
	color = 8;
	update();
}

void CHTitle::onMouse(MOUSE_EVENT_RECORD me) {
	if (me.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
		if (areaClickReceiver) {
			areaClickReceiver->onAreaClick(this);
		}
	}
}


// --------------------------------------------------------------------------------------------


BoxWithVScroll::BoxWithVScroll(std::string name_, int x, int y, int w, int h, bool stickRight) : CHElement(name_) {

	if (stickRight) {

		auto vs = new VScroll(name + ":vs", x, y, h, true, 1, 0, 1);
		vs->add();

		auto box = new CHBoxVScroll(name + ":box", x + 1, y, w, h, true, vs);
		box->add();
		vs->onChangeReceiver = box;
	}
}

CHBoxVScroll *BoxWithVScroll::getBox() {
	return (CHBoxVScroll *)ConsoleHelper::_getElement(name + ":box");
}



LinesHolder::LinesHolder(int limit_) : limit(limit_), receiver(0), needReceiveLines(false) {

}

void LinesHolder::pushLine(std::string line) {
	if (needReceiveLines) {
		lines.push_back(line);
		if (receiver) receiver->onPushLine();
		auto s = (int)lines.size();
		if (s > limit) {
			lines.erase(lines.begin());
			if (receiver) receiver->onPopLine();
		}
	}
}

void LinesHolder::clear() {
	lines.clear();
	if (receiver) receiver->onClearLines();

}



// ----------------------------------------------------------------------------------




CHMenu::CHMenu(std::string name_, int x_, int y_, int w_) : CHElement(name_), x(x_), y(y_), w(w_) {
	markerCreated = false;
	menuItemClickReceiver = 0;
}

void CHMenu::addMenu(std::string title) {
	auto o = items.size();
	auto id = name + ":item:" + title;
	auto t = new CHTitle(id, x, y + (int)o, title, w, false);
	t->areaClickReceiver = this;
	items.push_back(id);
	t->add();
}

void CHMenu::onAreaClick(CHElement *e) {
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



// ----------------------------------------------------------------------------------




Section::Section(std::string name_) : CHElement(name_) {

}

void Section::activate() {

}

void Section::onKeyDown(KEY_EVENT_RECORD ke) {
}

void Section::setInfo(std::string info, int n) {
	auto e = (CHTitle *)ConsoleHelper::_getElement("section_info" + std::to_string(n));
	if (e) {
		e->setTitle(info);
	}
}

// ----------------------------------------------------------------------------------


std::string SN_Inventory = "Inventory events";




Menu1Treator::Menu1Treator(std::string name, CHMenu *menu_, BoxWithVScroll *box_)
	: CHElement(name, false), menu(menu_), box(box_) {

	currentSection = 0;
	menu->menuItemClickReceiver = this;
}

bool Menu1Treator::onKey(KEY_EVENT_RECORD ke) {
	if (ke.bKeyDown) {
		if (currentSection) {
			currentSection->onKeyDown(ke);
		}		
	}
	return false;
}

void Menu1Treator::addLinesHolder(std::string name) {
	auto lh = new LinesHolder();
	linesHolders.insert(std::pair<std::string, LinesHolder *>(name, lh));
}

LinesHolder *Menu1Treator::getLinesHolder(std::string name) {
	auto it = linesHolders.find(name);
	if (it == linesHolders.end()) return 0;
	return it->second;
}

void Menu1Treator::addSection(Section *s) {
	sections[s->name] = s;
	std::string menuItemSearcher = menu->name + ":item:" + s->name;

	menuItemSections[menuItemSearcher] = s;
}

void Menu1Treator::activateSection(std::string name) {
	for (int i = 0; i < 2; i++) Section::setInfo("", i);
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

void Menu1Treator::onMenuItemClick(std::string name) {

	auto i = menuItemSections[name];

	if (i) {
		box->getBox()->setupLinesHolder(getLinesHolder(i->name));
		activateSection(i->name);
	}
	else {
		box->getBox()->setupLinesHolder(0);
		activateSection("");
	}
}

void Menu1Treator::pushLine(std::string linesHolderName, std::string line) {
	auto it = linesHolders.find(linesHolderName);
	if (it != linesHolders.end()) {
		auto lh = it->second;
		lh->pushLine(line);
	}
}




// ----------------------------------------------------------------------------------




InventorySection::InventorySection(LinesHolder *linesHolder_) : Section(SN_Inventory), linesHolder(linesHolder_) { }

void InventorySection::activate() {
	if (linesHolder->needReceiveLines) {
		setInfo("[S] stop logging");
	}
	else {
		setInfo("[S] start logging");
	}
	setInfo("[C] clear", 1);
}

void InventorySection::onKeyDown(KEY_EVENT_RECORD ke) {
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
		linesHolder->needReceiveLines = !linesHolder->needReceiveLines;
		activate();
	}
}



// ----------------------------------------------------------------------------------


