#include "WHitjesList.h"

WHitjesList::WHitjesList(GUI &gui, Persistence &persistence, WContainerWidget *parent) : WFilledTemplate(WString::tr("template-hitjesfoon-hitjeslist"), parent), gui(gui), persistence(persistence) {
    buildWidget();
}

WHitjesList::WHitjesList(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent) : WFilledTemplate(text, parent), gui(gui), persistence(persistence) {
    buildWidget();
}

WHitjesList::~WHitjesList() {

}

void WHitjesList::buildWidget() {
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
