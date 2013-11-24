#ifndef AUDIOMENU_H
#define AUDIOMENU_H

#define __NO_MINGW_LFS

#include <dirent.h>

#include "vlc/vlc.h"
#include "AudioMenuItem.h"
#include "Tools.h"

class AudioMenu {
public:
    AudioMenu();
    virtual ~AudioMenu();
    void fromPath(const char* path);

    virtual bool process(int);
    virtual libvlc_media_t* getMedia();
    virtual bool isEnded();
protected:
    AudioMenuItem* initialItem;
    AudioMenuItem* curItem;

    AudioMenuItem* createItem(const char* path);
private:
};

#endif // AUDIOMENU_H
