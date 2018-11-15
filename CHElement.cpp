#include "pch.h"
#include "CHElement.h"
#include "ConsoleHelper.h"
//#include <API/ARK/Ark.h>


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

void CHTitle::onWHeeled(int dir) {
	/*
	if (dir > 0) {
		color++;
	}
	else {
		color--;
	}
	update();
	*/
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
		// Log::GetLog()->info("BoxWithVScroll 1");

		auto vs = new VScroll(name + ":vs", x, y, h, true, 1, 0, 1);
		// Log::GetLog()->info("BoxWithVScroll 2");
		vs->add();
		// Log::GetLog()->info("BoxWithVScroll 3");

		auto box = new CHBoxVScroll(name + ":box", x + 1, y, w, h, true, vs);
		// box->color = 0xf8;
		box->add();
		vs->onChangeReceiver = box;
	}
}

CHBoxVScroll *BoxWithVScroll::getBox() {
	return (CHBoxVScroll *)ConsoleHelper::_getElement(name + ":box");
}


/*
void BoxWithVScroll::pushLine(std::string line) {
	CHBoxVScroll *box = (CHBoxVScroll *)ConsoleHelper::_getElement(name + ":box");
	if (box) box->pushLine(line);
}
*/



LinesHolder::LinesHolder() : receiver(0) {

}

void LinesHolder::pushLine(std::string line) {
	lines.push_back(line);
	if (receiver) receiver->onPushLine();
}

void LinesHolder::clear() {
	lines.clear();
	if (receiver) receiver->onClearLines();

}
