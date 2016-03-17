#include "gui/GUI.h"

GUI::GUI(PRINT_LEVEL msglevel) : msglevel(msglevel) {
}

void GUI::setMsglevel(PRINT_LEVEL msglevel) {
    this->msglevel = msglevel;
}

PRINT_LEVEL GUI::getMsglevel() {
    return msglevel;
}

GUIEvent &GUI::events() {
    return guiEvents;
}


void GUI::setHitjes(const vector<Hitje *> &hitjes) {
    this->hitjes = hitjes;
}

const vector<Hitje *> &GUI::getHitjes() {
    return hitjes;
}


void GUI::inputPhoneNum(int num) {
    events().inputPhoneNumSig(num);
}

void GUI::inputPhoneAlt(int alt) {
    events().inputPhoneAltSig(alt);
}

void GUI::setPlaybackState(PlaybackState state) {
    events().playbackSig(state);
}
