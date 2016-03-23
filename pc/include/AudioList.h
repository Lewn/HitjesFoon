#ifndef AUDIOLIST_H
#define AUDIOLIST_H

#define DEFAULT_DOWNLOAD    5
#define BUF_SIZE            2048

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
#include "Hitje.h"
#include "Config.h"
#include "retrieve/Retriever.h"
#include "gui/GUI.h"

using namespace std;

class AudioList {
public:
    AudioList(GUI &gui, Config &config);
    virtual ~AudioList();

    bool update(unsigned int downloadCount);
    void hitjeUpdate(const Hitje &hitje);
    const Hitje &getHitje(int hitjeIndex);

protected:
    GUI &gui;
    unsigned int downloadCount;
    Retriever retriever;
    vector<Hitje> hitjes;
    string hitjesPath;
    string listFilePath;

    int skipInvalidLines(ifstream &listFileStream, string &fileOutput);
    int readLine(ifstream &listFileStream, string &fileOutput);
    bool downloadVideoFile(Hitje &hitje);
private:
    AudioList(const AudioList & that) = delete;
};

#endif // AUDIOLIST_H
