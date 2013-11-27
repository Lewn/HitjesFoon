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
    char* buffer = new char[1000];
    char* title = new char[200];
    char* artist = new char[200];
    char* path = new char[200];

    do {
        toDownload = DEFAULT_DOWNLOAD;
        buffer[0] = title[0] = artist[0] = path[0] = '\0';

        char* titleTrim, *artistTrim, *pathTrim;
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
            if (toDownload > 0) {
                printf("Found new line:\n%s\ncontinue parsing?", buffer);
                if (readKeyboard() != INPUT_NONE) {
                    toDownload = -1;
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
                pathTrim = getAbsolutePath(listFilePath, pathLen, trim(path));
                libvlc_media_t* mediaFile = VLCInstance->newMediaFromPath(pathTrim);
                if (!checkMediaFile(mediaFile)) {
                    printf("Invalid file, '%s'\n", pathTrim);
                } else {
                    printf("Added hitje to the list, %d: '%s' %s' '%s'\n", hitIndex, titleTrim, artistTrim, pathTrim);
                    hitjesList[hitIndex] = mediaFile;
                }
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

    } while (toDownload == 0);

    SAFE_DELETE_ARRAY(buffer);
    SAFE_DELETE_ARRAY(title);
    SAFE_DELETE_ARRAY(artist);
    SAFE_DELETE_ARRAY(path);
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
    int res = sscanf(buffer, "%d %[^\t]\t%[^\t]\t%[^\t]", hitIndex, title, artist, path);
    if (!hitIndex || !*hitIndex) {
        // couldn't read line, reset
        res = -1;
    }
    if (toDownload > 0 && res == 3) {
        printf("\n\nTrying to download youtube\n");
        // no file found, title and artist are given, download from internet
        char* newpath = getVideoFile(*hitIndex, title, artist);
        if (newpath) {
            printf("Parsing file at path %s\n", newpath);
            // downloaded video, save the new path
            sprintf(path, "%s", newpath);
            SAFE_DELETE_ARRAY(newpath);
            res = 4;

            char hitIndexStr[10];
            sprintf(hitIndexStr, "%03d", *hitIndex);
            fileOutput += hitIndexStr;
            fileOutput += '\t';
            fileOutput += trim(title);
            fileOutput += '\t';
            fileOutput += trim(artist);
            fileOutput += '\t';
            fileOutput += trim(path);
            fileOutput += '\n';

            toDownload--;
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
        YoutubeAPI api;
        char *baseName = api.searchVid(query);

        if (baseName) {
            char *videoName = new char[strlen(baseName) + 6 + 1];
            sprintf(videoName, "%03d - %s", hitIndex, baseName);

            printf("Done searching and found %s\n", videoName);
            // create path for saving audio data
            char *videoPath = getAbsolutePath(listFilePath, pathLen, videoName);
            // set extension to mp3
            char* extension = strrchr(videoPath, '.');
            sprintf(extension, ".mp3");

            // use ffmpeg to get the audio file from video
            char* decodeCmd = new char[200];

            sprintf(decodeCmd, "ffmpeg -n -i \"%s\" -f mp3 -vn \"%s\"\n", baseName, videoPath);
            system(decodeCmd);
            SAFE_DELETE_ARRAY(decodeCmd);
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
                printf("Couldn't change extension for file %s\n", videoName);
            }
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
