#include "AudioList.h"

Hitje::Hitje(libvlc_media_t *mediaData, int hitIndex, string title, string artist) : hitIndex(hitIndex) {
    this->mediaData = mediaData;
    this->title = title;
    this->artist = artist;
}

Hitje::~Hitje() {
    VLC::release(mediaData);
}

int AudioList::checkMediaFile(libvlc_media_t *mediaFile) {
    libvlc_media_parse(mediaFile);
    if (!libvlc_media_is_parsed(mediaFile)) {
        return 0;
    }
    return libvlc_media_get_duration(mediaFile);
}

AudioList::AudioList(Config *config) {
    DIR *dir;
    // save the hitjeslist string in own memory for future reference
    do {
        listFilePath = config->getHitjesList();
        listFile = fopen(listFilePath.c_str(), "r");
        if (listFile == NULL) {
            printlevel(LWARNING, "Invalid list file specified, '%s', %d\n", config->getHitjesList().c_str(), errno);
        } else {
            // get the last backslash from hitjesPath for making relative paths
            hitjesPath = config->getHitjesPath();
            dir = opendir(hitjesPath.c_str());
            if (hitjesPath.empty() || dir == NULL) {
                SAFE_CLOSE(listFile);
                printlevel(LWARNING, "Invalid hitjespath specified, '%s', %d", hitjesPath.c_str(), errno);
            }
            closedir(dir);
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
            printlevel(LINFO, "\n");
            if (!config->nextHitjesConfig()) {
                throw "No valid hitjes config found";
            }
            printlevel(LINFO, "Trying next hitjes config...\n");
        }
    } while (listFile == NULL);

    printlevel(LINFO, "Creating the hitjeslist\n");
    hitjesList.resize(999, NULL);
    update(0);
}

AudioList::~AudioList() {
    // clear hitjeslist
    for (unsigned int i = 0; i < hitjesList.size(); i++) {
        SAFE_DELETE(hitjesList[i]);
    }
    hitjesList.clear();
}

libvlc_media_t *AudioList::getAudio(int audioIndex) {
    if (audioIndex < 0 || audioIndex > 999 || hitjesList[audioIndex] == NULL) {
        return NULL;
    }
    return hitjesList[audioIndex]->mediaData;
}

string AudioList::createHitjeName(const Hitje * hitje, bool absolute) {
    return createHitjeName(hitje->hitIndex, hitje->title, hitje->artist, absolute);
}

string AudioList::createHitjeName(int hitIndex, string title, string artist, bool absolute) {
    trim(title);
    trim(artist);
    if (title.empty()) {
        title = "notitle";
    }
    if (artist.empty()) {
        artist = "unknown";
    }
    // "ddd - artist: title.mp3"
    ostringstream videoNameStream;
    videoNameStream << setfill('0') << setw(3) << hitIndex;
    videoNameStream << " - " << artist << " - " << title << ".mp3";
    string videoName = videoNameStream.str();
    filesystemSafe(videoName);
    if (absolute) {
        return hitjesPath + videoName;
    }
    return videoName;
}


bool AudioList::update(unsigned int downloadCount) {
    this->downloadCount = downloadCount;
    int hitIndex = 0;
    char buffer[1000];
    char title[200];
    char artist[200];
    string hitjePath;
    int hitjes = 0;

    buffer[0] = title[0] = artist[0] = '\0';

    // Create a new list with 999 hitjes
    vector<Hitje *> newHitjesList(999);

    if (listFile == NULL) {
        listFile = fopen(listFilePath.c_str(), "r");
    }
    string fileOutput;

    VLC *VLCInstance = VLC::getInstance();

    skipInvalidLines(buffer, &hitIndex, title, artist, fileOutput);
    do {
        if (this->downloadCount && (readKeyboard() != INPUT_NONE)) {
            this->downloadCount = 0;
        }

        if (hitIndex <= 0 || hitIndex >= 999) {
            printlevel(LDEBUG, "Wrong hitindex, skipping %d: '%s' '%s'\n", hitIndex, title, artist);
        } else if (newHitjesList[hitIndex]) {
            printlevel(LERROR, "Hitindex %d is defined multiple times\n", hitIndex);
        } else {
            // and use all data to create a new element in list
            hitjePath = createHitjeName(hitIndex, title, artist, true);
            FILE *hitjeFile = fopen(hitjePath.c_str(), "r");
            if (hitjeFile != NULL) {
                printlevel(LDEBUG, "Parsing media file '%s'\n", hitjePath.c_str());
                libvlc_media_t *mediaFile = VLCInstance->newMediaFromPath(hitjePath.c_str());
                if (!checkMediaFile(mediaFile)) {
                    printlevel(LERROR, "Invalid file, '%s'\n", hitjePath.c_str());
                } else {
                    newHitjesList[hitIndex] = new Hitje(mediaFile, hitIndex, title, artist);
                    hitjes++;
                    printlevel(LDEBUG, "Successfully parsed media file\n");
                }
                SAFE_CLOSE(hitjeFile);
            }
        }
        printlevel(LDEBUG, "Retrieving next line\n");
        do {
            if (fgets(buffer, 999, listFile) && !parseBuf(buffer, &hitIndex, title, artist, fileOutput)) {
                hitIndex = -1;
            }
        } while (!feof(listFile) && hitIndex == -1);
    } while (!feof(listFile));
    SAFE_CLOSE(listFile);

    // TODO: delete all files not in list
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
                if (newHitjesList[hitIndex]) {
                    string hitjeName = createHitjeName(newHitjesList[hitIndex], false);
                    if (!strcmp(ent->d_name, hitjeName.c_str())) {
                        continue;
                    }
                }
                string absPath = hitjesPath + ent->d_name;
                printlevel(LWARNING, "Deleting unused file '%s'\n", ent->d_name);
                remove(absPath.c_str());
            }
        }
        closedir(dir);
    } else {
        printlevel(LERROR, "Could not clean hitjes files, directory not accessible\n");
    }


