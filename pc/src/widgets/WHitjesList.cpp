#include "widgets/WHitjesList.h"

WHitjesList::WHitjesList(GUI &gui, Persistence &persistence, WContainerWidget *parent) : WHitjesList(gui, persistence, WString::tr("template-hitjesfoon-hitjeslist"), parent) {}

WHitjesList::WHitjesList(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent) : WFilledTemplate(text, parent), gui(gui), persistence(persistence) {
}

WHitjesList::~WHitjesList() {

}

void WHitjesList::buildWidget() {
    // TODO button to download all hitjes
    hitjesContainer = new WContainerWidget();
    bindWidget("hitjeslist", hitjesContainer);
    for (unsigned int i = 1; i < 1000; i++) {
        WHitje *whitje = new WHitje(gui, persistence, i);
        whitje->buildWidget();
        hitjesContainer->addWidget(whitje);
    }

    WPushButton *downloadAllBtn = new WPushButton(WString::tr("strings-hitjeslist-downloadall"));
    bindWidget("downloadall", downloadAllBtn);

    downloadAllBtn->clicked().connect(this, &WHitjesList::downloadAll);

    // Listen to all persistence changes
    persistence.onChangeCallback(boost::bind(&WHitjesList::onPersistenceChange, this, _1));
}


void WHitjesList::onPersistenceChange(const string &key) {
}

void WHitjesList::downloadAll() {
    // Perform threaded, as downloading may take a very long time
    // As not to block the UI
    thread([&]() {
        for (WObject *child : hitjesContainer->children()) {
            // All children are in fact whitje
            WHitje *whitje = (WHitje *)child;
            whitje->download();
        }
    }).detach();
}
