#include "Hitje.h"
#include "VLC.h"


Hitje::Hitje(int hitIndex, string path) : Hitje(NULL, hitIndex, path, "", "") {
}

Hitje::Hitje(libvlc_media_t *mediaData, int hitIndex, string path, string artist, string title) : hitIndex(hitIndex), path(path) {
    init(mediaData, artist, title);
}

Hitje::Hitje(const Hitje &other) : Hitje(other.mediaData, other.hitIndex, other.path, other.artist, other.title) {
    // also copy download state
    downloadState = other.downloadState;
}

Hitje::~Hitje() {
    // decrease mediaData ref counter (implicitly release)
    VLC::release(mediaData);
}

void Hitje::init(libvlc_media_t *mediaData, string artist, string title) {
    // increase mediaData ref counter
    VLC::retain(mediaData);
    this->mediaData = mediaData;
    this->artist = trim(artist);
    this->title = trim(title);
}

string Hitje::toString() const {
    // "ddd: artist - title"
    stringstream str;
    str << setfill('0') << setw(3) << hitIndex << ": ";
    str << artist << " - " << title;
    return str.str();
}

ostream &Hitje::operator<<(ostream &str) const {
    // "ddd: artist - title"
    str << setfill('0') << setw(3) << hitIndex << ": ";
    str << artist << " - " << title;
    return str;
}

Hitje::operator bool() const {
    // We are in business if even mediaData is set
    return mediaData != NULL;
}

bool Hitje::operator!() const {
    return mediaData == NULL;
}

bool Hitje::operator ==(const Hitje& other) const {
    return hitIndex == other.hitIndex && mediaData == other.mediaData && artist == other.artist && title == other.title && downloadState == other.downloadState;
}

bool Hitje::operator !=(const Hitje& other) const {
    return hitIndex != other.hitIndex || mediaData != other.mediaData || artist != other.artist || title != other.title || downloadState != other.downloadState;
}

Hitje &Hitje::operator=(const Hitje &other) {
    VLC::release(mediaData);
    hitIndex = other.hitIndex;
    path = other.path;
    downloadState = other.downloadState;

    init(other.mediaData, other.artist, other.title);
    return *this;
}


string Hitje::createFilename(bool absolute, bool extension) const {
    // "ddd - artist - title.mp3"
    ostringstream filenameStream;
    filenameStream << setfill('0') << setw(3) << hitIndex;
    filenameStream << " - " << artist;
    filenameStream << " - " << title;
    if (extension) {
        filenameStream << ".mp3";
    }
    string filename = filenameStream.str();
    filesystemSafe(filename);
    if (absolute) {
        filename = path + filename;
    }
    return filename;
}

libvlc_media_t *Hitje::getMediaData() const {
    VLC::retain(mediaData);
    return mediaData;
}

void Hitje::setMediaData(libvlc_media_t *newMediaData) {
    // decrease mediaData ref counter (implicitly release)
    VLC::release(mediaData);
    // and increment new ref counter;
    VLC::retain(newMediaData);
    mediaData = newMediaData;
}
