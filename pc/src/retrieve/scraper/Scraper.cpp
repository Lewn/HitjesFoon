#include "retrieve/scraper/Scraper.h"

Scraper::Scraper(GUI &gui) : gui(gui), transfer(gui) {

}

Scraper::~Scraper() {

}


bool Scraper::scrape(ScraperData &scraperData) {
    throw "Can't search generic scraper data, must specify a type";
}


bool Scraper::scrape(YoutubeScraperData &scraperData) {
    // Default implementation can't search
    return false;
}

bool Scraper::scrape(SoundcloudScraperData &scraperData) {
    // Default implementation can't search
    return false;
}
