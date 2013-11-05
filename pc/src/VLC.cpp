#include "VLC.h"



const char* VLC::getDefaultPhoneOutput() {
    return phoneOutput;
}

const char* VLC::getDefaultSpeakerOutput() {
    return speakerOutput;
}

const char* VLC::getDefaultPhoneDevice() {
    return phoneDevice;
}

const char* VLC::getDefaultSpeakerDevice() {
    return speakerDevice;
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


char* VLC::phoneDevice = NULL;
char* VLC::speakerDevice = NULL;
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
    delete[] phoneDevice;
    delete[] speakerDevice;
}

void VLC::setPhoneDevice(const char* phoneDevice) {
    delete[] VLC::phoneDevice;
    VLC::phoneDevice = new char[strlen(phoneDevice)];
    strcpy(VLC::phoneDevice, phoneDevice);
}

void VLC::setSpeakerDevice(const char* speakerDevice) {
    delete[] VLC::speakerDevice;
    VLC::speakerDevice = new char[strlen(speakerDevice)];
    strcpy(VLC::speakerDevice, speakerDevice);
}



VLC::VLC() {
    libvlcInstance = libvlc_new(0, NULL);

    // get all attached audio devices
    libvlc_audio_output_t *audioOutputs, *curAudioOutput;
    audioOutputs = libvlc_audio_output_list_get(libvlcInstance);
    curAudioOutput = audioOutputs;

    phoneOutput = NULL;
    speakerOutput = NULL;
    while(curAudioOutput) {
        libvlc_audio_output_device_t *audioDevices, *curAudioDevice;
        audioDevices = libvlc_audio_output_device_list_get(libvlcInstance, curAudioOutput->psz_name);
        curAudioDevice = audioDevices;
        while(curAudioDevice) {
            printf("  -Device found:");
            if(phoneDevice && strcmp(curAudioDevice->psz_device, phoneDevice) == 0) {
                printf(" --- Phone device");
                int strLen = strlen(curAudioOutput->psz_name);
                phoneOutput = new char[strLen];
                strcpy(phoneOutput, curAudioOutput->psz_name);
            } else if (speakerDevice && strcmp(curAudioDevice->psz_device, speakerDevice) == 0) {
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

    if ((phoneDevice && (phoneOutput == NULL)) || (speakerDevice && (speakerOutput == NULL))) {
        throw "Not all output devices were found";
    }
}

VLC::~VLC() {
    libvlc_release(libvlcInstance);
    delete[] phoneOutput;
    delete[] speakerOutput;
}
