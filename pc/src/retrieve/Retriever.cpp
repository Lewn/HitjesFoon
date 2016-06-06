#include "retrieve/Retriever.h"

Retriever::Retriever(GUI &gui) : gui(gui) {
    buildScraperChain();
    buildDownloaderChain();
}

Retriever::~Retriever() {

}


bool Retriever::retrieve(const Hitje &mainHitje) {

    if (mainHitje || mainHitje.downloadState.downloading || mainHitje.artist.empty() || mainHitje.title.empty()) {
        // Don't need download for whatever reason
        return false;
    }

    // Limit the amount of active retrieve sessions
    if (!Retriever::blockRetrieve(mainHitje)) {
        // Couldn't acquire download session, thus couldn't download
        return false;
    }
    // Make sure we are working with a copy for the remainder
    Hitje hitje(mainHitje);

    try {
        // Mark as downloading active
        hitje.setDownloading(true);
        // Signal start of download to gui
        gui.setHitje(hitje);
        gui.printlevel(LDEBUG, "Retrieving hitje %s\n", hitje.toString().c_str());
        bool downloaded = false;

        // Collect necessary data about hitje
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
        // Release retrieve session
        Retriever::endRetrieve(hitje);
        // Signal end of downloading to gui
        gui.printlevel(LINFO, "Successfully downloaded hitje %d\n", hitje.hitIndex);
        gui.setHitje(hitje);
        return downloaded;
    } catch (...) {
        // Make sure we end our session
        Retriever::endRetrieve(hitje);
        // Signal end of downloading to gui
        gui.setHitje(hitje);
        gui.printlevel(LERROR, "Something went terribly wrong while retrieving\n");
    }
    return false;
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

void Retriever::normaliseAudio(Hitje &hitje) {
    char buf[1024];
    string cmd = "mp3gain -e -t -r -k -s i";

    gui.printlevel(LBGINFO, "Normalising hitje %d\n", hitje.hitIndex);
//    -e            no album analysis (because we don't use album)
//    -r            Apply per track gain (to actually alter the audio)
//    -o            Nice output?
//    -t            Use temp file
//    -k            Automatically lower gain to not clip
    cmd += '"' + hitje.createFilename(true, true) + '"';
    // TODO without command (source is included, thus can compile directly)
    auto cmdptr = cmdasync(cmd.c_str());
    while (!feof(cmdptr.get())) {
        if (fgets(buf, sizeof(buf), cmdptr.get()) != NULL) {
            gui.printlevel(LDEBUG, "%s", buf);
        }
    }
    gui.printlevel(LBGINFO, "Done normalising audio\n");
}


void Retriever::buildScraperChain() {
    // TODO add a scraper which corrects typo's and can retrieve additional information
    // database: musicbrainz.org, for example using libmusicbrainz5
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
    // Make sure audio is normalised
    normaliseAudio(hitje);
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


atomic<int> Retriever::retrieveCnt = ATOMIC_VAR_INIT(MAX_RETRIEVE_SESSIONS);

bool Retriever::blockRetrieve(const Hitje &hitje) {
    // Try to acquire a session
    while (atomic_fetch_sub(&retrieveCnt, 1) <= 0) {
        atomic_fetch_add(&retrieveCnt, 1);
        // Sleep to give other threads a chance to do work
        this_thread::sleep_for(chrono::seconds(1));
    }

    if (!hitje.canDownload()) {
        // It is not actually possible to download hitje in this state, abort
        // Already downloaded or started downloading, cancel
        atomic_fetch_add(&retrieveCnt, 1);;
    }
    return true;
}

void Retriever::endRetrieve(Hitje &hitje) {
    // Simply reduce amount of active sessions
    atomic_fetch_add(&retrieveCnt, 1);;
    // Mark hitje as not downloading anymore
    hitje.setDownloading(false);
}
