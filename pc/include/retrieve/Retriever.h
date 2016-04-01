#ifndef RETRIEVER_H
#define RETRIEVER_H

#define MAX_RETRIEVE_SESSIONS   10

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <atomic>

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

    bool retrieve(const Hitje &hitje);
    bool createMediaFile(Hitje &hitje);
protected:
    GUI &gui;
    vector<std::shared_ptr<Scraper>> scraperChain;
    vector<std::shared_ptr<Downloader>> downloaderChain;

    bool retrieveYoutube(Scraper *scraper, ScraperData data, Hitje &hitje);
    bool retrieveSoundcloud(Scraper *scraper, ScraperData data, Hitje &hitje);

    void normaliseAudio(Hitje &hitje);

    void buildScraperChain();
    void buildDownloaderChain();
private:
    static atomic<int> retrieveCnt;

    static bool blockRetrieve(const Hitje &hitje);
    static void endRetrieve(Hitje &hitje);
};

#endif // RETRIEVER_H
