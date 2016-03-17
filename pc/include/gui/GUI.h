#ifndef GUI_H
#define GUI_H

enum InputVal {
    INPUT_EARTH_UP = 11,
    INPUT_EARTH_DOWN,
    INPUT_EARTH_SWAP,
    INPUT_HORN_UP,
    INPUT_HORN_DOWN,
    INPUT_HORN_SWAP,
    INPUT_END,
    INPUT_UPDATE,
    INPUT_TEST,
    INPUT_NONE
};

#include <vector>
#include <string>
#include <memory>

#include "structs.h"
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

    void setHitjes(const vector<Hitje *> &hitjes);
    const vector<Hitje *> &getHitjes();

    virtual void printlevel(PRINT_LEVEL level, const char *format, ...) = 0;
    virtual void confirm(PRINT_LEVEL level, const char *format, ...) = 0;
    virtual int getInput() = 0;
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

    virtual void setDownloadState(std::shared_ptr<DownloadState> dlstate) = 0;
protected:
    PRINT_LEVEL msglevel;
    GUIEvent guiEvents;
    vector<Hitje *> hitjes;
private:
};

#endif // GUI_H
