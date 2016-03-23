#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include "gui/GUI.h"
#include "Hitje.h"

class Downloader {
public:
    Downloader(GUI &gui);
    virtual ~Downloader();

    bool download(Hitje &hitje, ScraperData &scraperData);
    virtual bool download(Hitje &hitje, YoutubeScraperData &scraperData);
    virtual bool download(Hitje &hitje, SoundcloudScraperData &scraperData);
protected:
    GUI &gui;
private:
};

#endif // DOWNLOADER_H
