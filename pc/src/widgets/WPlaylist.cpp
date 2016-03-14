#include "widgets/WPlaylist.h"

WPlaylist::WPlaylist(GUI *gui, Persistence *persistence, WContainerWidget *parent) : WFilledTemplate(WString::tr("template-hitjesfoon-home-playing"), parent), gui(gui), persistence(persistence) {
    buildWidget();
}

WPlaylist::WPlaylist(GUI *gui, Persistence *persistence, const WString &text, WContainerWidget *parent) : WFilledTemplate(text, parent), gui(gui), persistence(persistence) {
    buildWidget();
}

WPlaylist::~WPlaylist() {

}

void WPlaylist::buildWidget() {
    currentlyTypingText = new WText();
    currentlyPlayingText = new WText();
    playlistText = new WText();

    bindWidget("currently-typing", currentlyTypingText);
    bindWidget("currently-playing", currentlyPlayingText);
    bindWidget("playlist", playlistText);

    // Listen to all persistence changes
    persistence->onChangeCallback(boost::bind(&WPlaylist::onPersistenceChange, this, _1));
}


void WPlaylist::onPersistenceChange(const string &key) {
    if (key == "phone-num") {
        currentlyTypingText->setText(to_string(persistence->getIntData().getVal(key)));
    } else if (key == "phone-playlist") {
        updatePlaylist();
    }
}

void WPlaylist::updatePlaylist() {
    gui->printlevel(LINFO, "Updating playlist\n");
    string buf;
    const vector<int> playlist = persistence->getIntVectorData().getVal("phone-playlist");

    InputProcessor *processor = InputProcessor::getInstance();
    AudioList *audioList = processor->getHitjesList();
    const vector<Hitje *> hitjes = audioList->getHitjes();

    for (unsigned int i = 1; i < playlist.size(); i++) {
        buf += "<div>" + hitjes[playlist[i]]->toString() + "</div>";
    }
    playlistText->setText(WString(buf));

    if (playlist.size() != 0) {
        gui->printlevel(LINFO, "Set currently playing to %s\n", hitjes[playlist[0]]->toString().c_str());
        currentlyPlayingText->setText(WString(hitjes[playlist[0]]->toString()));
    } else {
        gui->printlevel(LINFO, "Emptied currently playing\n");
        currentlyPlayingText->setText(WString());
    }
    gui->printlevel(LINFO, "Updated playlist text\n");
}

