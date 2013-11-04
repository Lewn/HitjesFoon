#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include "vlc/vlc.h"
#include <string.h>

#include "VLC.h"
#include "AudioList.h"

class AudioPlayer {
public:
    enum AudioDevice {
        PHONE,
        SPEAKER
    };

    AudioPlayer(AudioDevice device, AudioList* audioList);
    virtual ~AudioPlayer();

    void setAudioDevice(AudioDevice device);
    void playAudio(int audioIndex);
    void playAudio(int audioIndex, int time);
    AudioPlayer* swapWith(AudioPlayer* other);
protected:
    const char* curOutput;

    static const char* audioOutputString(AudioDevice device);
    const char* audioDeviceString(AudioDevice device);

    AudioList* audioList;

    int getAudioIndex();
    int getAudioTime();
private:
    libvlc_media_player_t *audioPlayer;
    int audioIndex;
};

#endif // AUDIOPLAYER_H
