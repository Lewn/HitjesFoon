#include "AudioMenuItem.h"

int checkMediaFile(libvlc_media_t *mediaFile) {
    libvlc_media_parse(mediaFile);
    if (!libvlc_media_is_parsed(mediaFile)) {
        return 0;
    }
    return libvlc_media_get_duration(mediaFile);
}

AudioMenuItem::AudioMenuItem(string path, AudioMenuItem **followup, unsigned char followupLen, bool tts): followup(followup), followupLen(followupLen) {
    if (tts) {
        media = NULL;
        text = NULL;
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
        text = new char[fileSize + 5];
        text[0] = 's';
        text[1] = 'a';
        text[2] = 'y';
        text[3] = ' ';

        // copy the file into the buffer:
        result = fread(text + 4, 1, fileSize, pFile);
        if (result != fileSize) {
            throw "Reading error";
        }
        text[fileSize + 4] = 0;
        fclose(pFile);
    } else {
        text = NULL;
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
    } else {
        //SAFE_DELETE_ARRAY(text);
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
        printf("\n%s", text);
        system(text);
        return NULL;
    }
    return media;
}

