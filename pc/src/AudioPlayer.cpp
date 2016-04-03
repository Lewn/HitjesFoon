#include "AudioPlayer.h"


void callbackStatic(const libvlc_event_t *event, void *userData) {
    // Pass event to member function
    AudioPlayer *thisPlayer = (AudioPlayer*)userData;
    int eventType = event->type;
    thread([ = ]() {
        // Call threaded as to not block VLC
        thisPlayer->callback(eventType);
    }).detach();
}


AudioPlayer::AudioPlayer(GUI &gui, AudioDevice device, AudioList &audioList) : gui(gui), audioList(audioList), audioIndex(0) {
    VLC *VLCInstance = VLC::getInstance();
    //Load the VLC engine
    audioPlayer = VLCInstance->newMediaPlayer();

    setAudioDevice(device);
    // Attach to VLC events
    libvlc_event_manager_t *eventManager = libvlc_media_player_event_manager(audioPlayer);
    for (int type : {VLC_EVENTS_MEDIAPLAYER}) {
        if (libvlc_event_attach(eventManager, type, &callbackStatic, this)) {
            gui.printlevel(LERROR, "Couldn't attach listener\n");
        }
    }
}

AudioPlayer::~AudioPlayer() {
    libvlc_event_manager_t *eventManager = libvlc_media_player_event_manager(audioPlayer);
    for (int type : {libvlc_MediaPlayerPlaying}) {
        libvlc_event_detach(eventManager, type, &callbackStatic, this);
    }
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
    libvlc_media_t *media = audioList.getHitje(audioIndex).getMediaData();
    bool success = playAudio(media, position);
    VLC::release(media);
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
    if (position != 0) {
        libvlc_media_player_set_position(audioPlayer, position);
    }
    return true;
}

void AudioPlayer::pause() {
    if (isPlaying() && libvlc_media_player_can_pause(audioPlayer)) {
        libvlc_media_player_set_pause(audioPlayer, true);
    }
}

void AudioPlayer::resume() {
    // Check audio index if there is even a hitje to resume
    if (!isPlaying() && isBusy()) {
        libvlc_media_player_set_pause(audioPlayer, false);
    }
}

void AudioPlayer::stop() {
    if (isBusy()) {
        libvlc_media_player_stop(audioPlayer);
    }
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

bool AudioPlayer::isBusy() {
    libvlc_state_t state = libvlc_media_player_get_state(audioPlayer);
    switch (state) {
        case libvlc_Opening:
        case libvlc_Buffering:
        case libvlc_Playing:
        case libvlc_Paused:
            return true;
        case libvlc_NothingSpecial:
        case libvlc_Stopped:
        case libvlc_Ended:
        case libvlc_Error:
            return false;
    }
    throw "Invalid audio player state";
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


void AudioPlayer::onEnd(std::function<void ()> callback) {
    endSig.connect(callback);
}


void AudioPlayer::callback(int eventType) {
    switch (eventType) {
        case libvlc_MediaPlayerMediaChanged:
            // Directly when libvlc_media_player_set_media is called
            break;
        case libvlc_MediaPlayerOpening:
            // When a new hitje is started
            break;
        case libvlc_MediaPlayerBuffering:
            // Once on load, then starting, then continuously until whole hitje is loaded
            // May be dependent on the resource location (online will take longer)
            break;
        case libvlc_MediaPlayerSeekableChanged:
            // Once on load
            break;
        case libvlc_MediaPlayerLengthChanged:
            // Once on load
            break;
        case libvlc_MediaPlayerTimeChanged:
        case libvlc_MediaPlayerPositionChanged:
            // Triggered continuously during playback, ignore
            // Notably only trigger after whole buffering is done
            break;
        case libvlc_MediaPlayerPaused:
            // Manually paused playback
            // TODO gui events only from speaker audio player?
            gui.setPaused();
            break;
        case libvlc_MediaPlayerPlaying:
            // When started playing, or when resumed from pause
            gui.setPlaying();
            break;
        case libvlc_MediaPlayerStopped:
        // Manually stopped playback
        case libvlc_MediaPlayerEndReached:
            // Reached end of song
            audioIndex = 0;
            gui.setStopped();
            endSig();
            break;
        case libvlc_MediaPlayerEncounteredError:
            // Some error?
            gui.printlevel(LERROR, "Media player encountered some error?\n%s\n\n", libvlc_errmsg());
            libvlc_clearerr();
        default:
            gui.printlevel(LINFO, "Got event of type %s\n", libvlc_event_type_name(eventType));
    }
}
