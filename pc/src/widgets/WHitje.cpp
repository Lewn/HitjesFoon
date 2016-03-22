#include "WHitje.h"

WHitje::WHitje(GUI &gui, Persistence &persistence, int hitIndex, WContainerWidget *parent) : WFilledTemplate(WString::tr("template-hitjesfoon-hitjeslist-hitje"), parent), gui(gui), persistence(persistence), hitIndex(hitIndex) {
    buildWidget();
}

WHitje::WHitje(GUI &gui, Persistence &persistence, const WString &text, int hitIndex, WContainerWidget *parent) : WFilledTemplate(text, parent), gui(gui), persistence(persistence), hitIndex(hitIndex) {
    buildWidget();
}

WHitje::~WHitje() {
}

void WHitje::buildWidget() {
    stringstream hitIndexStr;
    hitIndexStr << setfill('0') << setw(3) << hitIndex;
    bindString("hitje-num", hitIndexStr.str());

    artistText = new WText();
    titleText = new WText();
    infoText = new WText();
    pgBar = new WProgressBar();
    pgBar->setRange(0, 100);

    updateHitje();

    bindWidget("hitje-artist", artistText);
    bindWidget("hitje-title", titleText);
    bindWidget("hitje-info", infoText);
    bindWidget("hitje-bar", pgBar);

    // Listen to all persistence changes
    persistence.onChangeCallback(boost::bind(&WHitje::onPersistenceChange, this, _1));
}


void WHitje::onPersistenceChange(const string &key) {
    if (key == to_string(hitIndex)) {
        // This hitje updated, also update widget
        updateHitje();
        WApplication::instance()->triggerUpdate();
    }
}

void WHitje::updateHitje() {
    const Hitje &hitje = persistence.getHitjeData().getVal(to_string(hitIndex));
    const DownloadState &dlstate = hitje.downloadState;
    pgBar->setValue(dlstate.percentage);

    artistText->setText(WString(hitje.artist));
    titleText->setText(WString(hitje.title));

    pgBar->setHidden(!dlstate.downloading);
    infoText->setHidden(dlstate.downloading);
    // TODO more info
    if (!hitje && !dlstate.downloading) {
        // hitje unavailable and not downloading, notify user
        // TODO add button to start downloading
        infoText->setText(WString("Hitje not available"));
    } else {
        infoText->setText(WString(""));
    }
}
