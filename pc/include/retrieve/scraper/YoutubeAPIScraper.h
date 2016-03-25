#ifndef YOUTUBEAPISCRAPER_H
#define YOUTUBEAPISCRAPER_H


#define DEVELOPER_KEY "AIzaSyCHwvjNTHiHoPC8tW6_ibs5MYbakDE8kTg" //Hitjesfoon
//#define DEVELOPER_KEY "AIzaSyBghZHHsanJEnahktJ5LmWbEYBv0WsgyVc" //Leon

#include "Structs.h"
#include "Tools.h"
#include "retrieve/scraper/Scraper.h"
#include "gui/GUI.h"
#include "retrieve/HTTPTransfer.h"
#include "JsonParser.h"

class YoutubeAPIScraper : public Scraper {
public:
//    using Scraper::scrape;
    YoutubeAPIScraper(GUI &gui);
    virtual ~YoutubeAPIScraper();

    bool scrape(YoutubeScraperData &scraperData) override;
protected:
    string makeRequest(const char *part, const char *fields, unsigned char maxResults, const char *extra);
private:
};

#endif // YOUTUBEAPISCRAPER_H
