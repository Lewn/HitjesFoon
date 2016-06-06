#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>

using namespace std;


enum InputVal {
    INPUT_EARTH_UP = 11,
    INPUT_EARTH_DOWN,
    INPUT_EARTH_SWAP,
    INPUT_HORN_UP,
    INPUT_HORN_DOWN,
    INPUT_HORN_SWAP,
    INPUT_UPDATE,
    INPUT_REFRESH,
    INPUT_RESET_LIST,
    INPUT_EXIT,
    INPUT_RESTART,
    INPUT_TEST,
    INPUT_NONE
};



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
