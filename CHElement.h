#pragma once

#include "windows.h"
#include <string>

class ConsoleHelper;

class CHElement {

public:

	int ox, oy;
	int x, y, w, h;

	bool waling;
	bool stickRight;
	bool isRender;
	int color;
	CHElement() {
		isRender = false;
	}

	CHElement(int x_, int y_, int w_, int h_) : x(x_), ox(x_), oy(y_), y(y_), w(w_), h(h_),
		isRender(true), waling(false), stickRight(false), color(7) {
	}


	virtual bool onKey(KEY_EVENT_RECORD ke) { return false; }
	virtual void onTimer() {}

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

	virtual void lostFocus() { }
	virtual void setFocus() { }

	virtual void onWHeeled(int dir) {}

	void add();
	void update();
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
