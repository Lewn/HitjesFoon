#ifndef AUDIOLIST_H
#define AUDIOLIST_H

#define DEFAULT_DOWNLOAD    5
#define BUF_SIZE            2048

#include "vlc/vlc.h"
#include <cerrno>
#include <dirent.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>

#include "Structs.h"
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

    bool update(unsigned int downloadCount);
    const Hitje &getHitje(int hitjeIndex);

protected:
    GUI &gui;
    unsigned int downloadCount;
    YoutubeAPI api;
    vector<Hitje> hitjes;
    string listFilePath;
    string hitjesPath;

    int skipInvalidLines(ifstream &listFileStream, string &fileOutput);
    int readLine(ifstream &listFileStream, string &fileOutput);
    bool downloadVideoFile(Hitje &hitje);
    bool createMediaFile(Hitje &hitje);

private:
    AudioList(const AudioList & that) = delete;
};

#endif // AUDIOLIST_H
