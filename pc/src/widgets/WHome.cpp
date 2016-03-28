#include "widgets/WHome.h"

WHome::WHome(GUI &gui, Persistence &persistence, WContainerWidget *parent) : WHome(gui, persistence, WString::tr("template-hitjesfoon-home"), parent) {}

WHome::WHome(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent) : WFilledTemplate(text, parent), gui(gui), persistence(persistence) {
}

WHome::~WHome() {
}


void WHome::buildWidget() {
    logContainer = new WContainerWidget();
    updateLog();

    bindWidget("log", logContainer);

    hitSearch = new WHitSearch(gui, persistence);
    hitSearch->buildWidget();
    playlist = new WPlaylist(gui, persistence);
    playlist->buildWidget();
    playbackControls = new WPlaybackControls(gui, persistence);
    playbackControls->buildWidget();
    bindWidget("template-hitjesfoon-home-input", hitSearch);
    bindWidget("template-hitjesfoon-home-playing", playlist);
    bindWidget("template-hitjesfoon-home-playback-controls", playbackControls);

    logUpdate = new WSound("resources/sounds/logUpdate.mp3", this);

    // Listen to all persistence changes
    persistence.onChangeCallback(boost::bind(&WHome::onPersistenceChange, this, _1));
}


void WHome::onPersistenceChange(const string &key) {
    if (key == "log") {
//        logText->setText(persistence.getStringData().getVal(key));
//        logUpdate->play();
    } else if (key == "logv") {
        updateLog();
        WApplication::instance()->triggerUpdate();
    } else {
//        printf("Persistence change key %s\n", key.c_str());
    }
}

void WHome::updateLog() {
    const vector<string> logv = persistence.getStringVectorData().getVal("logv");
    unsigned int logcount = logContainer->count();
    if (logcount != 0) {
        // Two widgets might have changed, force update on both
        WWidget *headWidget = logContainer->widget(0);
        logContainer->removeWidget(headWidget);
        SAFE_DELETE(headWidget);
        logcount--;
    }
    for (unsigned int i = logcount; i < logv.size(); i++) {
        WText *textEl = new WText(WString(logv[i]));
        textEl->addStyleClass(WString("list-group-item"));
        logContainer->insertWidget(0, textEl);
    }
}


