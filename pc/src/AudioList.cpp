#include "AudioList.h"

int AudioList::checkMediaFile(libvlc_media_t* mediaFile) {
    libvlc_media_parse(mediaFile);
    if (!libvlc_media_is_parsed(mediaFile)) {
        return 0;
    }
    return libvlc_media_get_duration(mediaFile);
}

AudioList::AudioList(const char* listFilePath) {
    FILE* listFile;
    int hitIndex = 0;
    char buffer[1000];
    char title[200];
    char artist[200];
    char path[200];

    buffer[0] = title[0] = artist[0] = path[0] = '\0';

    char *titleTrim = 0, *artistTrim = 0, *absPath = 0;
    listFile = fopen(listFilePath, "r");
    if (listFile == NULL) {
        printf("File read error %d", errno);
        throw "File not readable";
    }

    string fileOutput;

    // save data about hitjeslist
    this->listFilePath = new char[strlen(listFilePath) + 1];
    sprintf(this->listFilePath, "%s", listFilePath);
    // get the last backslash from hitjeslist for making relative paths
    pathLen = strrchr(listFilePath, '\\') - listFilePath + 1;

    printf("\nCreating the hitjeslist\n\n");
    // Create a new list with 999 hitjes
    hitjesList = new libvlc_media_t*[999];
    memset(hitjesList, 0, 999 * sizeof(hitjesList));

    // loop through all lines without hitjes
    do {
        if (buffer[0]) {
            printf("skipping %s", buffer);
        }
        if (feof(listFile)) {
            // reached end of file without any hitjes
            throw "No valid hitjes file";
        }
        fgets(buffer, 999, listFile);
    } while(parseBuf(buffer, &hitIndex, title, artist, path, fileOutput));
    printf("\nSkipped all non hitjes lines\n\n");

    VLC* VLCInstance = VLC::getInstance();

    do {
        if (downloadYoutube > 0) {
            printf("Found new line:\n%s\ncontinue parsing?", buffer);
            if (readKeyboard() != INPUT_NONE) {
                downloadYoutube = false;
            }
        }

        if (hitIndex <= 0 || hitIndex >= 999) {
            printf("Wrong hitindex, skipping %d: '%s' '%s' '%s'\n", hitIndex, title, artist, path);
        } else if (hitjesList[hitIndex]) {
            printf("Hitindex %d is defined multiple times\n", hitIndex);
        } else {
            // and use all data to create a new element in list
            titleTrim = trim(title);
            artistTrim = trim(artist);
            absPath = getAbsolutePath(listFilePath, pathLen, trim(path));
            printf("1 '%s'\n", absPath);
            libvlc_media_t* mediaFile = VLCInstance->newMediaFromPath(absPath);
            printf("2\n");
            if (!checkMediaFile(mediaFile)) {
                printf("Invalid file, '%s'\n", absPath);
            } else {
                printf("Added hitje to the list, %d: '%s' %s' '%s'\n", hitIndex, titleTrim, artistTrim, absPath);
                hitjesList[hitIndex] = mediaFile;
            }
            SAFE_DELETE_ARRAY(absPath);
        }
        do {
            fgets(buffer, 999, listFile);
            if (!parseBuf(buffer, &hitIndex, title, artist, path, fileOutput)) {
                hitIndex = -1;
            }
        } while (!feof(listFile) && hitIndex == -1);
    } while (!feof(listFile));
    fclose(listFile);

    listFile = fopen(listFilePath, "w");
    fputs(fileOutput.data(), listFile);
    fclose(listFile);
}

AudioList::~AudioList() {
}


libvlc_media_t* AudioList::getAudio(int audioIndex) {
    if (audioIndex < 0 || audioIndex > 999) {
        return 0;
    }
    return hitjesList[audioIndex];
}

bool AudioList::parseBuf(char* buffer, int *hitIndex, char *title, char *artist, char *path, string &fileOutput) {
    //int res = sscanf(buffer, "%d;%[^;];%[^;];%[^;]", hitIndex, title, artist, path);
        int res = sscanf(buffer, "%d;%[^;];%[^;];%[^;]", hitIndex, title, artist, path);

    if (!hitIndex || !*hitIndex) {
        // couldn't read line, reset
        res = -1;
    }
    char *pathTrim = trim(path);
    char *artistTrim = trim(artist);
    char *titleTrim = trim(title);
    if (res == 4 && !strlen(pathTrim)) {
        res--;
    }
    if (res == 3 && !strlen(artistTrim)) {
        res--;
    }
    if (res == 2 && !strlen(titleTrim)) {
        res--;
    }
    if (downloadYoutube && res == 3) {
        printf("\n\nTrying to download youtube\n");
        // no file found, title and artist are given, download from internet
        char *newpath = getVideoFile(*hitIndex, titleTrim, artistTrim);
        if (newpath) {
            printf("Parsing file at path %s\n", newpath);
            // downloaded video, save the new path
            sprintf(path, "%s", newpath);
            SAFE_DELETE_ARRAY(newpath);
            res = 4;

            char hitIndexStr[10];
            sprintf(hitIndexStr, "%03d", *hitIndex);
            fileOutput = fileOutput + hitIndexStr + ';' + titleTrim + ';' + artistTrim + ';' + pathTrim + '\n';
        } else {
            fileOutput += buffer;
            return false;
        }
    } else {
        fileOutput += buffer;
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
            char *videoPath = getAbsolutePath(listFilePath, pathLen, videoName);
            // set extension to mp3
            char *extension = strrchr(videoPath, '.');
            sprintf(extension, ".mp3");

            // use ffmpeg to get the audio file from video
            char decodeCmd[baseNameLen + strlen(videoPath) + 29 + 1 + 10];
            sprintf(decodeCmd, "ffmpeg -n -i \"%s\" -f mp3 -vn \"%s\"", baseName, videoPath);
            printf("calling ffmpeg with '%s'\n", decodeCmd);
            system(decodeCmd);
            SAFE_DELETE_ARRAY(videoPath);

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
