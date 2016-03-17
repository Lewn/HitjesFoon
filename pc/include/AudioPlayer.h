#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

class AudioPlayer;

#include "vlc/vlc.h"
#include <string.h>
#include <list>
#include <vector>

#include "VLC.h"
#include "AudioList.h"
#include "AudioPlayerEventListener.h"
#include "Tools.h"

using namespace std;

class AudioPlayer {
public:
    enum AudioDevice {
        PHONE,
        SPEAKER
    };

    AudioPlayer(GUI &gui, AudioDevice device, AudioList &audioList);
    virtual ~AudioPlayer();

    void setAudioDevice(AudioDevice device);
    bool playAudio(int audioIndex);
    bool playAudio(int audioIndex, float position);
    bool playAudio(libvlc_media_t *media);
    bool playAudio(libvlc_media_t *media, float position);

    void pause();
    void resume();
    void stop();

    AudioPlayer *swapWith(AudioPlayer *other);

    void setVolume(const int volume);
    int getVolume();

    void attachEventListener(AudioPlayerEventListener *listener);

    bool isPlaying();
    int getAudioIndex();
    float getAudioPosition();
protected:
    GUI &gui;
    string curOutput;
    list<AudioPlayerEventListener*> listeners;

    static string audioOutputString(AudioDevice device);
    string audioDeviceString(AudioDevice device);

    AudioList &audioList;

    static void callback (const libvlc_event_t *evt, void *userData);
    void notificate(AudioPlayerEventListener::Event eventType);
private:
    libvlc_media_player_t *audioPlayer;
    int audioIndex;

    AudioPlayer(const AudioPlayer &that) = delete;
};

#endif // AUDIOPLAYER_H
