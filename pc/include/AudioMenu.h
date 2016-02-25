#ifndef AUDIOMENU_H
#define AUDIOMENU_H

#define __NO_MINGW_LFS

#include <dirent.h>
#include <sys/stat.h>

#include "Tools.h"
#include "GUI.h"
#include "vlc/vlc.h"
#include "AudioMenuItem.h"

#include <string>

using namespace std;

class AudioMenu {
public:
    AudioMenu(GUI *gui);
    virtual ~AudioMenu();
    void fromPath(string path);

    virtual bool process(int);
    virtual libvlc_media_t *getMedia();
    virtual bool isEnded();
protected:
    GUI *gui;
    AudioMenuItem *initialItem;
    AudioMenuItem *curItem;

    AudioMenuItem *createItem(string path);
private:
    AudioMenu(const AudioMenu &that) = delete;
};

#endif // AUDIOMENU_H