// TODO: write updates to list (config?) file
    //listFile = fopen(listFilePath, "w");
    //fputs(fileOutput.data(), listFile);
    //SAFE_CLOSE(listFile);
    listFile = NULL;

// clear old hitjeslist
    for (unsigned int i = 0; i < hitjesList.size(); i++) {
        SAFE_DELETE(hitjesList[i]);
        hitjesList[i] = newHitjesList[i];
    }
    newHitjesList.clear();
    printlevel(LINFO, "\nFound a total of %d hitjes\n", hitjes);
// return wether there is more to download
    return this->downloadCount == 0;
}

void AudioList::skipInvalidLines(char *buffer, int *hitIndex, char *title, char *artist, string &fileOutput) {
    // loop through all lines without hitjes
    do {
        if (feof(listFile)) {
            // reached end of file without any hitjes
            throw "No valid hitjes file";
        }
        fgets(buffer, 999, listFile);
    } while (parseBuf(buffer, hitIndex, title, artist, fileOutput));
    printlevel(LDEBUG, "Skipped all non hitjes lines\n");
}

bool AudioList::parseBuf(char *buffer, int *hitIndex, char *title, char *artist, string &fileOutput) {
    *hitIndex = 0;
    artist[0] = title[0] = 0;
    printlevel(LDEBUG, "Parsing buffer:\n");
    printlevel(LDEBUG, "%s", buffer);
    int res = sscanf(buffer, "%d;%[^;];%[^;\n]", hitIndex, title, artist);
    printlevel(LDEBUG, "Got %d: '%s' '%s'\n", *hitIndex, title, artist);

    if (!hitIndex || !*hitIndex) {
        // couldn't read line, reset
        res = 0;
    } else if (res >= 2 && strlen(trim(title)) == 0) {
        // No title, don't search this file
        res = 1;
    }
    if (res <= 1) {
        // No valid line, just keep the contents
        fileOutput += buffer;
        return false;
    }
    string hitjePath = createHitjeName(*hitIndex, title, artist, true);
    FILE *fileTest = fopen(hitjePath.c_str(), "r");
    if (downloadCount && fileTest == NULL) {
        // We need to download more and file does not exist yet
        printlevel(LDEBUG, "\n\nTrying to download youtube\n");
        // no file found, title and artist are given, download from internet
        string newpath = getVideoFile(*hitIndex, title, artist);

        if (!newpath.empty()) {
            fileTest = fopen(hitjePath.c_str(), "r");
            if (fileTest == NULL) {
                printlevel(LWARNING, "Could not open '%s'\n", hitjePath.c_str());
                // We should have just downloaded this exact file, something went wrong
                throw "File could not be downloaded correctly";
            }
            downloadCount--;
            printlevel(LINFO, "\nDownloaded and added new hitje %s\n", hitjePath.c_str());

        }
    }
    SAFE_CLOSE(fileTest);

    // Some valid data, copy all the data that was valid
    fileOutput += to_string(*hitIndex);
    fileOutput += ';';
    fileOutput += title;
    fileOutput += ';';
    fileOutput += artist;
    fileOutput += "\n";
    // If the file existed already, we have a valid index
    return true;
}

string AudioList::getVideoFile(int hitIndex, const char *title, const char *artist) {
    try {
        string query = title;
        query += ' ';
        query += artist;
        string baseName = api.searchVid(query.c_str(), hitjesPath.c_str());

        if (!baseName.empty()) {
            string videoName = createHitjeName(hitIndex, title, artist, false);

            printlevel(LBGINFO, "Done searching and found %s\n", videoName.c_str());
            // create path for saving audio data
            string videoPath = hitjesPath + videoName;

#if defined (USE_YOUTUBE_DL)
            // audio format already correct mp3, just move the file
            printlevel(LBGINFO, "Moving '%s' to '%s'\n", baseName.c_str(), videoPath.c_str());
            if (rename(baseName.c_str(), videoPath.c_str())) {
                printlevel(LERROR, "Could not move file to correct location\n");
                printlevel(LERROR, "errno: %d\n", errno);
            }
#else
            // use ffmpeg to get the audio file from video
            string decodeCmd = "ffmpeg -n -i ";
            decodeCmd += '"' + baseName + '"';
            decodeCmd += " -f mp3 -vn ";
            decodeCmd += '"' + videoPath + '"';
            printlevel(LDEBUG, "calling ffmpeg with '%s'\n", decodeCmd.c_str());
            system(decodeCmd.c_str());

            FILE *musicFile = fopen(videoPath.c_str(), "r");
            bool fileExists = musicFile != NULL;
            SAFE_CLOSE(musicFile);

            if (!fileExists) {
                throw "File conversion failed";
            }

            // video file is not needed anymore, SAFE_DELETE(it
            remove(baseName.c_str());
            printlevel(LBGINFO, "Basename: %s\n", baseName.c_str());
            printlevel(LBGINFO, "Removed old file\n");
#endif // USE_YOUTUBE_DL
            printlevel(LBGINFO, "\n\nSuccessfully got file '%s'\n", videoName.c_str());
            return videoName;
        } else {
            printlevel(LWARNING, "No suitable video file found\n");
        }
    } catch (const char *e) {
        // on any error, still continue
        printlevel(LERROR, "An error occured while parsing, press any key to continue\n%s\n", e);
        getchar();
    }
    return "";
}
