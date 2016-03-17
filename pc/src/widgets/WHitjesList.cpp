#include "WHitjesList.h"

WHitjesList::WHitjesList(GUI &gui, Persistence &persistence, WContainerWidget *parent) : WFilledTemplate(WString::tr("template-hitjesfoon-hitjeslist"), parent), gui(gui), persistence(persistence) {

}

WHitjesList::WHitjesList(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent) : WFilledTemplate(text, parent), gui(gui), persistence(persistence) {

}

WHitjesList::~WHitjesList() {

}

void WHitjesList::buildWidget() {

}
