#include "VLC.h"



string VLC::getDefaultPhoneOutput() {
    return phoneOutput;
}

string VLC::getDefaultSpeakerOutput() {
    return speakerOutput;
}

string VLC::getDefaultPhoneDevice() {
    return phoneDevice;
}

string VLC::getDefaultSpeakerDevice() {
    return speakerDevice;
}

libvlc_media_t *VLC::newMediaFromPath(const char *path) {
    libvlc_media_t *mediaData = libvlc_media_new_path(libvlcInstance, path);
    libvlc_media_parse(mediaData);
    if (!libvlc_media_is_parsed(mediaData) || libvlc_media_get_duration(mediaData) == 0) {
        VLC::release(mediaData);
        return NULL;
    }
    return mediaData;
}

libvlc_media_list_t *VLC::newMediaList() {
    return libvlc_media_list_new(libvlcInstance);
}

libvlc_media_player_t *VLC::newMediaPlayer() {
    return libvlc_media_player_new(libvlcInstance);
}

libvlc_media_list_player_t *VLC::newMediaListPlayer() {
    return libvlc_media_list_player_new(libvlcInstance);
}



void VLC::retain(libvlc_media_t *media) {
    // Retain doesn't check for NULL
    if (media != NULL) {
        libvlc_media_retain(media);
    }
}

void VLC::retain(libvlc_media_list_t *mediaList) {
    // Retain doesn't check for NULL
    if (mediaList != NULL) {
        libvlc_media_list_retain(mediaList);
    }
}

void VLC::retain(libvlc_media_player_t *mediaPlayer) {
    // Retain doesn't check for NULL
    if (mediaPlayer != NULL) {
        libvlc_media_player_retain(mediaPlayer);
    }
}

void VLC::retain(libvlc_media_list_player_t *mediaListPlayer) {
    // Retain doesn't check for NULL
    if (mediaListPlayer != NULL) {
        libvlc_media_list_player_retain(mediaListPlayer);
    }
}


void VLC::release(libvlc_media_t *media) {
    // TODO make safe by setting to NULL
    // TODO does this fail with NULL?
    libvlc_media_release(media);
}

void VLC::release(libvlc_media_list_t *mediaList) {
    libvlc_media_list_release(mediaList);
}

void VLC::release(libvlc_media_player_t *mediaPlayer) {
    libvlc_media_player_release(mediaPlayer);
}

void VLC::release(libvlc_media_list_player_t *mediaListPlayer) {
    libvlc_media_list_player_release(mediaListPlayer);
}

GUI *VLC::gui = NULL;
Config *VLC::config = NULL;
VLC *VLC::instance = NULL;


VLC *VLC::getInstance() {
    if (instance == NULL) {
        // we only need one vlc instance
        if (gui == NULL) {
            throw "VLC needs a gui!";
        }
        instance = new VLC();
        if (!instance) {
            throw "Couldn't instantiate VLC";
        }
    }
    return instance;
}

void VLC::setGUI(GUI *gui) {
    VLC::gui = gui;
}

void VLC::setConfig(Config *config) {
    if (instance != NULL) {
        throw "Config should be set before using VLC";
    }
    VLC::config = config;
}

void VLC::deleteInstance() {
    SAFE_DELETE(instance);
}


vector<VLC::deviceoutput> VLC::getDeviceOutputs(vector<string> &outputNames) {
    // Get all attached audio devices
    libvlc_audio_output_t *audioOutputs, *curAudioOutput;
    audioOutputs = libvlc_audio_output_list_get(libvlcInstance);
    curAudioOutput = audioOutputs;
    vector<deviceoutput> outputs;
    while (curAudioOutput) {
        libvlc_audio_output_device_t *audioDevices, *curAudioDevice;
        audioDevices = libvlc_audio_output_device_list_get(libvlcInstance, curAudioOutput->psz_name);
        curAudioDevice = audioDevices;
        while (curAudioDevice) {
            outputNames.push_back(string(curAudioDevice->psz_device));
            outputs.push_back({string(curAudioOutput->psz_name), string(curAudioDevice->psz_device), string(curAudioDevice->psz_description)});
            curAudioDevice = curAudioDevice->p_next;
        }
        libvlc_audio_output_device_list_release(audioDevices);
        audioDevices = NULL;

        curAudioOutput = curAudioOutput->p_next;
    }
    libvlc_audio_output_list_release(audioOutputs);
    audioOutputs = NULL;
    return outputs;
}


void VLC::tryConfig(const vector<deviceoutput> &outputs) {
    // Reset all searches, restart whole set
    phoneDevice.clear();
    speakerDevice.clear();
    phoneOutput.clear();
    speakerOutput.clear();

    phoneDevice = config->getVLCPhoneDevice();
    speakerDevice = config->getVLCSpeakerDevice();

    gui->printlevel(LDEBUG, "\n");
    regex phoneRex(phoneDevice), speakerRex(speakerDevice);
    for (std::vector<deviceoutput>::const_iterator output = outputs.begin(); output != outputs.end(); ++output) {
        if (regex_match(output->device, phoneRex)) {
            gui->printlevel(LDEBUG, " --- Phone device\n");
            phoneOutput = output->output;
            phoneDevice = output->device;
        }
        if (regex_match(output->device, speakerRex)) {
            gui->printlevel(LDEBUG, " --- Speaker device\n");
            speakerOutput = output->output;
            speakerDevice = output->device;
        }
        gui->printlevel(LDEBUG, "  Device:      %s\n", output->device.c_str());
        gui->printlevel(LDEBUG, "  Description: %s\n\n", output->deviceDescription.c_str());
        gui->confirm(LDEBUG, "");
    }
}

VLC::VLC() {
    libvlcInstance = libvlc_new(0, NULL);

    vector<string> outputNames;
    vector<deviceoutput> outputs = getDeviceOutputs(outputNames);

    tryConfig(outputs);
    while (phoneDevice.empty() || speakerDevice.empty() || phoneOutput.empty() || speakerOutput.empty()) {
        if (!config->nextAudioDevices()) {
            break;
        }
        gui->printlevel(LDEBUG, "\nNo output combination found, retrying...\n");
        tryConfig(outputs);
    }
    if (phoneDevice.empty() || phoneOutput.empty()) {
        gui->printlevel(LINFO, "\n");
        gui->printlevel(LWARNING, "Phone device not found, which to use?\n");
        int i = gui->selection(outputNames);
        phoneOutput = outputs[i].output;
        phoneDevice = outputs[i].device;
        gui->printlevel(LINFO, "\n");
    }
    if (speakerDevice.empty() || speakerOutput.empty()) {
        gui->printlevel(LINFO, "\n");
        gui->printlevel(LWARNING, "Speaker device not found, which to use?\n");
        int i = gui->selection(outputNames);
        speakerOutput = outputs[i].output;
        speakerDevice = outputs[i].device;
        gui->printlevel(LINFO, "\n");
    }
}

VLC::~VLC() {
    libvlc_release(libvlcInstance);
}
