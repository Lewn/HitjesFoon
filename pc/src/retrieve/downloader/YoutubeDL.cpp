#include "retrieve/downloader/YoutubeDL.h"

YoutubeDL::YoutubeDL(GUI &gui) : Downloader(gui) {

}

YoutubeDL::~YoutubeDL() {

}



bool YoutubeDL::download(Hitje &hitje, YoutubeScraperData &scraperData) {
    try {
        // create filename without extension (switches between mp4 and mp3)
        string filename = hitje.createFilename(true, false) + ".mp4";
        gui.printlevel(LDEBUG, "Filename: '%s'\n", filename.c_str());

        gui.printlevel(LBGINFO, "\nStarted downloading '%s' using youtube-dl\n", filename.c_str());

        string cmd = "youtube-dl ";
        if (gui.getMsglevel() < PRINT_LEVEL::LBGINFO) {
            cmd += "--quiet --no-progress --no-warnings ";
        }
        cmd += "--newline ";
        cmd += "--extract-audio --audio-quality 9 --audio-format mp3 -o ";
        cmd += '"' + filename + '"';
        cmd += " -- ";
        cmd += scraperData.videoId;
        // redirect stderr to stdout, so we can read it
        cmd += " 2>&1";

        performCmd(hitje, cmd);
        return true;
    } catch (...) {
        // TODO catch more informative (and pass through unrecoverable errors)
        gui.printlevel(LERROR, "\nCaught an exception while using YoutubeDL\n");
        return false;
    }
}

void YoutubeDL::performCmd(Hitje &hitje, const string &cmd) {
    char buf[1024];
    cmatch match;
    // Search for output data (percentage, size, etc.)
    regex outputSearch("([0-9.]+)\\%\\s+of\\s+([0-9.]+)(G|M|K)iB\\s+at\\s+([0-9.]+)(G|M|K)iB/s\\s+ETA\\s+([0-9]+):([0-9]+)", regex_constants::ECMAScript | regex_constants::icase);
    regex errorSearch("ERROR: (.+)");

    // Open command asynchronously
    std::shared_ptr<FILE> cmdptr = cmdasync(cmd);
    // Keep reading command output
    while (!feof(cmdptr.get())) {
        if (fgets(buf, sizeof(buf), cmdptr.get()) != NULL) {
            gui.printlevel(LDEBUG, "%s", buf);
            if (regex_search(buf, match, outputSearch)) {
                hitje.downloadState.percentage = stof(match[1]);
                hitje.downloadState.dlsize = reformat(match[2], match[3]);
                hitje.downloadState.dlspeed = reformat(match[4], match[5]);
                hitje.downloadState.eta = 60 * stoi(match[6]) + stoi(match[7]);

                gui.setHitje(hitje);
            } else if (regex_search(buf, match, errorSearch)) {
                // TODO: throw better exceptions
                throw match[1].str().c_str();
            }
        }
    }
}

float YoutubeDL::reformat(const string &val, const string &type) {
    float fval = stof(val);
    // Convert kilo and giga to megabytes
    if (type == "K") {
        fval /= 1000;
    } else if (type == "G") {
        fval *= 1000;
    }
    return fval;
}
