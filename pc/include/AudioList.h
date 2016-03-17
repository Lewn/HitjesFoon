#ifndef AUDIOLIST_H
#define AUDIOLIST_H

#define DEFAULT_DOWNLOAD 5

#include "vlc/vlc.h"
#include <cerrno>
#include <dirent.h>

#include <sstream>
#include <iomanip>

#include "structs.h"
#include "Tools.h"
#include "Config.h"
#include "VLC.h"
#include "YoutubeAPI.h"
#include "gui/GUI.h"

using namespace std;

class AudioList {
public:
    AudioList(GUI &gui, Config &config);
    virtual ~AudioList();

    string createHitjeName(const Hitje *hitje, bool absolute);
    string createHitjeName(int hitIndex, string title, string artist, bool absolute);
    bool update(unsigned int downloadCount);
    libvlc_media_t *getAudio(int audioIndex);

    const vector<Hitje *> getHitjes();

protected:
    GUI &gui;
    unsigned int downloadCount;
    YoutubeAPI *api;
    vector<Hitje *> hitjes;
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
