#ifndef GUI_H
#define GUI_H

#include <vector>
#include <string>
#include <memory>

#include "Structs.h"
#include "Hitje.h"
#include "gui/GUIEvent.h"

using namespace std;

enum PRINT_LEVEL {
    LERROR,
    LWARNING,
    LINFO,
    LBGINFO,
    LDEBUG,
    LDEBUGSLOW
};

class GUI {
public:

    GUI(PRINT_LEVEL msglevel);
    void setMsglevel(PRINT_LEVEL msglevel);
    PRINT_LEVEL getMsglevel();

    GUIEvent &events();

    virtual void printlevel(PRINT_LEVEL level, const char *format, ...) = 0;
    virtual void confirm(PRINT_LEVEL level, const char *format, ...) = 0;

    virtual void addInput(int input) = 0;
    virtual int getInput() = 0;
    virtual int getKeystroke() = 0;
    virtual int selection(vector<string> options) = 0;

    virtual void inputPhoneNum(int num);
    virtual void inputPhoneAlt(int alt);

    virtual void setPlaying() = 0;
    virtual void setPaused() = 0;
    virtual void setStopped() = 0;
    virtual void setPlaybackState(PlaybackState state);

    virtual void setPhoneNum(int num) = 0;
    virtual void setPlaylist(const vector<int> &playlist) = 0;

    virtual void setSpeakerVolume(int volume) = 0;
    virtual void setPhoneVolume(int volume) = 0;

    virtual void setHitje(Hitje hitje) = 0;
protected:
    PRINT_LEVEL msglevel;
    GUIEvent guiEvents;
private:
};

#endif // GUI_H
