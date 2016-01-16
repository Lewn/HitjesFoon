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
        AudioMenuItem *nextItem = curItem->getFollowup(input - 1);
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

libvlc_media_t *AudioMenu::getMedia() {
    return curItem->getMedia();
}

bool AudioMenu::isEnded() {
    return !curItem || !curItem->hasNext();
}

void AudioMenu::fromPath(string path) {
    initialItem = createItem(path);
}

AudioMenuItem *AudioMenu::createItem(string path) {
    // try to open the specified dir
    DIR *dir = opendir(path.c_str());
    if (!dir) {
        printlevel(LERROR, "\nInvalid: '%s'\n", path.c_str());
        throw "Path should point to an existing directory";
    }

    struct dirent *entry;
    struct stat st;
    string textName, audioName;
    int dirCount = 0;
    while ((entry = readdir(dir))) {
        if (stat(entry->d_name, &st) == -1) {
            printlevel(LWARNING, "Invalid file '%s' in dir '%s'\n", entry->d_name, dir);
            continue;
        }
        if (S_ISREG(st.st_mode)) {
            // regular file
            if (!strncmp("text", entry->d_name, 4)) {
                if (textName.empty()) {
                    textName = entry->d_name;
                } else {
                    throw "Excess text files found";
                }
            } else if (!strncmp("audio", entry->d_name, 5)) {
                if (audioName.empty()) {
                    audioName = entry->d_name;
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
    if (audioName.empty() && textName.empty()) {
        throw "Either an audio or text file should be specified";
    }

    AudioMenuItem **followup = new AudioMenuItem*[dirCount];
    for (int i = 1; i <= dirCount; i++) {
        followup[i - 1] = createItem(path + std::to_string(i));
    }

    closedir(dir);
    AudioMenuItem *item;
    if (!audioName.empty()) {
        item = new AudioMenuItem(path + audioName, followup, dirCount, false);
    } else {
        item = new AudioMenuItem(path + textName, followup, dirCount, true);
    }
    return item;
}
