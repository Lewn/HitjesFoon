#ifndef WPLAYBACKCONTROLS_H
#define WPLAYBACKCONTROLS_H

#include <Wt/WApplication>
#include <Wt/WPushButton>
#include <Wt/WSlider>


#include "gui/GUI.h"
#include "gui/GUIEvent.h"
#include "widgets/WFilledTemplate.h"
#include "persistence/Persistence.h"

using namespace std;
using namespace boost;
using namespace Wt;


class WPlaybackControls : public WFilledTemplate {
public:
    WPlaybackControls(GUI &gui, Persistence &persistence, WContainerWidget *parent = NULL);
    WPlaybackControls(GUI &gui, Persistence &persistence, const WString &text, WContainerWidget *parent = NULL);
    virtual ~WPlaybackControls();

    void buildWidget();
    void updatePlayPause();

    void onPersistenceChange(const string &key);

    void processVolumeChange(int volume);
    void processStop();
    void processPlay();
    void processPause();
protected:
    GUI &gui;
    Persistence &persistence;

    WSlider *volumeSlider;
    WPushButton *stopBtn;
    WPushButton *playBtn;
    WPushButton *pauseBtn;

private:
};

#endif // WPLAYBACKCONTROLS_H
