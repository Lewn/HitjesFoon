#include "widgets/WidgetHome.h"

WidgetHome::WidgetHome(const WEnvironment& env, GUI *gui, Persistence *persistence) : WApplication(env), gui(gui), persistence(persistence), logUpdate(NULL) {
    setTitle("HitjesFoon");
    enableUpdates(true);

    logContainer = new WContainerWidget();
    updateLog();

    home = new WFilledTemplate(WString::tr("template-hitjesfoon-home"), root());
    home->bindWidget("log", logContainer);

    // TODO clean new pointers (use shared_ptr?)
    home->bindWidget("template-hitjesfoon-home-input", new WHitSearch(gui, persistence));
    home->bindWidget("template-hitjesfoon-home-playing", new WPlaylist(gui, persistence));
    home->bindWidget("template-hitjesfoon-home-playback-controls", new WPlaybackControls(gui, persistence));

    logUpdate = new WSound("resources/sounds/logUpdate.mp3");


    // Listen to all persistence changes
    persistence->onChangeCallback(boost::bind(&WidgetHome::onPersistenceChange, this, _1));
}

WidgetHome::~WidgetHome() {
    SAFE_DELETE(logUpdate);
}


void WidgetHome::onPersistenceChange(const string &key) {
    if (key == "log") {
//        logText->setText(persistence->getStringData().getVal(key));
//        logUpdate->play();
    } else if (key == "logv") {
        updateLog();
    } else {
        printf("Persistence change key %s\n", key.c_str());
    }
    triggerUpdate();
}

void WidgetHome::updateLog() {
    const vector<string> logv = persistence->getStringVectorData().getVal("logv");
    unsigned int logcount = logContainer->count();
    if (logcount != 0) {
        // Two widgets might have changed, force update on both
        logContainer->removeWidget(logContainer->widget(0));
        logcount--;
    }
    for (unsigned int i = logcount; i < logv.size(); i++) {
        WText *textEl = new WText(WString(logv[i]));
        textEl->addStyleClass(WString("list-group-item"));
        logContainer->insertWidget(0, textEl);
    }
}


