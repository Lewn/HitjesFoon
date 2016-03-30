#include "widgets/WPlaylist.h"

WPlaylist::WPlaylist(GUI &gui, Persistence &persistence, WContainerWidget *parent) : WPlaylist(gui, persistence, WString::tr("template-hitjesfoon-home-playing"), parent) {}

WPlaylist::WPlaylist(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent) : WFilledTemplate(text, parent), gui(gui), persistence(persistence) {
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
    persistence.onChangeCallback(boost::bind(&WPlaylist::onPersistenceChange, this, _1));
    // And set initial values
    updatePlaylist();
    currentlyTypingText->setText(to_string(persistence.getIntData().getVal("phone-num")));
}


void WPlaylist::onPersistenceChange(const string &key) {
    if (key == "phone-num") {
        currentlyTypingText->setText(to_string(persistence.getIntData().getVal(key)));
        WApplication::instance()->triggerUpdate();
    } else if (key == "phone-playlist") {
        updatePlaylist();
        WApplication::instance()->triggerUpdate();
    }
}

void WPlaylist::updatePlaylist() {
    gui.printlevel(LINFO, "Updating playlist\n");
    string buf;
    const vector<int> playlist = persistence.getIntVectorData().getVal("phone-playlist");

    for (unsigned int i = 1; i < playlist.size(); i++) {
        Hitje hitje = persistence.getHitjeData().getVal(to_string(playlist[i]));
        buf += "<div>" + hitje.toString() + "</div>";
    }
    playlistText->setText(WString(buf));

    if (playlist.size() != 0) {
        Hitje hitje = persistence.getHitjeData().getVal(to_string(playlist[0]));
        gui.printlevel(LINFO, "Set currently playing to %s\n", hitje.toString().c_str());
        currentlyPlayingText->setText(WString(hitje.toString()));
    } else {
        gui.printlevel(LINFO, "Emptied currently playing\n");
        currentlyPlayingText->setText(WString());
    }
    gui.printlevel(LINFO, "Updated playlist text\n");
}

