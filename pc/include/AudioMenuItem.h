#ifndef AUDIOMENUITEM_H
#define AUDIOMENUITEM_H

#include "Tools.h"
#include "GUI.h"
#include "vlc/vlc.h"
#include "VLC.h"

#ifdef _WIN32
#include <windows.h>
#else

#endif

class AudioMenuItem {
public:
    AudioMenuItem(GUI *gui, string path, AudioMenuItem **followup, unsigned char followupLen, bool tts);
    ~AudioMenuItem();

    bool hasNext();
    AudioMenuItem *getFollowup(unsigned char index);
    libvlc_media_t *getMedia();
protected:
    GUI *gui;
    string text;
    libvlc_media_t *media;
    AudioMenuItem **followup;
    unsigned char followupLen;
private:
    AudioMenuItem(const AudioMenuItem &that) = delete;
};

#endif // AUDIOMENUITEM_H
