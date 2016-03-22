#include "AudioList.h"

AudioList::AudioList(GUI &gui, Config &config) : gui(gui), api(gui) {
    DIR *dir;
    // save the hitjeslist string in own memory for future reference
    FILE *listFile = NULL;
    do {
        listFilePath = config.getHitjesList();
        listFile = fopen(listFilePath.c_str(), "r");
        if (listFile == NULL) {
            gui.printlevel(LWARNING, "Invalid list file specified, '%s', %d\n", config.getHitjesList().c_str(), errno);
        } else {
            // get the last backslash from hitjesPath for making relative paths
            hitjesPath = config.getHitjesPath();
            dir = opendir(hitjesPath.c_str());
            if (hitjesPath.empty() || dir == NULL) {
                SAFE_CLOSE(listFile);
                gui.printlevel(LWARNING, "Invalid hitjespath specified, '%s', %d", hitjesPath.c_str(), errno);
            }
            closedir(dir);
            // TODO use boost filesystem
#ifdef _WIN32   // Stupid windows paths
            if (hitjesPath.back() != '\\') {
                hitjesPath += '\\';
            }
#else
            if (hitjesPath.back() != '/') {
                hitjesPath += '/';
            }
#endif
        }
        if (listFile == NULL) {
            gui.printlevel(LINFO, "\n");
            if (!config.nextHitjesConfig()) {
                throw "No valid hitjes config found";
            }
            gui.printlevel(LINFO, "Trying next hitjes config...\n");
        }
    } while (listFile == NULL);
    SAFE_CLOSE(listFile);

    gui.printlevel(LINFO, "Creating the hitjeslist\n");
    // TODO no hitje 0 needed
    for (int i = 0; i < 1000; i++) {
        hitjes.insert(hitjes.end(), Hitje(i));
    }
    update(0);
}

AudioList::~AudioList() {
}

const Hitje &AudioList::getHitje(int hitjeIndex) {
    if (hitjeIndex < 0 || hitjeIndex > 999) {
        throw "Hitindex out of range";
    }
    return hitjes[hitjeIndex];
}


bool AudioList::update(unsigned int downloadCount) {
    this->downloadCount = downloadCount;
    int hitIndex;
    string fileOutput;
    ifstream listFileStream;

    gui.printlevel(LINFO, "Updating..\n");
    // enable exceptions on fail to open or read
    // TODO throws a basic_ios::clear exception? (cannot be properly handled)
//    listFileStream.exceptions(ifstream::failbit | ifstream::badbit);
    listFileStream.open(listFilePath.c_str(), ifstream::binary);

    printf("Skipping lines\n");
    hitIndex = skipInvalidLines(listFileStream, fileOutput);
    do {
        if (hitIndex <= 0 || hitIndex >= 999) {
            gui.printlevel(LDEBUG, "Wrong hitindex, skipping %d\n", hitIndex);
        } else {
            // Got hitje successfully
            gui.printlevel(LBGINFO, "Found hitje %d\n", hitIndex);
        }
        do {
            hitIndex = readLine(listFileStream, fileOutput);
        } while (!listFileStream.eof() && hitIndex == 0);
    } while (!listFileStream.eof());
    listFileStream.close();

    // All hitjes must start with a three digit number,
    // so delete all files that don't have this or whose
    // number is not in the hitjes list
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(hitjesPath.c_str())) != NULL) {
        char intbuf[4];
        intbuf[3] = '\0';
        // Loop over all files in hitjes directory
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                // Get first three digits and parse int.
                strncpy(intbuf, ent->d_name, 3);
                sscanf(intbuf, "%d", &hitIndex);
                if (hitjes[hitIndex]) {
                    string hitjeFilename = hitjes[hitIndex].createFilename();
                    if (!strcmp(ent->d_name, hitjeFilename.c_str())) {
                        continue;
                    }
                }
                string absPath = hitjesPath + ent->d_name;
                gui.printlevel(LWARNING, "Deleting unused file '%s'\n", ent->d_name);
                remove(absPath.c_str());
            }
        }
        closedir(dir);
    } else {
        gui.printlevel(LERROR, "Could not clean hitjes files, directory not accessible\n");
    }


