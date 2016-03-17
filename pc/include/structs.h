#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <sstream>

#include "vlc/vlc.h"

using namespace std;

struct DownloadState {
    int id;
    float percentage, dlsize, dlspeed;
    int eta;
};


struct Hitje {
    libvlc_media_t *mediaData;
    int hitIndex;
    string title;
    string artist;

    Hitje(libvlc_media_t *mediaData, int hitIndex, string title, string artist);
    ~Hitje();
    string toString();
    inline ostream &operator<<(ostream &str);
};



#endif
