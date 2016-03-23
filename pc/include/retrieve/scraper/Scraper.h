#ifndef SCRAPER_H
#define SCRAPER_H

#include "Structs.h"
#include "gui/GUI.h"
#include "retrieve/HTTPTransfer.h"

class Scraper {
public:
    Scraper(GUI &gui);
    virtual ~Scraper();

    bool scrape(ScraperData &scraperData);
    virtual bool scrape(YoutubeScraperData &scraperData);
    virtual bool scrape(SoundcloudScraperData &scraperData);
protected:
    GUI &gui;
    HTTPTransfer transfer;
private:
};

#endif // SCRAPER_H
