#ifndef YOUTUBEMANUALDOWNLOADER_H
#define YOUTUBEMANUALDOWNLOADER_H

#include "retrieve/downloader/Downloader.h"

class YoutubeManualDownloader : public Downloader {
public:
    YoutubeManualDownloader(GUI &gui);
    virtual ~YoutubeManualDownloader();
protected:
private:
};

#endif // YOUTUBEMANUALDOWNLOADER_H
