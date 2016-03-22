
#include "Structs.h"
#include "VLC.h"


inline bool operator==(const DownloadState& lhs, const DownloadState& rhs) {
    return lhs.downloading == rhs.downloading && lhs.percentage == rhs.percentage && lhs.dlsize == rhs.dlsize && lhs.dlspeed == rhs.dlspeed && lhs.eta == rhs.eta;
}

inline bool operator!=(const DownloadState& lhs, const DownloadState& rhs) {
    return !(lhs == rhs);
}


Hitje::Hitje(int hitIndex) : Hitje(NULL, hitIndex, "", "") {
}

Hitje::Hitje(libvlc_media_t *mediaData, int hitIndex, string artist, string title) : hitIndex(hitIndex) {
    // increase mediaData ref counter
    VLC::retain(mediaData);
    this->mediaData = mediaData;
    this->artist = trim(artist);
    this->title = trim(title);
    if (this->artist.empty()) {
        this->artist = "unknown";
    }
    if (this->title.empty()) {
        this->title = "no title";
    }
}

Hitje::Hitje(const Hitje &other) : Hitje(other.mediaData, other.hitIndex, other.artist, other.title) {
    // also copy download state
    downloadState = other.downloadState;
}

Hitje::~Hitje() {
    // decrease mediaData ref counter (implicitly release)
    VLC::release(mediaData);
}

string Hitje::toString() const {
    // "ddd: artist - title"
    stringstream str;
    str << *this;
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

Hitje::operator!() const {
    return mediaData == NULL;
}

Hitje::operator ==(const Hitje& other) const {
    return hitIndex == other.hitIndex && mediaData == other.mediaData && artist == other.artist && title == other.title && downloadState == other.downloadState;
}

Hitje::operator !=(const Hitje& other) const {
    return hitIndex != other.hitIndex || mediaData != other.mediaData || artist != other.artist || title != other.title || downloadState != other.downloadState;
}


string Hitje::createFilename() const {
    // "ddd: artist - title.mp3"
    ostringstream filenameStream;
    filenameStream << setfill('0') << setw(3) << hitIndex;
    filenameStream << " - " << title << ".mp3";
    string filename = filenameStream.str();
    filesystemSafe(filename);
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
