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
        if (!instance) {
            throw "Couldn't instantiate";
        }
    }
    return instance;
}

void VLC::deleteInstance() {
    if (instance != NULL) {
        SAFE_DELETE(instance);
    }
    SAFE_DELETE_ARRAY(phoneDevice);
    SAFE_DELETE_ARRAY(speakerDevice);
}

void VLC::setPhoneDevice(const char* phoneDevice) {
    SAFE_DELETE_ARRAY(VLC::phoneDevice);
    VLC::phoneDevice = new char[strlen(phoneDevice)];
    strcpy(VLC::phoneDevice, phoneDevice);
}

void VLC::setSpeakerDevice(const char* speakerDevice) {
    SAFE_DELETE_ARRAY(VLC::speakerDevice);
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
    vector<string> outputs, devices;
    devices.push_back(string("(default)"));
    while(curAudioOutput) {
        libvlc_audio_output_device_t *audioDevices, *curAudioDevice;
        audioDevices = libvlc_audio_output_device_list_get(libvlcInstance, curAudioOutput->psz_name);
        curAudioDevice = audioDevices;
        while(curAudioDevice) {
            outputs.push_back(string(curAudioOutput->psz_name));
            devices.push_back(string(curAudioDevice->psz_device));
            printf("-Device found:");
            if (phoneDevice && strcmp(curAudioDevice->psz_device, phoneDevice) == 0) {
                printf(" --- Phone device");
                phoneOutput = new char[strlen(curAudioOutput->psz_name)];
                strcpy(phoneOutput, curAudioOutput->psz_name);
            }
            if (speakerDevice && strcmp(curAudioDevice->psz_device, speakerDevice) == 0) {
                printf(" --- Speaker device");
                speakerOutput = new char[strlen(curAudioOutput->psz_name)];
                strcpy(speakerOutput, curAudioOutput->psz_name);
            }
            printf("\n");
            printf("  Device:      %s\n", curAudioDevice->psz_device);
            printf("  Description: %s\n", curAudioDevice->psz_description);
            curAudioDevice = curAudioDevice->p_next;
        }
        libvlc_audio_output_device_list_release(audioDevices);

        curAudioOutput = curAudioOutput->p_next;
    }

    if (!phoneDevice || (phoneOutput == NULL)) {
        printf("\nPhone device %s not found, which to use?\n", phoneDevice);
        int i = selection(devices);
        if (i != 0) {
            phoneOutput = new char[outputs[i - 1].size()];
            strcpy(phoneOutput, outputs[i - 1].data());
            phoneDevice = new char[devices[i].size()];
            strcpy(phoneDevice, devices[i].data());
        }
        printf("\n");
    }
    if (!speakerDevice || (speakerOutput == NULL)) {
        printf("\nSpeaker device %s not found, which to use?\n", speakerDevice);
        int i = selection(devices);
        if (i != 0) {
            speakerOutput = new char[outputs[i].size()];
            strcpy(speakerOutput, outputs[i].data());
            speakerDevice = new char[devices[i].size()];
            strcpy(speakerDevice, devices[i].data());
        }
        printf("\n");
    }
    libvlc_audio_output_list_release(audioOutputs);
}

VLC::~VLC() {
    libvlc_release(libvlcInstance);
    SAFE_DELETE_ARRAY(phoneOutput);
    SAFE_DELETE_ARRAY(speakerOutput);
}
