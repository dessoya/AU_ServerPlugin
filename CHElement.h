#pragma once

#include "windows.h"
#include <string>
#include "Lock.h"
#include <vector>

class CHElement {

public:
	Lock lock_;
	bool isRender;
	std::string name;

	CHElement(std::string, bool isRender_ = false);
	virtual ~CHElement();

	virtual void onTimer();
	virtual bool onKey(KEY_EVENT_RECORD ke);
	virtual void onResize(int cw, int ch);
	void lock() { lock_.lock(); }
	void unlock() { lock_.unlock(); }

	void add();
};

class CHRenderElement : public CHElement  {

public:

	int ox, oy;
	int x, y, w, h;

	bool waling;
	bool stickRight;
	int color;

	CHRenderElement(std::string, int x_, int y_, int w_, int h_, bool stickRight_ = false);
	virtual ~CHRenderElement();
	
	virtual std::string getLine(int idx, bool *);
	virtual void onResize(int cw, int ch);
	bool isArea(int hx, int hy);

	virtual void lostFocus();
	virtual void setFocus();
	virtual void onMouse(MOUSE_EVENT_RECORD me);
	virtual void onWHeeled(int dir);

	void update();
};


class CHBox : public CHRenderElement {
public:
	int sw, sh;
	CHBox(std::string, int x_, int y_, int w_, int h_, bool stickRight_ = false);
	virtual std::string getLine(int idx, bool *nc);
	virtual void onResize(int cw, int ch);
};

class VScrollChangeReceiver {
public:
	virtual void onVScrollChange(int pos) = 0;
};

class VScroll : public CHBox {
public:
	VScrollChangeReceiver  *onChangeReceiver;
	int total, pos, visible, ssize, sstart;

	VScroll(std::string, int x_, int y_, int h_, bool stickRight_, int total_, int pos_, int visible_);
	void changePos(int npos);
	virtual void onResize(int cw, int ch);
	void updateRegions();
	virtual void onMouse(MOUSE_EVENT_RECORD me);
	virtual std::string getLine(int idx, bool *nc);
};

class CHBoxVScroll : public CHBox, public VScrollChangeReceiver {
public:
	VScroll *vscroll;
	int pos;
	std::vector<std::string> lines;

	CHBoxVScroll(std::string, int x_, int y_, int w_, int h_, bool stickRight_, VScroll *vscroll_);
	void pushLine(std::string line);
	virtual void onWHeeled(int dir);
	virtual void onVScrollChange(int npos);
	virtual std::string getLine(int idx, bool *nc);
};

class CHTitle : public CHRenderElement {
	std::string title;
	int ocolor;
public:

	CHTitle(std::string, int x_, int y_, std::string title_, int w_, bool stickRight_ = false);
	virtual std::string getLine(int idx, bool *nc);
	void setTitle(std::string title_);
	virtual void lostFocus();
	virtual void setFocus();
	virtual void onWHeeled(int dir);
};



class BoxWithVScroll {
public:
	std::string name;

	BoxWithVScroll(std::string name_, int x, int y, int w, int h, bool stickRight);
	void pushLine(std::string line);
};
