#include "retrieve/downloader/Downloader.h"

Downloader::Downloader(GUI &gui) : gui(gui) {

}

Downloader::~Downloader() {

}


bool Downloader::download(Hitje &hitje, ScraperData &scraperData) {
    throw "Can't search generic scraper data, must specify a type";
}


bool Downloader::download(Hitje &hitje, YoutubeScraperData &scraperData) {
    // Default implementation can't search
    return false;
}

bool Downloader::download(Hitje &hitje, SoundcloudScraperData &scraperData) {
    // Default implementation can't search
    return false;
}
