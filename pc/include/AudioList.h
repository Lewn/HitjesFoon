#ifndef AUDIOLIST_H
#define AUDIOLIST_H

#define DEFAULT_DOWNLOAD 5

class AudioList;

#include "vlc/vlc.h"
#include <cerrno>
#include <dirent.h>

#include "Config.h"
#include "VLC.h"
#include "Tools.h"
#include "InputProcessor.h"
#include "YoutubeAPI.h"

using namespace std;

struct Hitje {
    libvlc_media_t *mediaData;
    int hitIndex;
    char *title;
    char *artist;

    Hitje(libvlc_media_t *mediaData, int hitIndex, char *title, char *artist);
    ~Hitje();
};

class AudioList {
public:
    AudioList(Config *config);
    virtual ~AudioList();

    const char *createHitjeName(const Hitje *hitje, bool absolute);
    const char *createHitjeName(int hitIndex, char *title, char *artist, bool absolute);
    bool update(unsigned int downloadCount);
    libvlc_media_t *getAudio(int audioIndex);

protected:
    unsigned int downloadCount;
    YoutubeAPI api;
    Hitje **hitjesList;
    string listFilePath;
    string hitjesPath;
    FILE *listFile;

    int checkMediaFile(libvlc_media_t *mediaFile);

    void skipInvalidLines(char *buffer, int *hitIndex, char *title, char *artist, string &fileOutput);
    bool parseBuf(char *buffer, int *hitIndex, char *title, char *artist, string &fileOutput);
    const char *getVideoFile(int hitIndex, char *title, char *artist);

private:
};

#endif // AUDIOLIST_H
