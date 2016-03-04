#ifndef GUIWT_H
#define GUIWT_H

#include "Tools.h"
#include <vector>
#include <iostream>
#include <conio.h>

#include <Wt/WApplication>
#include <Wt/Utils>
#include <Wt/WDialog>
#include <Wt/WLineEdit>
#include <Wt/WText>
#include <Wt/WRegExpValidator>
#include <Wt/WBootstrapTheme>

#include "gui/GUI.h"
#include "gui/GUINull.h"

#include "InputProcessor.h"
#include "Persistence.h"
#include "widgets/WidgetHome.h"


using namespace std;
using namespace Wt;

class GUIWt : public GUINull {
public:
    using GUINull::printlevel;

    GUIWt(PRINT_LEVEL msglevel);
    virtual ~GUIWt();

    void setServer(WServer &server);
    WApplication *createApplication(const WEnvironment& env);

    void setPhoneNum(int num);
    void setPlaylist(const vector<int> &playlist);

    virtual void setSpeakerVolume(int volume);
    virtual void setPhoneVolume(int volume);

    void logAppend(PRINT_LEVEL level, string text);
    void logHTML(vector<string> &logv, const string &el, bool nlfirst, bool nlsecond);

    virtual int getInput();
protected:
    Persistence *persistence;
    vector<WApplication *> applications;

    virtual void printlevel(PRINT_LEVEL level, const char *format, va_list args);
private:
};

#endif // GUIWT_H
