#include <API/ARK/Ark.h>
#include "M_console_helper.h"
#include "ConsoleHelper.h"
#include "M_hook_helper.h"
#include "M_timer.h"
#include "M_config_reader.h"
#include "processthreadsapi.h"
#include "Messages.h"

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



void installControls() {

	auto t1 = new CHTitle("menu1 name", 4, 2, "Control", 20);
	t1->color = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	t1->add();

	t1 = new CHTitle("menu2 name", 4, 12, "Section actions", 20);
	t1->color = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	t1->add();

	auto box = new BoxWithVScroll("log1", 1, 3, -50, -80, true);
	box->add();

	auto menu = new CHMenu("menu1", 6, 4, 20);
	menu->addMenu("Players activity");
	menu->addMenu("Players events");
	menu->addMenu(SN_Inventory);
	menu->add();

	auto mt = new Menu1Treator("mt1", menu, box);
	mt->addLinesHolder(SN_Inventory);
	mt->addLinesHolder("Players");
	mt->add();

	auto title = new CHTitle("section_info0", 4, 14, "", 30);
	title->add();
	title = new CHTitle("section_info1", 4, 15, "", 30);
	title->add();

	auto s1 = new InventorySection(mt->getLinesHolder(SN_Inventory));
	mt->addSection(s1);

	// mt->pushLine("Inventory", b);
}

char buf[4096];
void _l(std::string &sn, const char * format, ...) {
	auto mt = (Menu1Treator *)ConsoleHelper::_getElement("mt1");
	if (mt) {
		auto lh = mt->getLinesHolder(sn);
		if (lh) {
			
			va_list argptr;
			va_start(argptr, format);
			vsprintf(buf, format, argptr);
			va_end(argptr);

			lh->pushLine(buf);
		}
	}
}

bool bFirstTime, exited;
void __Timer2() {

	if (bFirstTime && ConsoleHelper::instance) {
		bFirstTime = false;
		ConsoleHelper::instance->clear();
		auto e = ConsoleHelper::instance->getElement("loading");
		ConsoleHelper::instance->delElement(e);
		installControls();
		_msg_server_config();
	}

	auto ctrlC = (CtrlC *)ConsoleHelper::_getElement("ctrlc");
	if (!exited && ctrlC && ctrlC->exitFlag) {
		exited = true;
		if (!bFirstTime) {
			Log::GetLog()->info("Saving world before reloading plugins ...");
			ArkApi::GetApiUtils().GetShooterGameMode()->SaveWorld();
			Log::GetLog()->info("World saved.");
		}

		Log::GetLog()->error("Exit");
		throw std::runtime_error("Exit");
	}
}

DECLARE_HOOK(UWorld_InitWorld, void, UWorld*, DWORD64);
void Hook_UWorld_InitWorld(UWorld *w, DWORD64 p) {
	UWorld_InitWorld_original(w, p);
	
	if (ConsoleHelper::instance) {
		ConsoleHelper::instance->clear();
	}

	_msg_server_config();
}



bool M_console_helper_init() {

	
	exited = false;

	new MyCH();

	auto ctrlC = new CtrlC();
	ctrlC->add();

	auto t1 = new CHTitle("title", 0, 0, std::string("ArkUpgrade server:"), 18, false);
	t1->add();

	t1 = new CHTitle("title2", 19, 0, config->getString("serverName"), 30, false);
	t1->color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
	t1->add();
	
	if (ArkApi::GetApiUtils().GetWorld()) {
		bFirstTime = false;
		installControls();
	}
	else {
		bFirstTime = true;
		t1 = new CHTitle("loading", 0, 2, std::string("Loading . . "), 40, false);
		t1->add();
	}
	

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

	return true;
}

