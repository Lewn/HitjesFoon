#include "VLC.h"



const char* VLC::getDefaultPhoneOutput() {
    return phoneOutput;
}

const char* VLC::getDefaultSpeakerOutput() {
    return speakerOutput;
}

libvlc_media_t* VLC::newMediaFromPath(const char* path) {
    return libvlc_media_new_path(libvlcInstance, path);
}

libvlc_media_list_t* VLC::newMediaList() {
    return libvlc_media_list_new(libvlcInstance);
}

libvlc_media_player_t* VLC::newMediaPlayer() {
    return libvlc_media_player_new(libvlcInstance);
}

libvlc_media_list_player_t* VLC::newMediaListPlayer() {
    return libvlc_media_list_player_new(libvlcInstance);
}


VLC* VLC::instance = NULL;


VLC* VLC::getInstance() {
    if (instance == NULL) {
        // we only need one vlc instance
        instance = new VLC();
    }
    return instance;
}

void VLC::deleteInstance() {
    if (instance != NULL) {
        delete instance;
    }
}



VLC::VLC() {
    libvlcInstance = libvlc_new(0, NULL);

    // get all attached audio devices
    libvlc_audio_output_t *audioOutputs, *curAudioOutput;
    audioOutputs = libvlc_audio_output_list_get(libvlcInstance);
    curAudioOutput = audioOutputs;

    while(curAudioOutput) {
        libvlc_audio_output_device_t *audioDevices, *curAudioDevice;
        audioDevices = libvlc_audio_output_device_list_get(libvlcInstance, curAudioOutput->psz_name);
        curAudioDevice = audioDevices;
        while(curAudioDevice) {
            printf("  -Device found:");
            if(strcmp(curAudioDevice->psz_device, PHONE_DEVICE) == 0) {
                printf(" --- Phone device");
                int strLen = strlen(curAudioOutput->psz_name);
                phoneOutput = new char[strLen];
                strcpy(phoneOutput, curAudioOutput->psz_name);
            } else if (strcmp(curAudioDevice->psz_device, SPEAKER_DEVICE) == 0) {
                printf(" --- Speaker device");
                int strLen = strlen(curAudioOutput->psz_name);
                speakerOutput = new char[strLen];
                strcpy(speakerOutput, curAudioOutput->psz_name);
            }
            printf("\n");
            printf("    Device:      %s\n", curAudioDevice->psz_device);
            printf("    Description: %s\n", curAudioDevice->psz_description);
            curAudioDevice = curAudioDevice->p_next;
        }
        libvlc_audio_output_device_list_release(audioDevices);

        curAudioOutput = curAudioOutput->p_next;
    }
    libvlc_audio_output_list_release(audioOutputs);

    if (phoneOutput == NULL || speakerOutput == NULL) {
        throw "Not all output devices were found";
    }
}

VLC::~VLC() {
    libvlc_release(libvlcInstance);
    delete[] phoneOutput;
    delete[] speakerOutput;
}
