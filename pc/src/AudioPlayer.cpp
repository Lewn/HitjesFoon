#include "AudioPlayer.h"


AudioPlayer::AudioPlayer(GUI *gui, AudioDevice device, AudioList *audioList) : gui(gui), audioList(audioList), audioIndex(0) {
    VLC *VLCInstance = VLC::getInstance();
    //Load the VLC engine
    audioPlayer = VLCInstance->newMediaPlayer();

    setAudioDevice(device);
}

AudioPlayer::~AudioPlayer() {
    VLC::release(audioPlayer);
}


string AudioPlayer::audioOutputString(AudioDevice device) {
    switch (device) {
        case AudioPlayer::PHONE:
            return VLC::getInstance()->getDefaultPhoneOutput();
        case AudioPlayer::SPEAKER:
            return VLC::getInstance()->getDefaultSpeakerOutput();
    }
    return NULL;
}

string AudioPlayer::audioDeviceString(AudioDevice device) {
    switch (device) {
        case AudioPlayer::PHONE:
            return VLC::getInstance()->getDefaultPhoneDevice();
        case AudioPlayer::SPEAKER:
            return VLC::getInstance()->getDefaultSpeakerDevice();
    }
    return NULL;
}


void AudioPlayer::setAudioDevice(AudioDevice device) {
    string deviceName = audioDeviceString(device);
    if (deviceName.empty()) {
        //throw "Invalid device specified";
        // empty device means use default output for this one, don't need to set anything
        return;
    }
    string output = audioOutputString(device);
    if (output.compare(curOutput)) {
        // we need to swap output too, besides device
        int playTime = -1;
        if (libvlc_media_player_is_playing(audioPlayer)) {
            // if player is still playing, save the current play time
            playTime = libvlc_media_player_get_time(audioPlayer);
            libvlc_media_player_stop(audioPlayer);
        }
        if (libvlc_audio_output_set(audioPlayer, output.c_str()) == -1) {
            // we couldn't swap output
            throw "ERROR setting output\n";
        }
        curOutput = output;
        // and swap device
        libvlc_audio_output_device_set(audioPlayer, output.c_str(), deviceName.c_str());
        if (playTime != -1) {
            // and resume at the previously saved time
            libvlc_media_player_set_time(audioPlayer, playTime);
            libvlc_media_player_play(audioPlayer);
        }
    } else {
        // we only need to swap device, this is simple
        libvlc_audio_output_device_set(audioPlayer, output.c_str(), deviceName.c_str());
    }
}

bool AudioPlayer::playAudio(int audioIndex) {
    return playAudio(audioIndex, 0);
}

bool AudioPlayer::playAudio(int audioIndex, float position) {
    libvlc_media_t *media = audioList->getAudio(audioIndex);
    bool success = playAudio(media, position);
    this->audioIndex = audioIndex;
    return success;
}

bool AudioPlayer::playAudio(libvlc_media_t *media) {
    return playAudio(media, 0);
}

bool AudioPlayer::playAudio(libvlc_media_t *media, float position) {
    stop();
    if (media == NULL) {
        return false;
    }
    libvlc_media_player_set_media(audioPlayer, media);
    libvlc_media_player_play(audioPlayer);
    if (position) {
        libvlc_media_player_set_position(audioPlayer, position);
    }
    return true;
}

void AudioPlayer::stop() {
    libvlc_media_player_stop(audioPlayer);
    audioIndex = 0;
}


AudioPlayer *AudioPlayer::swapWith(AudioPlayer *other) {
    int audioIndex = other->getAudioIndex();
    float position = other->getAudioPosition();
    other->playAudio(getAudioIndex(), getAudioPosition());
    playAudio(audioIndex, position);
    return other;
}

void AudioPlayer::setVolume(int volume) {
    libvlc_audio_set_volume(audioPlayer, volume);
}

int AudioPlayer::getVolume() {
    return libvlc_audio_get_volume(audioPlayer);
}

bool AudioPlayer::isPlaying() {
    return libvlc_media_player_is_playing(audioPlayer);
}

int AudioPlayer::getAudioIndex() {
    return audioIndex;
}

float AudioPlayer::getAudioPosition() {
    if (!isPlaying()) {
        return 0;
    }
    return (float)libvlc_media_player_get_position(audioPlayer);
}


void AudioPlayer::attachEventListener(AudioPlayerEventListener *listener) {
    if (listeners.empty()) {
        libvlc_event_manager_t *eventManager = libvlc_media_player_event_manager(audioPlayer);
        libvlc_event_attach(eventManager, libvlc_MediaPlayerEndReached, callback, this);
    }
    gui->printlevel(LDEBUG, "Attached a listener\n");
    listeners.push_back(listener);
}

void AudioPlayer::callback(const libvlc_event_t *evt, void *userData) {
    AudioPlayer *player = ((AudioPlayer*)userData);
    switch (evt->type) {
        case libvlc_MediaPlayerEndReached:
            player->gui->printlevel(LDEBUG, "\n\nSong ended");
            player->notificate(AudioPlayerEventListener::DONE);
            break;
        default:
            player->gui->printlevel(LDEBUG, "\n\nUnkown event triggered\n");
    }
}
void AudioPlayer::notificate(AudioPlayerEventListener::Event eventType) {
    gui->printlevel(LDEBUG, "\nNotificating %d\n", (int)listeners.size());
    for (AudioPlayerEventListener  *listener : listeners) {
        listener->audioPlayerEvent(eventType, this);
    }
}
