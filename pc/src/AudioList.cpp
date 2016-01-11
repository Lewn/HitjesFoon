#include "AudioList.h"

Hitje::Hitje(libvlc_media_t *mediaData, int hitIndex, char *title, char *artist) : hitIndex(hitIndex) {
    this->mediaData = mediaData;
    this->title = new char[strlen(title) + 1];
    strcpy(this->title, title);
    this->artist = new char[strlen(artist) + 1];
    strcpy(this->artist, artist);
}

Hitje::~Hitje() {
    VLC::release(mediaData);
    SAFE_DELETE_ARRAY(title);
    SAFE_DELETE_ARRAY(artist);
}

int AudioList::checkMediaFile(libvlc_media_t* mediaFile) {
    libvlc_media_parse(mediaFile);
    if (!libvlc_media_is_parsed(mediaFile)) {
        return 0;
    }
    return libvlc_media_get_duration(mediaFile);
}

AudioList::AudioList(const char* listFilePath, const char *hitjesPath) {
    this->listFilePath = NULL;
    listFile = fopen(listFilePath, "r");
    if (listFile == NULL) {
        printlevel(LERROR, "File read error %d", errno);
        throw "File not readable";
    }

    // save the hitjeslist string in own memory for future reference
    this->listFilePath = new char[strlen(listFilePath) + 1];
    strcpy(this->listFilePath, listFilePath);

    // get the last backslash from hitjesPath for making relative paths
    hitjesPathLen = strrchr(hitjesPath, '\\') - hitjesPath + 1;
    if (!hitjesPathLen) {
        throw "No valid hitjespath specified";
    }
    this->hitjesPath = new char[strlen(hitjesPath) + 1];
    strcpy(this->hitjesPath, hitjesPath);

    printlevel(LINFO, "Creating the hitjeslist\n");
    hitjesList = 0;
    update(0);
}

AudioList::~AudioList() {
    SAFE_DELETE_ARRAY(listFilePath);
    SAFE_DELETE_ARRAY(hitjesPath);

    // clear hitjeslist
    if (hitjesList) {
        for (int i = 0; i < sizeof(hitjesList); i++) {
            SAFE_DELETE(hitjesList[i]);
        }
        SAFE_DELETE_ARRAY(hitjesList);
    }
}

libvlc_media_t* AudioList::getAudio(int audioIndex) {
    if (audioIndex < 0 || audioIndex > 999 || !hitjesList[audioIndex]) {
        return 0;
    }
    return hitjesList[audioIndex]->mediaData;
}

char *AudioList::createHitjeName(const Hitje *hitje, bool absolute) {
    return createHitjeName(hitje->hitIndex, hitje->title, hitje->artist, absolute);
}

char *AudioList::createHitjeName(int hitIndex, char *title, char *artist, bool absolute) {
    const char *titleTrim = trim(title);
    const char *artistTrim = trim(artist);
    if (strlen(artistTrim) == 0) {
        artistTrim = "unknown";
    }
    if (strlen(titleTrim) == 0) {
        titleTrim = "notitle";
    }
    // "ddd - artist: title.mp3"
    int videoNameLen = 6 + strlen(artistTrim) + 3 + strlen(titleTrim) + 4;
    char *videoName = new char[videoNameLen + 1];
    sprintf(videoName, "%03d - %s - %s.mp3", hitIndex, artistTrim, titleTrim);
    filesystemSafe(videoName);
    if (absolute) {
        char *absPath = getAbsolutePath(hitjesPath, hitjesPathLen, videoName);
        SAFE_DELETE_ARRAY(videoName);
        return absPath;
    }
    return videoName;
}


