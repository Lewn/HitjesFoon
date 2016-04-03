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

    virtual void setPlaying() override;
    virtual void setPaused() override;
    virtual void setStopped() override;

    virtual void setPhoneNum(int num) override;
    virtual void setPlaylist(const vector<int> &playlist) override;

    virtual void setSpeakerVolume(int volume) override;
    virtual void setPhoneVolume(int volume) override;

    virtual void setHitje(Hitje hitje) override;

    void logAppend(PRINT_LEVEL level, string text);
    void logHTML(vector<string> &logv, const string &el, bool nlfirst, bool nlsecond);

    virtual int getInput() override;
protected:
    Persistence *persistence;

    virtual void printlevel(PRINT_LEVEL level, const char *format, va_list args) override;
private:
    int input = EOF;
    void inputLoop();
};

#endif // GUIWT_H
