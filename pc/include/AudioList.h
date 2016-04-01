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

    void update();
    void hitjeUpdate(const Hitje &hitje);
    const Hitje &getHitje(int hitjeIndex);

protected:
    GUI &gui;
    Retriever retriever;
    vector<Hitje> hitjes;
    string hitjesPath;
    string listFilePath;

    void writeUpdate();
    int skipInvalidLines(ifstream &listFileStream);
    int readLine(ifstream &listFileStream, ofstream *fileOutput = NULL);
    int parseHitIndex(string &line);
    int parseLine(string &line);
private:
    AudioList(const AudioList & that) = delete;
};

#endif // AUDIOLIST_H
