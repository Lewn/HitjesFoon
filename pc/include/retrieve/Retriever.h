#ifndef RETRIEVER_H
#define RETRIEVER_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>

#include "gui/GUI.h"
#include "Hitje.h"
#include "VLC.h"

#include "retrieve/scraper/Scraper.h"
#include "retrieve/scraper/YoutubeAPIScraper.h"

#include "retrieve/downloader/Downloader.h"
#include "retrieve/downloader/YoutubeDL.h"

using namespace std;

class Retriever {
public:
    Retriever(GUI &gui);
    virtual ~Retriever();

    bool retrieve(Hitje &hitje);
    bool createMediaFile(Hitje &hitje);
protected:
    GUI &gui;
    vector<std::shared_ptr<Scraper>> scraperChain;
    vector<std::shared_ptr<Downloader>> downloaderChain;

    bool retrieveYoutube(Scraper *scraper, ScraperData data, Hitje &hitje);
    bool retrieveSoundcloud(Scraper *scraper, ScraperData data, Hitje &hitje);

    void buildScraperChain();
    void buildDownloaderChain();
private:
};

#endif // RETRIEVER_H
