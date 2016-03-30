#include "widgets/WHitje.h"

WHitje::WHitje(GUI &gui, Persistence &persistence, int hitIndex, WContainerWidget *parent) : WHitje(gui, persistence, WString::tr("template-hitjesfoon-hitjeslist-hitje"), hitIndex, parent) {}

WHitje::WHitje(GUI &gui, Persistence &persistence, const WString &text, int hitIndex, WContainerWidget *parent) : WFilledTemplate(text, parent), gui(gui), persistence(persistence), hitIndex(hitIndex) {
}

WHitje::~WHitje() {
    SAFE_DELETE(editDialog);
}

void WHitje::buildWidget() {
    stringstream hitIndexStr;
    hitIndexStr << setfill('0') << setw(3) << hitIndex;
    bindString("hitje-num", hitIndexStr.str());

    artistText = new WText();
    artistText->clicked().connect(std::bind(&WHitje::showDialog, this, true));
    titleText = new WText();
    titleText->clicked().connect(std::bind(&WHitje::showDialog, this, false));
    infoText = new WText();
    downloadBtn = new WPushButton("Download");
    downloadBtn->clicked().connect(std::bind(&WHitje::download, this));
    pgBar = new WProgressBar();
    pgBar->setRange(0, 100);

    updateHitje();

    bindWidget("hitje-artist", artistText);
    bindWidget("hitje-title", titleText);
    bindWidget("hitje-info", infoText);
    bindWidget("hitje-download", downloadBtn);
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

void WHitje::showDialog(bool artist) {
    if (editDialog == NULL) {
        editDialog = new WDialog();
        WLineEdit *edit = new WLineEdit(editDialog->contents());

        WPushButton *ok = new WPushButton("OK", editDialog->footer());
        ok->setDefault(true);

        WPushButton *cancel = new WPushButton("Cancel", editDialog->footer());
        editDialog->rejectWhenEscapePressed();

        ok->clicked().connect(editDialog, &WDialog::accept);
        cancel->clicked().connect(editDialog, &WDialog::reject);

        editDialog->finished().connect(std::bind([ = ] () {
            if (editDialog->result() == WDialog::Accepted) {
                persistence.getHitjeData().manipulateVal(to_string(hitIndex), [&] (Hitje & hitje) {
                    if (editingArtist) {
                        hitje.artist = edit->text().narrow();
                    } else {
                        hitje.title = edit->text().narrow();
                    }
                    hitje.setMediaData(NULL);
                });
            }
        }));
    }
    const Hitje &hitje = persistence.getHitjeData().getVal(to_string(hitIndex));
    WLineEdit *edit = (WLineEdit *)editDialog->contents()->widget(0);
    if (artist) {
        editDialog->setWindowTitle("Editing artist");
        edit->setText(hitje.artist);
    } else {
        editDialog->setWindowTitle("Editing title");
        edit->setText(hitje.title);
    }
    // Directly request focus to this field
    edit->setFocus();
    editingArtist = artist;
    editDialog->show();
}

bool WHitje::download() {
    // Check whether download is even necessary
    const Hitje &hitjeCheck = persistence.getHitjeData().getVal(to_string(hitIndex));
    if (hitjeCheck || hitjeCheck.downloadState.downloading || hitjeCheck.artist.empty() || hitjeCheck.title.empty()) {
        // Don't need download for whatever reason
        return false;
    }

    // Perform threaded, as downloading may take a very long time
    // As not to block the UI
    persistence.getHitjeData().manipulateVal(to_string(hitIndex), [&](Hitje & hitje) {
        thread([&]() {
            // Create a new retriever
            Retriever retriever(gui);
            // And download this hitje
            // TODO dirty to keep this reference?
            retriever.retrieve(hitje);
        }).detach();
    });
    return true;
}

void WHitje::updateHitje() {
    bool downloadable = false;
    const Hitje &hitje = persistence.getHitjeData().getVal(to_string(hitIndex));
    const DownloadState &dlstate = hitje.downloadState;

    if (!hitje.artist.empty()) {
        artistText->setText(WString(hitje.artist));
    } else {
        artistText->setText("unknown");
    }
    if (!hitje.title.empty()) {
        titleText->setText(WString(hitje.title));
    } else {
        titleText->setText("no title");
    }

    if (dlstate.downloading) {
        // Update percentage while downloading
        pgBar->setValue(dlstate.percentage);
    } else if (hitje) {
        // Nothing wrong yet
        infoText->setText(WString(""));
    } else {
        // hitje unavailable and not downloading, notify user
        if (hitje.artist.empty() && hitje.title.empty()) {
            // neither artist or title filled out
            infoText->setText(WString("Artist and title still required"));
        } else if (hitje.artist.empty()) {
            // no artist specified
            infoText->setText(WString("Artist still required"));
        } else if (hitje.title.empty()) {
            // no title specified
            infoText->setText(WString("Title still required"));
        } else {
            // else we can download
            downloadable = true;
        }
    }
    pgBar->setHidden(!dlstate.downloading);
    downloadBtn->setHidden(!downloadable);
    infoText->setHidden(dlstate.downloading || downloadable);
}
