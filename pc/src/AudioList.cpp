#include "AudioList.h"

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
        printf("File read error %d", errno);
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

    printf("\nCreating the hitjeslist\n\n");
    hitjesList = 0;
    update(0);
}

AudioList::~AudioList() {
}

libvlc_media_t* AudioList::getAudio(int audioIndex) {
    if (audioIndex < 0 || audioIndex > 999) {
        return 0;
    }
    return hitjesList[audioIndex];
}


bool AudioList::update(unsigned int downloadCount) {
    this->downloadCount = downloadCount;
    int hitIndex = 0;
    char buffer[1000];
    char title[200];
    char artist[200];
    char path[200];
    char *absPath = 0;

    buffer[0] = title[0] = artist[0] = path[0] = '\0';

    // Create a new list with 999 hitjes
    libvlc_media_t **newHitjesList = new libvlc_media_t*[999];
    memset(newHitjesList, 0, 999 * sizeof(newHitjesList));

    if (listFile == NULL) {
        listFile = fopen(listFilePath, "r");
    }
    string fileOutput;

    VLC* VLCInstance = VLC::getInstance();

    skipInvalidLines(buffer, &hitIndex, title, artist, path, fileOutput);
    do {
        if (this->downloadCount && (readKeyboard() != INPUT_NONE)) {
            this->downloadCount = 0;
        }

        if (hitIndex <= 0 || hitIndex >= 999) {
            printf("Wrong hitindex, skipping %d: '%s' '%s' '%s'\n", hitIndex, title, artist, path);
        } else if (newHitjesList[hitIndex]) {
            printf("Hitindex %d is defined multiple times\n", hitIndex);
        } else {
            // and use all data to create a new element in list
            absPath = getAbsolutePath(hitjesPath, hitjesPathLen, path);
            libvlc_media_t* mediaFile = VLCInstance->newMediaFromPath(absPath);
            if (!checkMediaFile(mediaFile)) {
                printf("Invalid file, '%s'\n", absPath);
            } else {
                newHitjesList[hitIndex] = mediaFile;
            }
            SAFE_DELETE_ARRAY(absPath);
        }
        do {
            if (fgets(buffer, 999, listFile) && !parseBuf(buffer, &hitIndex, title, artist, path, fileOutput)) {
                hitIndex = -1;
            }
        } while (!feof(listFile) && hitIndex == -1);
    } while (!feof(listFile));
    fclose(listFile);

    // TODO: delete all files not in list

    listFile = fopen(listFilePath, "w");
    fputs(fileOutput.data(), listFile);
    fclose(listFile);
    listFile = NULL;

    // clear old hitjeslist
    if (hitjesList) {
        for (int i = 0; i < sizeof(hitjesList); i++) {
            libvlc_media_release(hitjesList[i]);
        }
        delete[] hitjesList;
    }
    hitjesList = newHitjesList;
}

void AudioList::skipInvalidLines(char* buffer, int *hitIndex, char *title, char *artist, char *path, string &fileOutput) {
    // loop through all lines without hitjes
    do {
        if (feof(listFile)) {
            // reached end of file without any hitjes
            throw "No valid hitjes file";
        }
        fgets(buffer, 999, listFile);
    } while(parseBuf(buffer, hitIndex, title, artist, path, fileOutput));
    printf("\nSkipped all non hitjes lines\n\n");
}

bool AudioList::parseBuf(char* buffer, int *hitIndex, char *title, char *artist, char *path, string &fileOutput) {
    int res = sscanf(buffer, "%d;%[^;];%[^;];%[^;]", hitIndex, title, artist, path);

    if (!hitIndex || !*hitIndex) {
        // couldn't read line, reset
        res = -1;
    }
    if (res == 4) {
        sprintf(path, "%s", trim(path));
        if (!strlen(path)) {
            // actually emtpy
            res--;
        } else {
            char *absPath = getAbsolutePath(hitjesPath, hitjesPathLen, path);
            FILE *fileTest = fopen(absPath, "r");
            if (fileTest == 0) {
                res--;
            }
            fclose(fileTest);
            SAFE_DELETE_ARRAY(absPath);
        }
    }
    if (res >= 3) {
        sprintf(artist, "%s", trim(artist));
        if (!strlen(artist)) {
            res = 2;
        }
    }
    if (res >= 2) {
        sprintf(title, "%s", trim(title));
        if (!strlen(title)) {
            res = 1;
        }
    }
    if (downloadCount && res == 3) {
        printf("\n\nTrying to download youtube\n");
        // no file found, title and artist are given, download from internet
        char *newpath = getVideoFile(*hitIndex, title, artist);
        if (newpath) {
            printf("Parsing file at path %s\n", newpath);
            // downloaded video, save the new path
            sprintf(path, "%s", newpath);
            SAFE_DELETE_ARRAY(newpath);
            res = 4;
            downloadCount--;
            printf("\nDownloaded and added new hitje %s\n", path);
        }
    }
    string buf = "\n";
    switch (res) {
        case 4:
            buf = path + buf;
            buf = ';' + buf;
        case 3:
            buf = artist + buf;
            buf = ';' + buf;
        case 2:
            buf = title + buf;
            buf = ';' + buf;
        case 1:
            char hitIndexStr[4];
            sprintf(hitIndexStr, "%03d", *hitIndex);
            buf = hitIndexStr + buf;
            break;
        default:
            buf = buffer;
    }
    if (buf.size() > 1) {
        fileOutput += buf;
    }
    return res == 4;
}

char* AudioList::getVideoFile(int hitIndex, char *title, char *artist) {
    try {
        char query[strlen(title) + strlen(artist) + 1];
        sprintf(query, "%s %s", title, artist);

        printf("Searching for %s\n", query);
        char *baseName = api.searchVid(query);

        if (baseName) {
            int baseNameLen = strlen(baseName);
            char *videoName = new char[baseNameLen + 6 + 1];
            sprintf(videoName, "%03d - %s", hitIndex, baseName);

            printf("Done searching and found %s\n", videoName);
            // create path for saving audio data
            char *videoPath = getAbsolutePath(hitjesPath, hitjesPathLen, videoName);
            // set extension to mp3
            char *extension = strrchr(videoPath, '.');
            sprintf(extension, ".mp3");

            // use ffmpeg to get the audio file from video
            char decodeCmd[baseNameLen + strlen(videoPath) + 29 + 1 + 10];
            sprintf(decodeCmd, "ffmpeg -n -i \"%s\" -f mp3 -vn \"%s\"", baseName, videoPath);
            printf("calling ffmpeg with '%s'\n", decodeCmd);
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
            printf("Removed old file\n");

            // set extension to zero
            extension = strrchr(videoName, '.');
            if (extension) {
                // add .mp3 extension to filename
                sprintf(extension, ".mp3");
            } else {
                printf("Filename: %s\n", videoName);
                throw "Couldn't change extension";
            }
            printf("Basename: %s\n", baseName);
            SAFE_DELETE_ARRAY(baseName);
            return videoName;
        } else {
            printf("No suitable video file found\n");
        }
    } catch (const char* e) {
        // on any error, still continue
        printf("An error occured while parsing, press any key to continue\n%s\n", e);
        getchar();
    }
    return NULL;
}
