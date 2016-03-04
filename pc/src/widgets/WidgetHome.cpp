#include "widgets/WidgetHome.h"

WidgetHome::WidgetHome(const WEnvironment& env, GUI *gui, Persistence *persistence) : WApplication(env), gui(gui), persistence(persistence), logUpdate(NULL), searchSP(NULL) {
    setTitle("HitjesFoon");
    enableUpdates(true);

    logContainer = new WContainerWidget();
    updateLog();

    home = new WFilledTemplate(WString::tr("template-hitjesfoon-home"), root());
    home->bindWidget("log", logContainer);

    hitInputText = new WLineEdit();
    WIntValidator *intValidator = new Wt::WIntValidator(0, 999);
    hitInputText->setValidator(intValidator);

    WPushButton *hitOkBtn = new WPushButton(WString::tr("strings-hitinput-ok"));
    hitHelpText = new WText();

    home->bindWidget("hitinput", hitInputText);
    home->bindWidget("hitok", hitOkBtn);
    home->bindWidget("hithelp", hitHelpText);


    searchInputText = new WLineEdit();
    WLengthValidator *lenValidator = new WLengthValidator(1, 1000);
    searchInputText->setValidator(lenValidator);

    WPushButton *searchOkBtn = new WPushButton(WString::tr("strings-searchinput-ok"));
    searchHelpText = new WText();

    initSearchSuggestions();
    searchSP->forEdit(searchInputText);

    home->bindWidget("searchinput", searchInputText);
    home->bindWidget("searchok", searchOkBtn);
    home->bindWidget("searchhelp", searchHelpText);

    currentlyTypingText = new WText();
    currentlyPlayingText = new WText();
    playlistText = new WText();

    home->bindWidget("currently-typing", currentlyTypingText);
    home->bindWidget("currently-playing", currentlyPlayingText);
    home->bindWidget("playlist", playlistText);

    volumeSlider = new WSlider();
    volumeSlider->setTickInterval(1);
    volumeSlider->setMinimum(0);
    volumeSlider->setMaximum(100);
    volumeSlider->setValue(persistence->getIntData().getVal("volume-speaker"));

    stopBtn = new WPushButton(WString::tr("strings-playback-stop"));
    playpauseBtn = new WPushButton(WString::tr("strings-playback-pause"));

    home->bindWidget("playback-volume", volumeSlider);
    home->bindWidget("playback-stop", stopBtn);
    home->bindWidget("playback-playpause", playpauseBtn);

    logUpdate = new WSound("resources/sounds/logUpdate.mp3");


    hitInputText->keyPressed().connect(std::bind([ = ] (const WKeyEvent & e) {
        if (e.key() == Key_Enter) {
            processHitInput();
        }
    }, std::placeholders::_1));
    hitOkBtn->clicked().connect(this, &WidgetHome::processHitInput);
    searchInputText->keyPressed().connect(std::bind([ = ] (const WKeyEvent & e) {
        if (e.key() == Key_Enter) {
            processSearchInput();
        }
    }, std::placeholders::_1));
    searchOkBtn->clicked().connect(this, &WidgetHome::processSearchInput);

    // Process on value change
    volumeSlider->valueChanged().connect(boost::bind(WidgetHome::processVolumeChange, this, _1));
    // Also directly process on slide event
//    volumeSlider->sliderMoved().connect(boost::bind(WidgetHome::processVolumeChange, this, _1));

    stopBtn->clicked().connect(this, &WidgetHome::processStop);
    stopBtn->clicked().connect(this, &WidgetHome::processPlayPause);

    // Listen to all persistence changes
    persistence->onChangeCallback(boost::bind(&WidgetHome::onPersistenceChange, this, _1));
}

WidgetHome::~WidgetHome() {
    SAFE_DELETE(logUpdate);
}

void WidgetHome::processHitInput() {
    string num = hitInputText->text().toUTF8();
    if (hitInputText->validate() == WValidator::Valid) {
        InputProcessor *inputProcessor = InputProcessor::getInstance();
        inputProcessor->process(num);
        hitInputText->setText("");
    } else {
        string helpText = "Invalid input, '" + num + "'. Required format is 'ddd'.";
        hitHelpText->setText(helpText);
        gui->printlevel(LERROR, "Invalid input, '%s'\nShould be of the format 'ddd'", num.c_str());
    }
}

void WidgetHome::processSearchInput() {
    string ser = searchInputText->text().toUTF8();
    if (searchInputText->validate() == WValidator::Valid) {
        try {
            int num = stoi(ser);
            InputProcessor *inputProcessor = InputProcessor::getInstance();
            inputProcessor->inputNum(num);
            searchInputText->setText("");
        } catch (...) {
            string helpText = "Input '" + ser + "' is not a valid input (must start with number)";
            searchHelpText->setText(helpText);
            gui->printlevel(LERROR, "%s\n", helpText.c_str());
        }
    } else {
        string helpText = "Invalid input, '" + ser + "'. Cannot request too large.";
        searchHelpText->setText(helpText);
        gui->printlevel(LERROR, "Invalid input, '%s'\nCannot request too large\n", ser.c_str());
    }
}

void WidgetHome::processVolumeChange(int volume) {
    gui->setSpeakerVolume(volume);
}

void WidgetHome::processStop() {
}

void WidgetHome::processPlayPause() {
}


void WidgetHome::onPersistenceChange(string key) {
    printf("Persistence change key %s\n", key.c_str());
    if (key == "log") {
//        logText->setText(persistence->getStringData().getVal(key));
//        logUpdate->play();
    } else if (key == "logv") {
        updateLog();
    } else if (key == "phone-num") {
        currentlyTypingText->setText(to_string(persistence->getIntData().getVal(key)));
    } else if (key == "phone-playlist") {
        updatePlaylist();
    } else if (key == "volume-speaker") {
        volumeSlider->setValue(persistence->getIntData().getVal("volume-speaker"));
    } else if (key == "volume-phone") {

    }
    triggerUpdate();
}


void WidgetHome::initSearchSuggestions() {
    if (searchSP == NULL) {
        WSuggestionPopup::Options searchOptions;
        searchOptions.highlightBeginTag = "<span class=\"highlight\">";
        searchOptions.highlightEndTag = "</span>";
        searchOptions.whitespace = " \\n";
        searchOptions.wordSeparators = "-., \"@\\n;";

        searchSP = new WSuggestionPopup(
            WSuggestionPopup::generateMatcherJS(searchOptions),
            WSuggestionPopup::generateReplacerJS(searchOptions),
            searchInputText);

        searchSP->setMaximumSize(WLength::Auto, WLength(200, WLength::Unit::Pixel));

        InputProcessor *processor = InputProcessor::getInstance();
        AudioList *audioList = processor->getHitjesList();
        const vector<Hitje *> hitjes = audioList->getHitjes();
        for (Hitje *hitje : hitjes) {
            if (hitje != NULL) {
                searchSP->addSuggestion(hitje->toString());
            }
        }
    }
}

void WidgetHome::updatePlaylist() {
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


