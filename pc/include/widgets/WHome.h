#ifndef GUIWTAPPLICATION_H
#define GUIWTAPPLICATION_H

#include <Wt/WApplication>
#include <Wt/WSound>
#include <Wt/WContainerWidget>

#include <string>
#include <iostream>

#include "Tools.h"
#include "gui/GUI.h"
#include "widgets/WFilledTemplate.h"
#include "widgets/WPlaylist.h"
#include "widgets/WPlaybackControls.h"
#include "widgets/WHitSearch.h"
#include "persistence/Persistence.h"
#include "InputProcessor.h"
#include "AudioList.h"

using namespace std;
using namespace boost;
using namespace Wt;

class WHome : public WFilledTemplate {
public:
    WHome(GUI &gui, Persistence &persistence, WContainerWidget *parent = NULL);
    WHome(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent = NULL);
    virtual ~WHome();

    void buildWidget();
    void onPersistenceChange(const string &key);

protected:
    GUI &gui;
    Persistence &persistence;

    WHitSearch *hitSearch;
    WPlaylist *playlist;
    WPlaybackControls *playbackControls;
    WContainerWidget *logContainer;
    WSound *logUpdate;

    // Local version of the log count (# of already logged items)
    int logl;

    void updateLog();
private:
};

#endif // GUIWTAPPLICATION_H
