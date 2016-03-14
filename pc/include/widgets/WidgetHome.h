#ifndef GUIWTAPPLICATION_H
#define GUIWTAPPLICATION_H

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WText>
#include <Wt/WTextArea>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WLabel>
#include <Wt/WIntValidator>
#include <Wt/WLengthValidator>
#include <Wt/WSound>
#include <Wt/WSuggestionPopup>
#include <Wt/WContainerWidget>
#include <Wt/WTemplate>
#include <Wt/WEnvironment>
#include <Wt/WSlider>

#include <string>
#include <iostream>

#include "Tools.h"
#include "gui/GUI.h"
#include "widgets/WFilledTemplate.h"
#include "widgets/WPlaylist.h"
#include "widgets/WPlaybackControls.h"
#include "widgets/WHitSearch.h"
#include "Persistence.h"
#include "InputProcessor.h"
#include "AudioList.h"

using namespace std;
using namespace boost;
using namespace Wt;

class WidgetHome : public WApplication {
public:
    WidgetHome(const WEnvironment& env, GUI *gui, Persistence *persistence);
    virtual ~WidgetHome();

    void onPersistenceChange(const string &key);

protected:
    GUI *gui;
    Persistence *persistence;
    WTemplate *home;

    WContainerWidget *logContainer;
    WSound *logUpdate;

    void updateLog();
private:
};

#endif // GUIWTAPPLICATION_H
