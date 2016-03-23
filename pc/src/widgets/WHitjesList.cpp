#include "widgets/WHitjesList.h"

WHitjesList::WHitjesList(GUI &gui, Persistence &persistence, WContainerWidget *parent) : WHitjesList(gui, persistence, WString::tr("template-hitjesfoon-hitjeslist"), parent) {}

WHitjesList::WHitjesList(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent) : WFilledTemplate(text, parent), gui(gui), persistence(persistence) {
    buildWidget();
}

WHitjesList::~WHitjesList() {

}

void WHitjesList::buildWidget() {
    // TODO button to download all hitjes
    hitjesContainer = new WContainerWidget();
    bindWidget("hitjeslist", hitjesContainer);
    for (unsigned int i = 1; i < 1000; i++) {
        hitjesContainer->addWidget(new WHitje(gui, persistence, i));
    }

    // Listen to all persistence changes
    persistence.onChangeCallback(boost::bind(&WHitjesList::onPersistenceChange, this, _1));
}


void WHitjesList::onPersistenceChange(const string &key) {
}
