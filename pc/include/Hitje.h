#ifndef HITJE_H
#define HITJE_H

#include <string>
#include <sstream>
#include <iomanip>

#include "Structs.h"
#include "Tools.h"
#include "vlc/vlc.h"

using namespace std;

class Hitje {
public:
    int hitIndex;
    string path, artist, title;
    DownloadState downloadState;

    Hitje(int hitIndex, string path);
    Hitje(libvlc_media_t *mediaData, int hitIndex, string path, string artist, string title);
    Hitje(const Hitje &other);
    ~Hitje();

    string toString() const;
    inline ostream &operator<<(ostream &str) const;
    operator bool() const;
    bool operator !() const;
    bool operator ==(const Hitje& other) const;
    bool operator !=(const Hitje& other) const;
    Hitje &operator=(const Hitje &other);

    string createFilename(bool absolute = false, bool extension = true) const;

    // Increases/decreases refcount, take care for memory management!
    libvlc_media_t *getMediaData() const;
    void setMediaData(libvlc_media_t *newMediaData);
protected:
    void init(libvlc_media_t *mediaData, string artist, string title);

private:
    libvlc_media_t *mediaData;
};

#endif // HITJE_H
