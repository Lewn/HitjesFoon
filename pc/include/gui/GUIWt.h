#ifndef GUIWT_H
#define GUIWT_H

#include "Structs.h"
#include "Tools.h"

#include <vector>
#include <iostream>
#include <memory>

#include <Wt/WApplication>
#include <Wt/Utils>
#include <Wt/WDialog>
#include <Wt/WLineEdit>
#include <Wt/WText>
#include <Wt/WRegExpValidator>
#include <Wt/WBootstrapTheme>

#include "Hitje.h"
#include "gui/GUI.h"
#include "gui/GUINull.h"
#include "gui/GUIEvent.h"

#include "widgets/AHitjesfoon.h"

#include "InputProcessor.h"
#include "persistence/Persistence.h"


using namespace std;
using namespace Wt;

class GUIWt : public GUINull {
public:
    using GUINull::printlevel;

    GUIWt(PRINT_LEVEL msglevel);
    virtual ~GUIWt();

    void setServer(WServer &server);
    void onPersistenceChange(const string &key);
    WApplication *createApplication(const WEnvironment& env);

    virtual void setPlaying();
    virtual void setPaused();
    virtual void setStopped();

    virtual void setPhoneNum(int num);
    virtual void setPlaylist(const vector<int> &playlist);

    virtual void setSpeakerVolume(int volume);
    virtual void setPhoneVolume(int volume);

    virtual void setHitje(Hitje hitje);

    void logAppend(PRINT_LEVEL level, string text);
    void logHTML(vector<string> &logv, const string &el, bool nlfirst, bool nlsecond);

    virtual int getInput();
protected:
    Persistence *persistence;

    virtual void printlevel(PRINT_LEVEL level, const char *format, va_list args);
private:
    int input = EOF;
    void inputLoop();
};

#endif // GUIWT_H
