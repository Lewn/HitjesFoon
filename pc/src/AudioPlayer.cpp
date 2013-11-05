#include "AudioPlayer.h"



// create event manager
//libvlc_event_manager_t* eventManager = libvcl_media_player_event_manager(audioPlayer);
// listen for media end event
//libvlc_event_attach(eventManager, libvlc_event_e::libvlc_MediaDiscovererEnded);


AudioPlayer::AudioPlayer(AudioDevice device, AudioList* audioList) {
    this->audioList = audioList;
    VLC* VLCInstance = VLC::getInstance();
    //Load the VLC engine
    audioPlayer = VLCInstance->newMediaPlayer();

    // start device at phone
    curOutput = "";
    setAudioDevice(device);
    audioIndex = 0;
}

AudioPlayer::~AudioPlayer() {

}


const char* AudioPlayer::audioOutputString(AudioDevice device) {
    switch (device) {
        case AudioPlayer::PHONE:
            return VLC::getInstance()->getDefaultPhoneOutput();
        case AudioPlayer::SPEAKER:
            return VLC::getInstance()->getDefaultSpeakerOutput();
    }
    return NULL;
}

const char* AudioPlayer::audioDeviceString(AudioDevice device) {
    switch (device) {
        case AudioPlayer::PHONE:
            return VLC::getInstance()->getDefaultPhoneDevice();
        case AudioPlayer::SPEAKER:
            return VLC::getInstance()->getDefaultSpeakerDevice();
    }
    return NULL;
}


void AudioPlayer::setAudioDevice(AudioDevice device) {
    const char *deviceName = audioDeviceString(device);
    if (deviceName == NULL) {
        //throw "Invalid device specified";
        // NULL device means use default output for this one, don't need to set anything
        return;
    }
    const char* output = audioOutputString(device);
    if (strcmp(output, curOutput)) {
        // we need to swap output too, besides device
        int playTime = -1;
        if (libvlc_media_player_is_playing(audioPlayer)) {
            // if player is still playing, save the current play time
            playTime = libvlc_media_player_get_time(audioPlayer);
            libvlc_media_player_stop(audioPlayer);
        }
        if (libvlc_audio_output_set(audioPlayer, output) == -1) {
            // we couldn't swap output
            throw "ERROR setting output\n";
        }
        curOutput = output;
        // and swap device
        libvlc_audio_output_device_set(audioPlayer, output, deviceName);
        if (playTime != -1) {
            // and resume at the previously saved time
            libvlc_media_player_set_time(audioPlayer, playTime);
            libvlc_media_player_play(audioPlayer);
        }
    }else {
        // we only need to swap device, this is simple
        libvlc_audio_output_device_set(audioPlayer, output, deviceName);
    }
}

void AudioPlayer::playAudio(int audioIndex) {
    playAudio(audioIndex, 0);
}

void AudioPlayer::playAudio(int audioIndex, float position) {
    libvlc_media_player_stop(audioPlayer);
    libvlc_media_t* media = audioList->getAudio(audioIndex);
    if (!media) {
        this->audioIndex = 0;
        return;
    }
    this->audioIndex = audioIndex;
    libvlc_media_player_set_media(audioPlayer, media);
	libvlc_media_player_play(audioPlayer);
    if (position) {
        libvlc_media_player_set_position(audioPlayer, position);
    }
}


AudioPlayer* AudioPlayer::swapWith(AudioPlayer* other) {
    int audioIndex = other->getAudioIndex();
    float position = other->getAudioPosition();
    other->playAudio(getAudioIndex(), getAudioPosition());
    playAudio(audioIndex, position);
    return other;
}

int AudioPlayer::getAudioIndex() {
    return audioIndex;
}

float AudioPlayer::getAudioPosition() {
    if (!libvlc_media_player_is_playing(audioPlayer)) {
        return 0;
    }
    return (float)libvlc_media_player_get_position(audioPlayer);
}
