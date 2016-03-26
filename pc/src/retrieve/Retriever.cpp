#include "retrieve/Retriever.h"

Retriever::Retriever(GUI &gui) : gui(gui) {
    buildScraperChain();
    buildDownloaderChain();
}

Retriever::~Retriever() {

}


bool Retriever::retrieve(Hitje &hitje) {
    gui.printlevel(LDEBUG, "Retrieving hitje %s\n", hitje.toString().c_str());
    bool downloaded = false;
    // Signal start of download to gui
    hitje.downloadState.downloading = true;
    gui.setHitje(hitje);
    ScraperData data;
    data.query = hitje.artist + ' ' + hitje.title;
    for (auto &scraper : scraperChain) {
        // TODO try to weigh in best location to download from
        // maybe add configuration option to specify preferred download location
        // or generate a score depending on found video and only download thresh/highest score
        // First try to download from youtube
        if (retrieveYoutube(scraper.get(), data, hitje)) {
            downloaded = true;
            break;
        }
        // Else try to download from soundcloud
        if (retrieveSoundcloud(scraper.get(), data, hitje)) {
            downloaded = true;
            break;
        }
        // Try next scraper instead
    }
    // Generate media file, attach it to hitje
    downloaded &= createMediaFile(hitje);
    // Signal end of downloading to gui
    hitje.downloadState.downloading = false;
    gui.setHitje(hitje);
    return downloaded;
}

bool Retriever::retrieveYoutube(Scraper *scraper, ScraperData data, Hitje &hitje) {
    YoutubeScraperData ytData = (YoutubeScraperData) data;
    if (scraper->scrape(ytData)) {
        // Successfully scraped data (and stored in ytData), try downloading
        for (auto &downloader : downloaderChain) {
            if (downloader->download(hitje, ytData)) {
                // Successfully downloaded, return success
                return true;
            }
            // Try next downloader
        }
    }
    // No downloader could retrieve data, return
    return false;
}

bool Retriever::retrieveSoundcloud(Scraper *scraper, ScraperData data, Hitje &hitje) {
    SoundcloudScraperData scData = (SoundcloudScraperData) data;
    if (scraper->scrape(scData)) {
        // Successfully scraped data (and stored in scData), try downloading
        for (auto &downloader : downloaderChain) {
            if (downloader->download(hitje, scData)) {
                // Successfully downloaded, return success
                return true;
            }
            // Try next downloader
        }
    }
    // No downloader could retrieve data, return
    return false;
}


void Retriever::buildScraperChain() {
    // Only insert elements if not previously filled
    if (scraperChain.empty()) {
        scraperChain.push_back(make_shared<YoutubeAPIScraper>(gui));
    }
}

void Retriever::buildDownloaderChain() {
    // Only insert elements if not previously filled
    if (downloaderChain.empty()) {
        downloaderChain.push_back(make_shared<YoutubeDL>(gui));
    }
}

bool Retriever::createMediaFile(Hitje &hitje) {
    // Create the media file, check if it is parseable
    // Construct filename including path
    string hitjePath = hitje.createFilename(true);
    gui.printlevel(LDEBUG, "Trying to open media data\n");
    // Try to open the file
    ifstream hitjeFileStream(hitjePath.c_str(), ifstream::binary);
    if (!hitjeFileStream.good()) {
        // Error during opening, don't parse it further (may not exist?)
        gui.printlevel(LDEBUG, "Couldn't open file, no media data\n");
        return false;
    }
    hitjeFileStream.close();
    // Try to parse the file as media data
    gui.printlevel(LDEBUG, "Parsing media file '%s'\n", hitjePath.c_str());
    libvlc_media_t *mediaData = VLC::getInstance()->newMediaFromPath(hitjePath.c_str());
    if (mediaData == NULL) {
        // Something went wrong opening the media (wrong format, corrupted etc.)
        gui.printlevel(LERROR, "Invalid file, '%s'\n", hitjePath.c_str());
        return false;
    }
    // Successfully parsed media file, means we can use it
    // Store it in the hitje
    hitje.setMediaData(mediaData);
    gui.printlevel(LDEBUG, "Successfully parsed media file\n");
    // Release the data (decrease refcount, hitje keeps its own)
    VLC::release(mediaData);
    return true;
}