bool AudioList::update(unsigned int downloadCount) {
    this->downloadCount = downloadCount;
    int hitIndex = 0;
    char buffer[1000];
    char title[200];
    char artist[200];
    char *hitjePath = 0;
    int hitjes = 0;

    buffer[0] = title[0] = artist[0] = '\0';

    // Create a new list with 999 hitjes
    Hitje **newHitjesList = new Hitje*[999];
    memset(newHitjesList, 0, 999 * sizeof(newHitjesList));

    if (listFile == NULL) {
        listFile = fopen(listFilePath, "r");
    }
    string fileOutput;

    VLC* VLCInstance = VLC::getInstance();

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
            FILE *hitjeFile = fopen(hitjePath, "r");
            if (hitjeFile != NULL) {
                printlevel(LDEBUG, "Parsing media file '%s'\n", hitjePath);
                libvlc_media_t* mediaFile = VLCInstance->newMediaFromPath(hitjePath);
                if (!checkMediaFile(mediaFile)) {
                    printlevel(LERROR, "Invalid file, '%s'\n", hitjePath);
                } else {
                    newHitjesList[hitIndex] = new Hitje(mediaFile, hitIndex, title, artist);
                    hitjes++;
                    printlevel(LDEBUG, "Successfully parsed media file\n");
                }
                fclose(hitjeFile);
            }
            SAFE_DELETE_ARRAY(hitjePath);
        }
        printlevel(LDEBUG, "Retrieving next line\n");
        do {
            if (fgets(buffer, 999, listFile) && !parseBuf(buffer, &hitIndex, title, artist, fileOutput)) {
                hitIndex = -1;
            }
        } while (!feof(listFile) && hitIndex == -1);
    } while (!feof(listFile));
    fclose(listFile);

    // TODO: delete all files not in list
    // All hitjes must start with a three digit number,
    // so delete all files that don't have this or whose
    // number is not in the hitjes list
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (hitjesPath)) != NULL) {
        char intbuf[4];
        intbuf[3] = '\0';
        // Loop over all files in hitjes directory
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                // Get first three digits and parse int.
                strncpy(intbuf, ent->d_name, 3);
                sscanf(intbuf, "%d", &hitIndex);
                if (newHitjesList[hitIndex]) {
                    char *hitjeName = createHitjeName(newHitjesList[hitIndex], false);
                    if (!strcmp(ent->d_name, hitjeName)) {
                        SAFE_DELETE_ARRAY(hitjeName);
                        continue;
                    }
                    SAFE_DELETE_ARRAY(hitjeName);
                }
                char *absPath = getAbsolutePath(hitjesPath, hitjesPathLen, ent->d_name);
                printlevel(LWARNING, "Deleting unused file '%s'\n", ent->d_name);
                remove(absPath);
                SAFE_DELETE_ARRAY(absPath);
            }
        }
        closedir(dir);
    } else {
        printlevel(LERROR, "Could not clean hitjes files, directory not accessible\n");
    }


// TODO: write updates to list (config?) file
    //listFile = fopen(listFilePath, "w");
    //fputs(fileOutput.data(), listFile);
    //fclose(listFile);
    listFile = NULL;

// clear old hitjeslist
    if (hitjesList) {
        for (int i = 0; i < sizeof(hitjesList); i++) {
            SAFE_DELETE(hitjesList[i]);
        }
        SAFE_DELETE_ARRAY(hitjesList);
    }
    hitjesList = newHitjesList;
    printlevel(LINFO, "\nFound a total of %d hitjes\n", hitjes);
// return wether there is more to download
    return this->downloadCount == 0;
}

void AudioList::skipInvalidLines(char* buffer, int *hitIndex, char *title, char *artist, string &fileOutput) {
    // loop through all lines without hitjes
    do {
        if (feof(listFile)) {
            // reached end of file without any hitjes
            throw "No valid hitjes file";
        }
        fgets(buffer, 999, listFile);
    } while(parseBuf(buffer, hitIndex, title, artist, fileOutput));
    printlevel(LDEBUG, "Skipped all non hitjes lines\n");
}

