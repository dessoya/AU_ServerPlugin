#include <API/ARK/Ark.h>
#include "M_console_helper.h"
#include "ConsoleHelper.h"
#include "M_hook_helper.h"
#include "M_timer.h"
#include "M_config_reader.h"
#include "processthreadsapi.h"

BoxWithVScroll *logBox;

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

CtrlC *ctrlC = 0;

bool bFirstTime, exited;
void __Timer2() {
	if (bFirstTime && ConsoleHelper::instance) {
		bFirstTime = false;
		ConsoleHelper::instance->clear();
		auto e = ConsoleHelper::instance->getElement("loading");
		ConsoleHelper::instance->delElement(e);
	}
	if (!exited && ctrlC && ctrlC->exitFlag) {
		exited = true;
		if (!bFirstTime) {
			Log::GetLog()->info("Saving world before reloading plugins ...");
			ArkApi::GetApiUtils().GetShooterGameMode()->SaveWorld();
			Log::GetLog()->info("World saved.");
		}

		Log::GetLog()->error("Exit");

		// ArkApi::GetApiUtils().GetWorld()->
		// Log::GetLog()->info("Exit."); 
		// throw std::runtime_error("Exit");
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



bool M_console_helper_init() {

	bFirstTime = true;
	exited = false;

	new MyCH();
	ctrlC = new CtrlC();
	ctrlC->add();

	auto t1 = new CHTitle("title", 0, 0, std::string("ArkUpgrade server:"), 18, false);
	t1->add();

	t1 = new CHTitle("title2", 19, 0, config->getString("serverName"), 30, false);
	t1->color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
	t1->add();
	
	if (ArkApi::GetApiUtils().GetWorld()) {

	}
	else {
		t1 = new CHTitle("loading", 0, 2, std::string("Loading . . "), 40, false);
		t1->add();
	}

	logBox = new BoxWithVScroll("log1", 1, 3, -50, -80, true);

	t1 = new CHTitle("menu1 name", 4, 2, "Control", 20);
	t1->color = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	t1->add();

	t1 = new CHTitle("menu2 name", 4, 12, "Section actions", 20);
	t1->color = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	t1->add();


	ArkApi::GetCommands().AddOnTimerCallback("__Timer2", &__Timer2);

	__register_hook(UWorld, InitWorld);

	return true;
}
bool M_console_helper_done() {

	ArkApi::GetCommands().RemoveOnTimerCallback("__Timer2");
	__unregister_hook(UWorld, InitWorld);

	auto c = ConsoleHelper::instance;
	ConsoleHelper::instance = 0;
	delete c;

	delete logBox;

	return true;
}

