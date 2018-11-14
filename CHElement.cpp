#include "CHElement.h"
#include "ConsoleHelper.h"

void CHElement::update() {
	if (ConsoleHelper::instance) {
		ConsoleHelper::instance->drawElement(this);
	}
}

void CHElement::add() {
	if (ConsoleHelper::instance) {
		ConsoleHelper::instance->addElement(this);
	}
}