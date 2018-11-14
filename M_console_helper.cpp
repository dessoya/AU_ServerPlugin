#include <API/ARK/Ark.h>
#include "M_console_helper.h"
#include "ConsoleHelper.h"
#include "M_hook_helper.h"
#include "M_timer.h"
#include "processthreadsapi.h"

class MyCH : public ConsoleHelper {

	CHTitle *keytitle, *mousetitle;

public:

	MyCH() : ConsoleHelper() {

		keytitle = new CHTitle(0, 0, "", 40);
		keytitle->waling = true;
		keytitle->stickRight = true;
		addElement(keytitle);

		mousetitle = new CHTitle(0, 1, "", 40);
		mousetitle->waling = true;
		mousetitle->stickRight = true;
		addElement(mousetitle);

	}

	virtual void onKey(KEY_EVENT_RECORD ke) {

		char buf[130];
		sprintf_s(buf, "кнопка %d %s state %d", ke.wVirtualKeyCode, (ke.bKeyDown ? "нажата" : "отжата"), ke.dwControlKeyState);
		// sprintf_s(buf, "key %d %s state %d", ke.wVirtualKeyCode, (ke.bKeyDown ? "pressed" : "unpressed"), ke.dwControlKeyState);
		keytitle->setTitle(buf);
		drawElement(keytitle);

		ConsoleHelper::onKey(ke);


		/*
		if (ke.wVirtualKeyCode == 67 && ke.dwControlKeyState & 8) {
			exit(1);
		}
		*/
	}

	virtual void onMouse(MOUSE_EVENT_RECORD me) {

		/*
		me.dwEventFlags == MOUSE_WHEELED
		tt > 0 or < 0 = direction
		*/

		
		char buf[130];
		short *w = (short *)&me.dwButtonState;
		int tt = w[1];
		sprintf_s(buf, "x %d y %d f %d s %d", me.dwMousePosition.X, me.dwMousePosition.Y, me.dwEventFlags, tt);
		mousetitle->setTitle(buf);
		drawElement(mousetitle);
		

		ConsoleHelper::onMouse(me);

	}
};

class CtrlC : public CHElement {
public:
	volatile bool exitFlag;
	CtrlC() : CHElement(), exitFlag(false) {

	}

	virtual bool onKey(KEY_EVENT_RECORD ke) {
		if (ke.wVirtualKeyCode == 67 && ke.dwControlKeyState & 8) {
			exitFlag = true;
		}
		return false;
	}
};

CtrlC *ctrlC = 0;

bool bFirstTime, exited;
void __Timer2() {
	if (bFirstTime && ConsoleHelper::instance) {
		bFirstTime = false;
		ConsoleHelper::instance->clear();
	}
	if (!exited && ctrlC && ctrlC->exitFlag) {
		exited = true;
		if (!bFirstTime) {
			Log::GetLog()->info("Saving world before reloading plugins ...");
			ArkApi::GetApiUtils().GetShooterGameMode()->SaveWorld();
			Log::GetLog()->info("World saved.");
		}

		// ArkApi::GetApiUtils().GetWorld()->

		Log::GetLog()->info("Exit."); 
		throw std::runtime_error("Exit");
		/*
		FreeConsole();
		ExitProcess(0);
		exit(0);
		*/
		// char *a = 0; *a = 0;
	}
}

DECLARE_HOOK(UWorld_InitWorld, void, UWorld*, DWORD64);
void Hook_UWorld_InitWorld(UWorld *w, DWORD64 p) {
	UWorld_InitWorld_original(w, p);
	
	if (ConsoleHelper::instance) {
		ConsoleHelper::instance->clear();
	}
}


class SinChar : public CHElement {
public:
	std::string c;
	__time cur_time;
	double pos;
public:
	SinChar(int x_, std::string c_, int p) : CHElement(x_, 0, 1, 1), c(c_) {
		cur_time = std::chrono::system_clock::now();
		pos = p * 10;
	}

	virtual std::string getLine(int idx) {
		if (idx == 0) return c;
		return "";
	}

	virtual void onTimer() {
		auto onu = std::chrono::system_clock::now();
		auto workedtime = std::chrono::duration_cast<std::chrono::milliseconds>(onu - cur_time);
		pos += workedtime.count();
		cur_time = onu;
		auto y_ = (sin(pos / 500.0) * (((double)ConsoleHelper::instance->h - 4.0) / 2)) + ((double)ConsoleHelper::instance->h / 2.0);
		ConsoleHelper::instance->moveElement(this, x, (int)y_);
	}
};

SinChar *s1 = 0;

bool M_console_helper_init() {

	bFirstTime = true;
	exited = false;
	new MyCH();
	ctrlC = new CtrlC();
	ctrlC->add();

	char *s1 = "ArkUpgrade Server HH island";
	char bb[2];
	bb[1] = 0;

	for (int i = 0, l = strlen(s1); i < l; i++) {
		bb[0] = s1[i];
		auto s2 = new SinChar(10 + i*2, bb, i);
		s2->add();
	}



	ArkApi::GetCommands().AddOnTimerCallback("__Timer2", &__Timer2);

	__register_hook(UWorld, InitWorld);

	return true;
}
bool M_console_helper_done() {

	ArkApi::GetCommands().RemoveOnTimerCallback("__Timer2");
	__unregister_hook(UWorld, InitWorld);

	ctrlC = 0;

	auto c = ConsoleHelper::instance;
	ConsoleHelper::instance = 0;
	delete c;

	return true;
}