bool AudioList::parseBuf(char* buffer, int *hitIndex, char *title, char *artist, string &fileOutput) {
    *hitIndex = 0;
    artist[0] = title[0] = 0;
    printlevel(LDEBUG, "Parsing buffer:\n");
    printlevel(LDEBUG, buffer);
    int res = sscanf(buffer, "%d;%[^;];%[^;]", hitIndex, title, artist);
    printlevel(LDEBUG, "Got %d: '%s' '%s'\n", *hitIndex, title, artist);

    if (!hitIndex || !*hitIndex) {
        // couldn't read line, reset
        res = 0;
    }else if (res >= 2 && strlen(trim(title)) == 0) {
        // No title, don't search this file
        res = 1;
    }
    if (res <= 1) {
        // No valid line, just keep the contents
        fileOutput += buffer;
        return false;
    }
    char *hitjePath = createHitjeName(*hitIndex, title, artist, true);
    FILE *fileTest = fopen(hitjePath, "r");
    if (downloadCount && fileTest == NULL) {
        // We need to download more and file does not exist yet
        printlevel(LDEBUG, "\n\nTrying to download youtube\n");
        // no file found, title and artist are given, download from internet
        char *newpath = getVideoFile(*hitIndex, title, artist);
        if (newpath) {
            SAFE_DELETE_ARRAY(newpath);
            fileTest = fopen(hitjePath, "r");
            if (fileTest == NULL) {
                SAFE_DELETE_ARRAY(hitjePath);
                // We should have just downloaded this exact file, something went wrong
                throw "File could not be downloaded correctly";
            }
            downloadCount--;
            printlevel(LINFO, "\nDownloaded and added new hitje %s\n", hitjePath);

        }
    }
    fclose(fileTest);
    SAFE_DELETE_ARRAY(hitjePath);

    // Some valid data, copy all the data that was valid
    char hitIndexStr[4];
    sprintf(hitIndexStr, "%03d", *hitIndex);
    fileOutput += hitIndexStr;
    fileOutput += ';';
    fileOutput += title;
    fileOutput += ';';
    fileOutput += artist;
    fileOutput += "\n";
    // If the file existed already, we have a valid index
    return true;
}

char* AudioList::getVideoFile(int hitIndex, char *title, char *artist) {
    try {
        char query[strlen(title) + strlen(artist) + 1];
        sprintf(query, "%s %s", title, artist);

        printlevel(LBGINFO, "Searching for %s\n", query);
        char *baseName = api.searchVid(query, hitjesPath);
        printlevel(LBGINFO, "Downloaded:  %s\n", baseName);

        if (baseName) {
            char *videoName = createHitjeName(hitIndex, title, artist, false);

            printlevel(LBGINFO, "Done searching and found %s\n", videoName);
            // create path for saving audio data
            char *videoPath = getAbsolutePath(hitjesPath, hitjesPathLen, videoName);

#if defined (USE_YOUTUBE_DL)
            // audio format already correct mp3, just move the file
            printlevel(LBGINFO, "Moving '%s' to '%s'\n", baseName, videoPath);
            if (rename(baseName, videoPath)) {
                printlevel(LERROR, "Could not move file to correct location\n");
                printlevel(LERROR, "errno: %d\n", errno);
            }
            // and clean
            SAFE_DELETE_ARRAY(videoPath);
            SAFE_DELETE_ARRAY(baseName);
#else
            // use ffmpeg to get the audio file from video
            char decodeCmd[videoNameLen + strlen(videoPath) + 200];
            sprintf(decodeCmd, "ffmpeg -n -i \"%s\" -f mp3 -vn \"%s\"", baseName, videoPath);
            printlevel(LDEBUG, "calling ffmpeg with '%s'\n", decodeCmd);
            system(decodeCmd);

            FILE *musicFile = fopen(videoPath, "r");
            bool fileExists = musicFile != NULL;
            fclose(musicFile);
            SAFE_DELETE_ARRAY(videoPath);

            if (!fileExists) {
                SAFE_DELETE_ARRAY(baseName);
                throw "File conversion failed";
            }

            // video file is not needed anymore, SAFE_DELETE(it
            remove(baseName);
            printlevel(LBGINFO, "Basename: %s\n", baseName);
            SAFE_DELETE_ARRAY(baseName);
            printlevel(LBGINFO, "Removed old file\n");
#endif // USE_YOUTUBE_DL
            printlevel(LBGINFO, "\n\nSuccessfully got file '%s'\n", videoName);
            return videoName;
        } else {
            printlevel(LWARNING, "No suitable video file found\n");
        }
    } catch (const char* e) {
        // on any error, still continue
        printlevel(LERROR, "An error occured while parsing, press any key to continue\n%s\n", e);
        getchar();
    }
    return NULL;
}
