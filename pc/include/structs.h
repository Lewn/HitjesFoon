#ifndef STRUCTS_H
#define STRUCTS_H

// TODO rename to data types or something?
// Hitje became too extensive
// Maybe simply move Hitje to its own class

#include <string>
#include <sstream>
#include <iomanip>

#include "Tools.h"
#include "vlc/vlc.h"

using namespace std;

struct DownloadState {
    bool downloading = false;
    float percentage = 0, dlsize = 0, dlspeed = 0;
    int eta = 0;
};


class Hitje {
public:
    int hitIndex;
    string artist, title;
    DownloadState downloadState;

    Hitje(int hitIndex);
    Hitje(libvlc_media_t *mediaData, int hitIndex, string artist, string title);
    Hitje(const Hitje &other);
    ~Hitje();

    string toString() const;
    inline ostream &operator<<(ostream &str) const;
    operator bool() const;
    operator !() const;
    operator ==(const Hitje& other) const;
    operator !=(const Hitje& other) const;

    string createFilename() const;

    // Increases/decreases refcount, take care for memory management!
    libvlc_media_t *getMediaData() const;
    void setMediaData(libvlc_media_t *newMediaData);

private:
    libvlc_media_t *mediaData;
};



#endif
