#include "AudioList.h"

char* trimLeft(char* toTrim) {
    while (!isgraph(*toTrim)) {
        toTrim++;
    }
    return toTrim;
}

char* trimRight(char* toTrim) {
    char* tmp = toTrim;
    toTrim += strlen(toTrim);
    while (!isgraph(*(--toTrim))) {
        *toTrim = 0;
    }
    return tmp;
}

char* trim(char* toTrim) {
    return trimLeft(trimRight(toTrim));
}

int AudioList::checkMediaFile(libvlc_media_t* mediaFile) {
    libvlc_media_parse(mediaFile);
    if (!libvlc_media_is_parsed(mediaFile)) {
        return 0;
    }
    libvlc_media_track_t*** tracks = new libvlc_media_track_t**[5];
    return libvlc_media_tracks_get(mediaFile, tracks);
}

char* AudioList::getAbsolutePath(const char* listFilePath, int pathLen, const char* filename) {
    // we need the path and the filename in one string
    int totalLen = pathLen + strlen(filename);
    char* absolute = new char[totalLen];
    // copy path in the new filename
    strncpy(absolute, listFilePath, pathLen);
    // and copy the name itself
    strcpy(absolute + pathLen, filename);
    return absolute;
}

AudioList::AudioList(const char* listFilePath) {
    FILE* listFile;
    int hitIndex;
    char* buffer = new char[1000];
    char* title = new char[200];
    char* artist = new char[200];
    char* path = new char[200];
    listFile = fopen(listFilePath, "r");
    if (listFile == NULL) {
        throw "File not readable";
    }

    buffer[0] = 0;

    printf("\nCreating the hitjeslist\n\n");
    // Create a new list with 999 hitjes
    hitjesList = new libvlc_media_t*[999];
    memset(hitjesList, 0, 999 * sizeof(hitjesList));

    // get the last backslash from hitjeslist for making relative paths
    int pathLen = strrchr(listFilePath, '\\') - listFilePath + 1;

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
    } while(sscanf(buffer, "%d %[^&]&%[^&]&%[^&]", &hitIndex, title, artist, path) != 4);
    printf("\nSkipped all non hitjes lines\n\n");

    VLC* VLCInstance = VLC::getInstance();

    do {
        //printf("Read line %s", buffer);
        if (hitIndex <= 0 || hitIndex > 999) {
            printf("Wrong hitindex, skipping %d: '%s' '%s' '%s'\n", hitIndex, title, artist, path);
        } else if (hitjesList[hitIndex]) {
            printf("Hitindex %d is defined multiple times\n", hitIndex);
        } else {
            // and use all data to create a new element in list
            title = trim(title);
            artist = trim(artist);
            path = getAbsolutePath(listFilePath, pathLen, trim(path));
            libvlc_media_t* mediaFile = VLCInstance->newMediaFromPath(path);
            if (!checkMediaFile(mediaFile)) {
                printf("Invalid file, '%s'\n", path);
            } else {
                printf("Added hitje to the list, %d: '%s' %s' '%s'\n", hitIndex, title, artist, path);
                hitjesList[hitIndex] = mediaFile;
            }
        }
        do {
            fgets(buffer, 999, listFile);
            if (sscanf(buffer, "%d %[^&]&%[^&]&%[^&]", &hitIndex, title, artist, path) != 4) {
                //printf("Wrong line, %s", buffer);
                hitIndex = -1;
            }
        } while (!feof(listFile) && hitIndex == -1);
    } while (!feof(listFile));
}

AudioList::~AudioList() {
}


libvlc_media_t* AudioList::getAudio(int audioIndex) {
    if (audioIndex < 0 || audioIndex > 999) {
        return 0;
    }
    return hitjesList[audioIndex];
}
