#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H


#include "vlc/vlc.h"
#include <string.h>
#include <list>
#include <vector>
#include <functional>
#include <thread>

#include "VLC.h"
#include "AudioList.h"
#include "Tools.h"

using namespace std;

class AudioPlayer {
public:
    enum AudioDevice {
        PHONE,
        SPEAKER
    };
    typedef boost::signals2::signal<void ()> endSignal;

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

    bool isPlaying();
    bool isBusy();
    int getAudioIndex();
    float getAudioPosition();

    void onEnd(std::function<void ()> callback);
    void callback(int eventType);
protected:
    GUI &gui;
    string curOutput;
    endSignal endSig;

    static string audioOutputString(AudioDevice device);
    static string audioDeviceString(AudioDevice device);

    AudioList &audioList;
private:
    libvlc_media_player_t *audioPlayer;
    int audioIndex;

    AudioPlayer(const AudioPlayer &that) = delete;
};

#endif // AUDIOPLAYER_H
