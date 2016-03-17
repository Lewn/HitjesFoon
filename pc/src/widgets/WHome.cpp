#include "widgets/WHome.h"

WHome::WHome(GUI &gui, Persistence &persistence, WContainerWidget *parent) : WFilledTemplate(WString::tr("template-hitjesfoon-home"), parent), gui(gui), persistence(persistence) {
    buildWidget();
}

WHome::WHome(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent) : WFilledTemplate(text, parent), gui(gui), persistence(persistence) {
    buildWidget();
}


void WHome::buildWidget() {
    logContainer = new WContainerWidget();
    updateLog();

    bindWidget("log", logContainer);

    hitSearch = new WHitSearch(gui, persistence);
    playlist = new WPlaylist(gui, persistence);
    playbackControls = new WPlaybackControls(gui, persistence);
    bindWidget("template-hitjesfoon-home-input", hitSearch);
    bindWidget("template-hitjesfoon-home-playing", playlist);
    bindWidget("template-hitjesfoon-home-playback-controls", playbackControls);

    logUpdate = new WSound("resources/sounds/logUpdate.mp3");

    // Listen to all persistence changes
    persistence.onChangeCallback(boost::bind(&WHome::onPersistenceChange, this, _1));
}

WHome::~WHome() {
    SAFE_DELETE(playbackControls);
    SAFE_DELETE(playlist);
    SAFE_DELETE(hitSearch);
    SAFE_DELETE(logUpdate);
}


void WHome::onPersistenceChange(const string &key) {
    if (key == "log") {
//        logText->setText(persistence.getStringData().getVal(key));
//        logUpdate->play();
    } else if (key == "logv") {
        updateLog();
    } else {
        printf("Persistence change key %s\n", key.c_str());
    }
}

void WHome::updateLog() {
    const vector<string> logv = persistence.getStringVectorData().getVal("logv");
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


