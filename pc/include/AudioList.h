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

#include <sstream>
#include <iomanip>

using namespace std;

struct Hitje {
    libvlc_media_t *mediaData;
    int hitIndex;
    string title;
    string artist;

    Hitje(libvlc_media_t *mediaData, int hitIndex, string title, string artist);
    ~Hitje();
};

class AudioList {
public:
    AudioList(Config *config);
    virtual ~AudioList();

    string createHitjeName(const Hitje *hitje, bool absolute);
    string createHitjeName(int hitIndex, string title, string artist, bool absolute);
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
    string getVideoFile(int hitIndex, const char *title, const char *artist);

private:
    AudioList(const AudioList &that) = delete;
};

#endif // AUDIOLIST_H