// TODO: write updates to list (config?) file
    //listFile = fopen(listFilePath, "w");
    //fputs(fileOutput.data(), listFile);
    //SAFE_CLOSE(listFile);

    for (const Hitje &hitje : hitjes) {
        gui.setHitje(hitje);
    }

    // return whether there is more to download
    return this->downloadCount == 0;
}

int AudioList::skipInvalidLines(ifstream &listFileStream, string &fileOutput) {
    int hitIndex;
    // loop through all lines without hitjes
    do {
        if (listFileStream.eof()) {
            // reached end of file without any hitjes
            throw "No valid hitjes file";
        }
        hitIndex = readLine(listFileStream, fileOutput);
    } while (hitIndex == 0);
    gui.printlevel(LDEBUG, "Skipped all non hitjes lines\n");
    return hitIndex;
}

int AudioList::readLine(ifstream &listFileStream, string &fileOutput) {
    int hitIndex;
    string title, artist;
    // TODO no use of buffer possible? (detect newline manually)
    stringbuf line;
    gui.printlevel(LDEBUG, "Retrieving next line\n");
    // Read a line in buffer
    listFileStream.get(line);
    // ignore newline characters
    listFileStream.ignore();
    istringstream buffer(line.str());
    gui.printlevel(LDEBUG, "\n%s\n", buffer.str().c_str());

    buffer >> hitIndex;             // read hit index
    // skip until ;
    buffer.ignore(numeric_limits<streamsize>::max(), ';');
    getline(buffer, title, ';');    // read until ; as title
    getline(buffer, artist, ';');   // read until ; as artist

    title = trim(title);
    artist = trim(artist);
    gui.printlevel(LDEBUG, "Got %d: '%s' '%s'\n", hitIndex, title.c_str(), artist.c_str());

    if (hitIndex <= 0 || hitIndex >= 999 || title.empty()) {
        // Some of the information was missing or wrong
        // TODO inform user of wrong information
        // Just keep the contents
        fileOutput += line.str();
        // Signal an invalid line
        return 0;
    }

    // Store all data in hitje
    hitjes[hitIndex].title = title;
    hitjes[hitIndex].artist = artist;
    // Try to create the media file for this hitje
    if (!createMediaFile(hitjes[hitIndex]) && downloadCount > 0) {
        // Couldn't create, no worries, just download it
        gui.printlevel(LDEBUG, "\n\nTrying to download youtube\n");
        if (downloadVideoFile(hitjes[hitIndex])) {
            // Successfully downloaded new file, decrease download counter
            downloadCount--;
            gui.printlevel(LINFO, "\nDownloaded and added new hitje %s\n", hitjes[hitIndex].toString().c_str());
        }
    }

    // Some valid data, copy all the data that was valid
    fileOutput += to_string(hitIndex);
    fileOutput += ';';
    fileOutput += title;
    fileOutput += ';';
    fileOutput += artist;
    fileOutput += "\n";
    // If the file existed already, we have a valid index
    return hitIndex;
}

bool AudioList::downloadVideoFile(Hitje &hitje) {
    try {
        string query = hitje.artist + ' ' + hitje.title;
        if (api.searchVid(hitje, query, hitjesPath)) {
            return createMediaFile(hitje);
        } else {
            gui.printlevel(LWARNING, "No suitable video file found\n");
        }
    } catch (const char *e) {
        // on any error, still continue
        gui.confirm(LERROR, "An error occured while parsing, press any key to continue\n%s\n", e);
    }
    return false;
}


bool AudioList::createMediaFile(Hitje &hitje) {
    // Create the media file, check if it is parseable
    string hitjePath = hitjesPath + hitje.createFilename();
    // Try to open the file
    ifstream hitjeFileStream(hitjePath.c_str(), ifstream::binary);
    if (!hitjeFileStream.good()) {
        // Error during opening, don't parse it further (may not exist?)
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
    gui.setHitje(hitje);
    gui.printlevel(LDEBUG, "Successfully parsed media file\n");
    // Release the data (decrease refcount, hitje keeps its own)
    VLC::release(mediaData);
    return true;
}



