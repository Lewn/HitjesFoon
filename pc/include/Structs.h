#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>

using namespace std;

enum ScraperDataType {
    INVALID,
    YOUTUBE,
    SOUNDCLOUD
};

struct ScraperData {
    ScraperDataType type = INVALID;
    string query;
};

struct YoutubeScraperData : public ScraperData {
    YoutubeScraperData(ScraperData &data);
    ScraperDataType type = YOUTUBE;
    string videoId;
};

struct SoundcloudScraperData : public ScraperData {
    SoundcloudScraperData(ScraperData &data);
    ScraperDataType type = SOUNDCLOUD;
};

struct DownloadState {
    bool downloading = false;
    float percentage = 0, dlsize = 0, dlspeed = 0;
    int eta = 0;
};

inline bool operator==(const DownloadState& lhs, const DownloadState& rhs) {
    return lhs.downloading == rhs.downloading && lhs.percentage == rhs.percentage && lhs.dlsize == rhs.dlsize && lhs.dlspeed == rhs.dlspeed && lhs.eta == rhs.eta;
}

inline bool operator!=(const DownloadState& lhs, const DownloadState& rhs) {
    return !(lhs == rhs);
}


#endif
