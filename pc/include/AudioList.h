#ifndef AUDIOLIST_H
#define AUDIOLIST_H

#include "vlc/vlc.h"
#include <string.h>
#include <ctype.h>

#include "VLC.h"

using namespace std;

class AudioList {
public:
    AudioList(const char* listFilePath);
    virtual ~AudioList();

    libvlc_media_t* getAudio(int audioIndex);

protected:
    libvlc_media_t** hitjesList;

    int checkMediaFile(libvlc_media_t* mediaFile);
    char* getAbsolutePath(const char* listFilePath, int pathLen, const char* filename);

private:
};

#endif // AUDIOLIST_H
