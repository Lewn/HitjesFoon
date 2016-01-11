#include "AudioMenu.h"
#include "InputProcessor.h"


AudioMenu::AudioMenu() {
    initialItem = curItem = NULL;
}

AudioMenu::~AudioMenu() {
    SAFE_DELETE(initialItem);
}

bool AudioMenu::process(int input) {
    if (!curItem || input == 0) {
        // reset when nothing is set or input is 0
        curItem = initialItem;
        return true;
    }
    if (input > 0 && input <= 9) {
        // the only valid menu input
        AudioMenuItem* nextItem = curItem->getFollowup(input - 1);
        if (nextItem) {
            // get next audio menu item
            printlevel(LDEBUG, "\nGoing into item %d ", input);
            curItem = nextItem;
            return true;
        } else {
            printlevel(LDEBUG, "\nInvalid input %d ", input);
        }
    } else if (input == INPUT_EARTH_DOWN || input == INPUT_EARTH_SWAP) {
        // replay on earth button
        return true;
    }
    return false;
}

libvlc_media_t* AudioMenu::getMedia() {
    return curItem->getMedia();
}

bool AudioMenu::isEnded() {
    return !curItem || !curItem->hasNext();
}

void AudioMenu::fromPath(const char* path) {
    initialItem = createItem(path);
}

AudioMenuItem* AudioMenu::createItem(const char* path) {
    // try to open the specified dir
    DIR* dir = opendir(path);
    if (!dir) {
        printf("\nInvalid: '%s'\n", path);
        throw "Path should point to an existing directory";
    }

    struct dirent *entry;
    struct stat st;
    char *textName = NULL;
    char *audioName = NULL;
    int dirCount = 0;
    while ((entry = readdir(dir))) {
        if (stat(entry->d_name, &st) == -1) {
            printlevel(LWARNING, "Invalid file '%s' in dir '%s'\n", entry->d_name, dir);
            continue;
        }
        if (S_ISREG(st.st_mode)) {
            // regular file
            if (!strncmp("text", entry->d_name, 4)) {
                if (textName == NULL) {
                    textName = new char[strlen(entry->d_name) + 1];
                    strcpy(textName, entry->d_name);
                } else {
                    throw "Excess text files found";
                }
            } else if (!strncmp("audio", entry->d_name, 5)) {
                if (audioName == NULL) {
                    audioName = new char[strlen(entry->d_name) + 1];
                    strcpy(audioName, entry->d_name);
                } else {
                    throw "Excess audio files found";
                }
            }
        } else if (S_ISDIR(st.st_mode)) {
            // directory
            int dirNum;
            if (sscanf(entry->d_name, "%d", &dirNum) == 1) {
                if (++dirCount != dirNum) {
                    throw "Excess directories found";
                }
            }
        } else {
            printlevel(LWARNING, "\nUnknown entry '%s' with type %d", entry->d_name, st.st_mode);
        }
    }
    if (audioName == NULL && textName == NULL) {
        throw "Either an audio or text file should be specified";
    }

    int pathLen = strlen(path);
    AudioMenuItem** followup = new AudioMenuItem*[dirCount];
    char* dirName = new char[10];
    for (int i = 1; i <= dirCount; i++) {
        sprintf(dirName, "%d\\", i);
        char* absPath = getAbsolutePath(path, pathLen, dirName);
        followup[i - 1] = createItem(absPath);
        SAFE_DELETE_ARRAY(absPath);
    }
    SAFE_DELETE_ARRAY(dirName);

    closedir(dir);
    AudioMenuItem* item;
    if (audioName) {
        char* absPath = getAbsolutePath(path, pathLen, audioName);
        item = new AudioMenuItem(absPath, followup, dirCount, false);
        SAFE_DELETE_ARRAY(absPath);
    } else {
        char* absPath = getAbsolutePath(path, pathLen, textName);
        item = new AudioMenuItem(absPath, followup, dirCount, true);
        SAFE_DELETE_ARRAY(absPath);
    }
    SAFE_DELETE_ARRAY(audioName);
    SAFE_DELETE_ARRAY(textName);
    return item;
}
