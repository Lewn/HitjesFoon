#ifndef AUDIOLIST_H
#define AUDIOLIST_H

#define DEFAULT_DOWNLOAD 5

class AudioList;

#include "vlc/vlc.h"
#include <cerrno>

#include "VLC.h"
#include "Tools.h"
#include "InputProcessor.h"
#include "YoutubeAPI.h"

using namespace std;

class AudioList {
public:
    AudioList(const char* listFilePath);
    virtual ~AudioList();

    libvlc_media_t* getAudio(int audioIndex);

protected:
    YoutubeAPI api;
    libvlc_media_t** hitjesList;
    bool downloadYoutube;
    int pathLen;
    char* listFilePath;

    int checkMediaFile(libvlc_media_t* mediaFile);

    bool parseBuf(char* buffer, int *hitIndex, char *title, char *artist, char *path, string &fileOutput);
    char* getVideoFile(int hitIndex, char *title, char *artist);

private:
};

#endif // AUDIOLIST_H
