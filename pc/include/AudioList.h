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
    AudioList(const char *listFilePath, const char *hitjesPath);
    virtual ~AudioList();

    bool update(unsigned int downloadCount);
    libvlc_media_t* getAudio(int audioIndex);

protected:
    unsigned int downloadCount;
    YoutubeAPI api;
    libvlc_media_t **hitjesList;
    int hitjesPathLen;
    char *listFilePath;
    char *hitjesPath;
    FILE *listFile;

    int checkMediaFile(libvlc_media_t* mediaFile);

    void skipInvalidLines(char* buffer, int *hitIndex, char *title, char *artist, char *path, string &fileOutput);
    bool parseBuf(char* buffer, int *hitIndex, char *title, char *artist, char *path, string &fileOutput);
    char* getVideoFile(int hitIndex, char *title, char *artist);

private:
};

#endif // AUDIOLIST_H
