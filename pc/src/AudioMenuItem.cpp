#include "AudioMenuItem.h"

int checkMediaFile(libvlc_media_t *mediaFile) {
    libvlc_media_parse(mediaFile);
    if (!libvlc_media_is_parsed(mediaFile)) {
        return 0;
    }
    return libvlc_media_get_duration(mediaFile);
}

AudioMenuItem::AudioMenuItem(GUI *gui, string path, AudioMenuItem **followup, unsigned char followupLen, bool tts) : gui(gui), followup(followup), followupLen(followupLen) {
    if (tts) {
        media = NULL;
        FILE *pFile;
        unsigned int fileSize;
        size_t result;

        pFile = fopen(path.c_str(), "rb");
        if (pFile == NULL) {
            throw "Invalid file specified";
        }

        // obtain file size:
        fseek(pFile, 0, SEEK_END);
        fileSize = ftell(pFile);
        rewind(pFile);

        // allocate memory to contain the whole file:
        char buf[fileSize];

        // copy the file into the buffer:
        result = fread(buf, 1, fileSize, pFile);
        if (result != fileSize) {
            throw "Reading error";
        }
        text = "say ";
        text += buf;
        SAFE_CLOSE(pFile);
    } else {
        text.clear();
        media = VLC::getInstance()->newMediaFromPath(path.c_str());
        if (!checkMediaFile(media)) {
            throw "Invalid media file";
        }
    }
}

AudioMenuItem::~AudioMenuItem() {
    for (unsigned char i = 0; i < followupLen; i++) {
        SAFE_DELETE(followup[i]);
    }
    SAFE_DELETE_ARRAY(followup);

    if (media) {
        VLC::release(media);
    }
}

bool AudioMenuItem::hasNext() {
    return followupLen;
}

AudioMenuItem *AudioMenuItem::getFollowup(unsigned char index) {
    if (index < followupLen) {
        return followup[index];
    }
    return NULL;
}

libvlc_media_t *AudioMenuItem::getMedia() {
    if (!media) {
        gui->printlevel(LDEBUG, "\n%s", text.c_str());
        system(text.c_str());
        return NULL;
    }
    return media;
}

