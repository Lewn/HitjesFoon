#ifndef AUDIOMENUITEM_H
#define AUDIOMENUITEM_H

#include "vlc/vlc.h"
#include "VLC.h"
#include "Tools.h"

#ifdef _WIN32
#include <windows.h>
#else

#endif

class AudioMenuItem {
public:
    AudioMenuItem(string path, AudioMenuItem **followup, unsigned char followupLen, bool tts);
    ~AudioMenuItem();

    bool hasNext();
    AudioMenuItem *getFollowup(unsigned char index);
    libvlc_media_t *getMedia();
protected:
    char *text;
    libvlc_media_t *media;
    AudioMenuItem **followup;
    unsigned char followupLen;
private:
};

#endif // AUDIOMENUITEM_H
