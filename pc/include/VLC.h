#ifndef VLC_H
#define VLC_H

#define DEFAULT_OUTPUT_DEVICE "waveout"
#define PHONE_DEVICE "Headset (Sound Blaster Tactic(3 ($ffff,$ffff)"
#define SPEAKER_DEVICE "Luidsprekers (High Definition A ($1,$ffff)"

#include "vlc/vlc.h"
#include <string.h>

using namespace std;

class VLC {
public:
    static VLC* getInstance();
    static void deleteInstance();

    VLC();
    virtual ~VLC();

    const char* getDefaultPhoneOutput();
    const char* getDefaultSpeakerOutput();

    libvlc_media_t* newMediaFromPath(const char* path);
    libvlc_media_list_t* newMediaList();
    libvlc_media_player_t* newMediaPlayer();
    libvlc_media_list_player_t* newMediaListPlayer();
protected:
    libvlc_instance_t* libvlcInstance;
private:
    char* phoneOutput;
    char* speakerOutput;

    static VLC* instance;
};

#endif // VLC_H
