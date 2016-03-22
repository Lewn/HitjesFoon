#include "gui/GUIWt.h"


GUIWt::GUIWt(PRINT_LEVEL msglevel) : GUINull(msglevel), persistence(NULL) {
}

GUIWt::~GUIWt() {
}

void GUIWt::setServer(WServer &server) {
    persistence = new Persistence(server);
    persistence->getStringData().initVal("log", string(""));
    vector<string> logv;
    logv.push_back("");
    persistence->getStringVectorData().initVal("logv", logv);
    persistence->getIntData().initVal("playback-state", STOP);
}

WApplication *GUIWt::createApplication(const WEnvironment& env) {
    if (persistence == NULL) {
        throw "Persistence not created yet, first set server object";
    }
    AHitjesfoon *app = new AHitjesfoon(env, *this, *persistence);

//    app->setCssTheme("polished");
    WBootstrapTheme *bootstrapTheme = new WBootstrapTheme(app);
    bootstrapTheme->setVersion(WBootstrapTheme::Version3);
    bootstrapTheme->setResponsive(true);
    app->setTheme(bootstrapTheme);

    // load the default bootstrap3 (sub-)theme
    app->useStyleSheet("resources/themes/bootstrap/3/bootstrap-theme.min.css");

    app->messageResourceBundle().use(app->appRoot() + "language/home");
    app->messageResourceBundle().use(app->appRoot() + "language/hitjeslist");
    app->messageResourceBundle().use(app->appRoot() + "language/404");
    app->messageResourceBundle().use(app->appRoot() + "templates/home");
    app->messageResourceBundle().use(app->appRoot() + "templates/hitjeslist");
    app->messageResourceBundle().use(app->appRoot() + "templates/404");

    return app;
}


void GUIWt::printlevel(PRINT_LEVEL level, const char *format, va_list args) {
    GUINull::printlevel(level, format, args);
    if (persistence != NULL) {
        char buf[1000];
        vsnprintf(buf, sizeof(buf), format, args);

        logAppend(level, buf);
    }
}


void GUIWt::setPlaying() {
    persistence->getIntData().setVal("playback-state", PLAY);
}

void GUIWt::setPaused() {
    persistence->getIntData().setVal("playback-state", PAUSE);
}

void GUIWt::setStopped() {
    persistence->getIntData().setVal("playback-state", STOP);
}

void GUIWt::setPhoneNum(int num) {
    persistence->getIntData().setVal("phone-num", num);
}

void GUIWt::setPlaylist(const vector<int> &playlist) {
    // TODO more efficient way of updating phone-playlist, as it now is triggered too many times
    // once when a hitje is stopped, then when the playlist is moved forward etc. etc.
    persistence->getIntVectorData().setVal("phone-playlist", playlist);
}

void GUIWt::setSpeakerVolume(int volume) {
    events().speakerVolumeSig(volume);
    persistence->getIntData().setVal("volume-speaker", volume);
}

void GUIWt::setPhoneVolume(int volume) {
    events().phoneVolumeSig(volume);
    persistence->getIntData().setVal("volume-phone", volume);
}

void GUIWt::setHitje(Hitje hitje) {
    persistence->getHitjeData().setVal(to_string(hitje.hitIndex), hitje);
}

void GUIWt::logAppend(PRINT_LEVEL level, string text) {
    string style = "";
    string el;
    bool nlfirst = (text.size() > 1) && (*(text.begin()) == '\n');
    bool nlsecond = (*(text.cend() - 1) == '\n');

    // append log text
    persistence->getStringData().addVal("log", text);
    // Don't print stringent log data to web interface
    // (both slow and crashes!)
    if (level > LINFO) {
        return;
    }

    // end with newline, erase
    if (nlfirst) {
        text.erase(text.begin());
    }
    if (nlsecond) {
        text.erase(text.end() - 1);
    }
    if (!text.empty()) {
        if (level == LERROR) {
            style = "style=\"color:red\"";
        } else if (level == LWARNING) {
            style = "style=\"color:orange\"";
        } else {
//            style = "style=\"color:green\"";
        }

        text = Utils::htmlEncode(text, Utils::HtmlEncodingFlag::EncodeNewLines);
        el = "<span " + style + ">" + text + "</span>\n";
        persistence->getStringVectorData().manipulateVal("logv", boost::bind(&GUIWt::logHTML, this, _1, el, nlfirst, nlsecond));
    }
}

void GUIWt::logHTML(vector<string> &logv, const string &el, bool nlfirst, bool nlsecond) {
    string &last = *(logv.end() - 1);
    if (nlfirst && !last.empty()) {
        logv.push_back(el);
    } else {
        last += el;
    }
    if (nlsecond) {
        logv.push_back("");
    }
}

int GUIWt::getInput() {
    WApplication *app = WApplication::instance();
    int c = 0;
    if (app == NULL) {
        // No user input, get from terminal
        // Still require a reliable cross platform solution
        if (kbhit()) {
            c = getch();
        }
    } else {
        // Get input from the users browser and process
        printlevel(LWARNING, "function getInput not implemented yet\n");
    }
    if (c >= '0' && c <= '9') {
        // got a number, return it
        return c - '0';
    } else if (c == 8 || c == 'd') {
        // backspace, simulate earth button (reset number)
        return INPUT_EARTH_DOWN;
    } else if (c == 's') {
        // s, simulate horn (swap output)
        return INPUT_HORN_SWAP;
    } else if (c == 'q') {
        // q, return quit
        return INPUT_END;
    } else if (c == 'u') {
        return INPUT_UPDATE;
    } else if (c == 't') {
        // t, testing purposes
        return INPUT_TEST;
    }
    return INPUT_NONE;
}

